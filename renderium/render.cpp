#pragma execution_character_set("utf-8")

//includes
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <uchar.h>
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>


#include "render_UTF_and_Loging_utils.cpp"
#include "Video_and_Texture_menagers.cpp"

#define COMPILE_WITH_TOOLS

#define RENDER_VERSION "b_0.4.0"

#define TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING false
#define SCREEN_SIZE_X 200
#define SCREEN_SIZE_Y 60

#define SCREEN_PIXEL_COUNT SCREEN_SIZE_X*SCREEN_SIZE_Y
#define LOG_OBJECTS_RENDERED false

namespace render
{
enum ScreenObjectType : uint8_t
{
    //this should always be the first "ScreenObject" of a new frame
    e_NewFrame,
    //renders a single char on the screen
    e_Point,
    //Texture render a texture from resorceID, those textures are stored in a seprate txt file
    e_Texture,
    //renders a Texture but only within a set Square
    e_SquareMaskedTexture,
    //renders a Texture but only within a set Elipsis
    e_ElipsisMaskedTexture,
    //renders a Texture but only within a set Circle, sizeX is the redius of it
    e_CircleMaskedTexture,
    //LabelObjects are a string of text on the screen, text is stored in a seprate buffer of label strings
    e_Label,
    //Overrides the BacColor Screen Buffer        
    e_BacOverlay,
    //Overrides the ForColor Screen Buffer
    e_ForOverlay,
    //a Square on the screen
    e_Square,
    //an Elipsis on the screen
    e_Elipsis,
    //an Circle on the screen, sizeX is the redius of it
    e_Circle,
    //Line from posX/Y to a point offset by sizeX/Y, var defines width of the line
    e_LineTo,
    //Bad Apple player from txt files, used for testing of the rendering pipeline
    e_BadApple,
    //Renders a video from a VideoStreamMenager object
    e_Video
};
std::string getNameOfScreenObjectType(ScreenObjectType type) {
    std::string ret = "Unknown";
    switch (type)
    {
    case e_NewFrame:
        ret = "NewFrame";
        break;
    case e_Point:
        ret = "Point";
        break;
    case e_Texture:
        ret = "Texture";
        break;
    case e_SquareMaskedTexture:
        ret = "SquareMaskedTexture";
        break;
    case e_ElipsisMaskedTexture:
        ret = "ElipsisMaskedTexture";
        break;
    case e_CircleMaskedTexture:
        ret = "CircleMaskedTexture";
        break;
    case e_Label:
        ret = "Label";
        break;
    case e_BacOverlay:
        ret = "BacOverlay";
        break;
    case e_ForOverlay:
        ret = "ForOverlay";
        break;
    case e_Square:
        ret = "Square";
        break;
    case e_Elipsis:
        ret = "Elipsis";
        break;
    case e_Circle:
        ret = "Circle";
        break;
    case e_LineTo:
        ret = "LineTo";
        break;
    case e_BadApple:
        ret = "BadApple";
        break;
    }
    return ret;
}

struct ScreenObject
{
    ScreenObjectType type;
    uint8_t var;
    uint16_t resorceID;
    int16_t posX;
    int16_t posY;
    char32_t character;
    uint8_t bacColor;
    uint8_t forColor;
    uint8_t sizeX;
    uint8_t SizeY;


    ScreenObject() {
        type = e_Texture;
        var = 0;
        resorceID = 0;
        posX = 0;
        posY = 0;
        character = U'￼';
        bacColor = 0;
        forColor = 255;
    };
    ScreenObject
    (
        ScreenObjectType _type,
        uint8_t _var,
        uint16_t _resorceID,
        int16_t _posX,
        int16_t _posY,
        char32_t _character,
        uint8_t _bacColor,
        uint8_t _forColor
    )
    {
        type = _type;
        var = _var;
        resorceID = _resorceID;
        posX = _posX;
        posY = _posY;
        character = _character;
        bacColor = _bacColor;
        forColor = _forColor;
    };
};

std::atomic<bool> renderThreadBusIdle = false;
std::atomic<bool> mainThreadFinnishedDataTransfer = false;
std::mutex LockRenderThread;

std::vector<ScreenObject> ObjectsToRender;
std::vector<VideoStreamMenager> VideoStreems;


ScreenObjectType* objectTypeBuffer;
uint8_t* objectVarBuffer;
uint16_t* objectresorceIDBuffer;
int16_t* objectPosXBuffer;
int16_t* objectPosYBuffer;
char32_t* objectCharacterBuffer;
uint8_t* objectBacColorBuffer;
uint8_t* objectForColorBuffer;
uint8_t* objectSizeXBuffer;
uint8_t* objectSizeYBuffer;

char32_t* screenCharBuffer;
uint8_t* screenBacColorBuffer;
uint8_t* screenForColorBuffer;

std::string screenOutputBuffer;

std::int32_t AnimationFrameCount = 0;
std::int64_t FrameCount = 0;
std::int64_t PhysicsFrameCount = 0;
std::int64_t FramesSinceCls = 0;
bool RelyOnCLS = false;
time_t timeSincePreviousFPSCalculation;
std::int64_t FramesSinceFPSCalculation;
double FPS = 0.0;
#ifdef _WIN32
    std::chrono::time_point<std::chrono::steady_clock> previousFrameTime = std::chrono::high_resolution_clock::now();
#else
    std::chrono::time_point previousFrameTime = std::chrono::high_resolution_clock::now();
#endif // _WIN32


    
namespace internal{
    //This function needs to be called by the main thread to kick start rendering
    inline void Initialize() {
        while (!renderThreadBusIdle) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        mainThreadFinnishedDataTransfer = true;
        return;
    }
    inline void cleenScreenBuffer() {
        for (int_fast32_t i = 0; i < SCREEN_PIXEL_COUNT; i++)
        {
            screenCharBuffer[i] = U' ';
            screenBacColorBuffer[i] = 0;
            screenForColorBuffer[i] = 255;
        }
    }
    inline void cookScreenBuffers() {
        screenOutputBuffer = "";
        static uint8_t previousBacColor = 0;
        static uint8_t previousForColor = 255;

        for (int_fast32_t y = 0; y < SCREEN_SIZE_Y; y++)
        {
            for (int_fast16_t x = 0; x < SCREEN_SIZE_X; x++)
            {
                if (previousBacColor != screenBacColorBuffer[y * SCREEN_SIZE_X + x])
                {
                    screenOutputBuffer += "\033[48;5;" + std::to_string(screenBacColorBuffer[y * SCREEN_SIZE_X + x]) + "m";
                    previousBacColor = screenBacColorBuffer[y * SCREEN_SIZE_X + x];
                }
                if (previousForColor != screenForColorBuffer[y * SCREEN_SIZE_X + x])
                {
                    screenOutputBuffer += "\033[38;5;" + std::to_string(screenForColorBuffer[y * SCREEN_SIZE_X + x]) + "m";
                    previousForColor = screenForColorBuffer[y * SCREEN_SIZE_X + x];
                }
                screenOutputBuffer += codeToUTF8(screenCharBuffer[y * SCREEN_SIZE_X + x]);
            }
            if (y < (SCREEN_SIZE_Y))
            {
                screenOutputBuffer += "\n";
            }
        }
        return;
    }
    inline void displayScreenOutputBuffer() {
        if (timeSincePreviousFPSCalculation != time(NULL))
        {
            int64_t timePassed = time(NULL) - timeSincePreviousFPSCalculation;
            double FrameCountDouble = FrameCount;
            double timePassedDouble = timePassed;
            FPS = FramesSinceFPSCalculation / timePassedDouble;
            timeSincePreviousFPSCalculation = time(NULL);
            FramesSinceFPSCalculation = 0;
        }
        else
        {
            FramesSinceFPSCalculation++;
        }

        if (FramesSinceCls >= 10 && RelyOnCLS) {
            FramesSinceCls = 0;
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
                
            std::cout << screenOutputBuffer << std::endl;
        }
        else
        {
            FramesSinceCls++;
            std::string tmp = "\033[H";
            tmp += screenOutputBuffer;
            std::cout << tmp << std::endl;
        }
    }
    inline void renderScreenObject(int_fast16_t idx) {
        if (LOG_OBJECTS_RENDERED) {
            std::string tmp = "currently rendering ScreenObject at idx: ";
            tmp += std::to_string(idx);
            Log(tmp);
        }
        int_fast32_t X = objectPosXBuffer[idx];
        int_fast32_t Y = objectPosYBuffer[idx];
        switch (objectTypeBuffer[idx]) {
        case e_NewFrame:
            screenCharBuffer[1] = U'ę';
            break;
        case e_Point:

            break;
        case e_Texture:

            break;
        case e_SquareMaskedTexture:

            break;
        case e_ElipsisMaskedTexture:

            break;
        case e_CircleMaskedTexture:

            break;
        case e_Label:

            break;
        case e_BacOverlay:

            break;
        case e_ForOverlay:

            break;
        case e_Square:
            for (int_fast32_t i = 0; i < objectSizeYBuffer[idx]; ++i) {
                for (int_fast32_t j = 0; j < objectSizeXBuffer[idx]; ++j) {
                    if (X < 0 || Y < 0 || Y >= SCREEN_SIZE_Y || X >= SCREEN_SIZE_X) {
                        continue;
                    }
                    if (objectCharacterBuffer[idx] != U' ')
                    {
                        screenCharBuffer[(Y * SCREEN_SIZE_X) + X] = objectCharacterBuffer[idx];
                        screenForColorBuffer[(Y * SCREEN_SIZE_X) + X] = objectForColorBuffer[idx];
                    }
                    screenBacColorBuffer[(Y * SCREEN_SIZE_X) + X] = objectBacColorBuffer[idx];
                    X++;
                }
                Y++;
                X = objectPosXBuffer[idx];
            }
            break;
        case e_Elipsis:

            break;
        case e_Circle:

            break;
        case e_LineTo:

            break;
        case e_BadApple:
        {
            std::ifstream BadAppleFrame;
            std::string pathToframe = "./frames-ascii/out";
            std::string tmpFileName = "";
            if (FrameCount < 10) {
                tmpFileName += "000" + std::to_string(FrameCount) + ".jpg.txt";
            }
            else if (FrameCount < 100) {
                tmpFileName += "00" + std::to_string(FrameCount) + ".jpg.txt";
            }
            else if (FrameCount < 1000) {
                tmpFileName += "0" + std::to_string(FrameCount) + ".jpg.txt";
            }
            else {
                tmpFileName += std::to_string(FrameCount) + ".jpg.txt";
            }
            pathToframe += tmpFileName;
            Log(pathToframe);
            BadAppleFrame.open(pathToframe);
            if (BadAppleFrame.is_open()) {
                for (int i = 0; i < 36; ++i) {
                    std::string line;
                    getline(BadAppleFrame, line);
                    for (int_fast16_t j = 0; j < line.size(); ++j) {
                        if (j < SCREEN_SIZE_X) {
                            screenCharBuffer[i * SCREEN_SIZE_X + j] = line[j];
                            screenBacColorBuffer[i * SCREEN_SIZE_X + j] = 0;
                            screenForColorBuffer[i * SCREEN_SIZE_X + j] = 255;
                        }
                    }
                }
            }
            else {
                Log("file not found");
            }
            BadAppleFrame.close();
            // getchar();
        }
            break;
        case e_Video:
            for (int_fast16_t i = 0; i < SCREEN_SIZE_Y; ++i)
            {
                for (int_fast16_t j = 0; j < SCREEN_SIZE_X; ++j) {
                    int16_t color = VideoStreems[objectresorceIDBuffer[idx]].getColorAtFrame(i * 2, j);
                    if (color > -1)
                    {
                        screenBacColorBuffer[(i * SCREEN_SIZE_X) + j] = (uint8_t)color;
                    }
                    color = VideoStreems[objectresorceIDBuffer[idx]].getColorAtFrame(i * 2 + 1, j);
                    if (color > -1)
                    {
                        screenCharBuffer[(i * SCREEN_SIZE_X) + j] = U'▄';
                        screenForColorBuffer[(i * SCREEN_SIZE_X) + j] = (uint8_t)color;
                    }
                }
            }
            VideoStreems[objectresorceIDBuffer[idx]].frameNO++;
            objectCharacterBuffer[idx]++;
            break;
        default:
            std::string tmp = "Unrecognized objectType detected at idx: ";
            tmp += std::to_string(idx) + "\nobject position is " + std::to_string(objectPosXBuffer[idx]) + "/" + std::to_string(objectPosYBuffer[idx]);
            tmp += "\ntypeidx: " + std::to_string(objectTypeBuffer[idx]) + "\nTHIS SHOULD BE FIXED";
            ReportError(tmp, true);
            break;
        }
        return;
    }

    static int renderThreadMain() {
        //Pre render loop
        //std::setlocale(LC_CTYPE, ".UTF-8");
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(NULL);
#ifdef _WIN32
        system("chcp 65001");
#endif // Win32
        std::cout << "test ęąśążźóó,    ♔ ♔♔👍\n" << "version: " << RENDER_VERSION << "\n";
        std::cout.flush();
        screenCharBuffer = new char32_t[SCREEN_SIZE_X * SCREEN_SIZE_Y];
        screenBacColorBuffer = new uint8_t[SCREEN_SIZE_X * SCREEN_SIZE_Y];
        screenForColorBuffer = new uint8_t[SCREEN_SIZE_X * SCREEN_SIZE_Y];
        objectTypeBuffer = new ScreenObjectType[4096];
        objectVarBuffer = new uint8_t[4096];
        objectresorceIDBuffer = new uint16_t[4096];
        objectPosXBuffer = new int16_t[4096];
        objectPosYBuffer = new int16_t[4096];
        objectCharacterBuffer = new char32_t[4096];
        objectBacColorBuffer = new uint8_t[4096];
        objectForColorBuffer = new uint8_t[4096];
        objectSizeXBuffer = new uint8_t[4096];
        objectSizeYBuffer = new uint8_t[4096];
        timeSincePreviousFPSCalculation = time(NULL);
        //done seting up wait for the main thread to start its work
        renderThreadBusIdle = true;

        //main render loop
        int_fast16_t amountOfObjects = 0;
        while (true) {
#ifdef _WIN32
            std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::high_resolution_clock::now();
#else
            std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();
#endif // _WIN32
            if (currentTime - previousFrameTime > std::chrono::microseconds(33333))
            //if (true)
            {
                previousFrameTime = currentTime;
                AnimationFrameCount++;
                FrameCount++;
                if (mainThreadFinnishedDataTransfer == true)
                {
                    //acknowlage reciving the data
                    mainThreadFinnishedDataTransfer = false;
                    render::Log("Render thread recived data from main thread, starting data transfer to internal buffers");

                    //work asigned, coppy data to internal buffers and work on it
                    AnimationFrameCount = 0;
                    PhysicsFrameCount++;
                    {
                        LockRenderThread.lock();
                        size_t sizeOfObjectsToRender = ObjectsToRender.size();
                        if (sizeOfObjectsToRender > UINT16_MAX) { //there has to be a limit
                            ReportError("Exceded the maximum amount of Objects to render in one frame", true, true);

                            //If the program hasn't aborted yet,
                            //don't coppy everything and hope for the best
                            sizeOfObjectsToRender = UINT16_MAX;
                        }
                        amountOfObjects = (int_fast16_t)sizeOfObjectsToRender;
                        delete[] objectTypeBuffer;
                        delete[] objectVarBuffer;
                        delete[] objectresorceIDBuffer;
                        delete[] objectPosXBuffer;
                        delete[] objectPosYBuffer;
                        delete[] objectCharacterBuffer;
                        delete[] objectBacColorBuffer;
                        delete[] objectForColorBuffer;
                        delete[] objectSizeXBuffer;
                        delete[] objectSizeYBuffer;

                        objectTypeBuffer = new ScreenObjectType[sizeOfObjectsToRender];
                        objectVarBuffer = new uint8_t[sizeOfObjectsToRender];
                        objectresorceIDBuffer = new uint16_t[sizeOfObjectsToRender];
                        objectPosXBuffer = new int16_t[sizeOfObjectsToRender];
                        objectPosYBuffer = new int16_t[sizeOfObjectsToRender];
                        objectCharacterBuffer = new char32_t[sizeOfObjectsToRender];
                        objectBacColorBuffer = new uint8_t[sizeOfObjectsToRender];
                        objectForColorBuffer = new uint8_t[sizeOfObjectsToRender];
                        objectSizeXBuffer = new uint8_t[sizeOfObjectsToRender];
                        objectSizeYBuffer = new uint8_t[sizeOfObjectsToRender];

                        for (int_fast16_t i = 0; i < sizeOfObjectsToRender; ++i)
                        {
                            objectTypeBuffer[i] = ObjectsToRender[i].type;
                            objectVarBuffer[i] = ObjectsToRender[i].var;
                            objectresorceIDBuffer[i] = ObjectsToRender[i].resorceID;
                            objectPosXBuffer[i] = ObjectsToRender[i].posX;
                            objectPosYBuffer[i] = ObjectsToRender[i].posY;
                            objectCharacterBuffer[i] = ObjectsToRender[i].character;
                            objectBacColorBuffer[i] = ObjectsToRender[i].bacColor;
                            objectForColorBuffer[i] = ObjectsToRender[i].forColor;
                            objectSizeXBuffer[i] = ObjectsToRender[i].sizeX;
                            objectSizeYBuffer[i] = ObjectsToRender[i].SizeY;
                        }
                        LockRenderThread.unlock();
                    }

                    //now that the data is copied main thread can send more over
                    renderThreadBusIdle = true;

                    //Start rendering the objects to a back buffer.
                    cleenScreenBuffer();
                    for (int_fast16_t i = 0; i < amountOfObjects; ++i) {
                        renderScreenObject(i);
                    }
                    cookScreenBuffers();
                    displayScreenOutputBuffer();
                }
                else
                {
                    //Start rendering the objects to a back buffer.
                    cleenScreenBuffer();
                    for (int_fast16_t i = 0; i < amountOfObjects; ++i) {
                        renderScreenObject(i);
                    }
                    cookScreenBuffers();
                    displayScreenOutputBuffer();
                }
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        return 0;
    }
}
using namespace internal;
std::thread RenderThread(renderThreadMain);

inline void DeInitialize() {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    RenderThread.detach();
    return;
}
}
