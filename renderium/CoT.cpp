//MSVC exclusive pragma
#include <cstdio>
#pragma execution_character_set("utf-8")

//includes
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

#define COMPILE_WITH_TOOLS

#define RENDER_VERSION "b_0.4.0"

#define TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING false
#define SCREEN_SIZE_X 200
#define SCREEN_SIZE_Y 60 // 120

#include "render.h"




#ifdef __linux__

#endif
#ifdef _WIN32

#endif


int main(int argc, char* argv[])
{
    render::Log("initializing the render namespace");
    render::Initialize();

    render::LockRenderThread.lock();
    // render::VideoStreamMenager bootVideo = render::VideoStreamMenager("./bad apple/frame-#####.bmp", 6549, render::VideoStreamMenager::e_BMP);
    // bootVideo.exportAsDzadzV("./badExport/apple.dzadzV", true);
    render::VideoStreamMenager bootVideo2 = render::VideoStreamMenager("./badExport/apple.dzadzV", 6549, render::VideoStreamMenager::e_DzadzV);
    
    render::VideoStreems.push_back(bootVideo2);
    render::ObjectsToRender.clear();
    render::LockRenderThread.unlock();

    static bool gameRunning = true;
    while (gameRunning) {
        {
            render::ScreenObject tmp;
            tmp.type = render::e_NewFrame;
            if (render::renderThreadBusIdle) {
                render::LockRenderThread.lock();
                render::ObjectsToRender.clear();
                render::ObjectsToRender.push_back(tmp);
                tmp.type = render::e_Square;
                tmp.posX = 0;
                tmp.posY = 0;
                tmp.sizeX = 255;
                tmp.SizeY = 255;
                tmp.character = U'🮕';
                tmp.bacColor = 0;
                tmp.forColor = 250;
                render::ObjectsToRender.push_back(tmp);
                tmp.type = render::e_Video;
                tmp.resorceID = 0;
                render::ObjectsToRender.push_back(tmp);
                //tmp.type = render::e_BadApple;
                //render::ObjectsToRender.push_back(tmp);
                render::LockRenderThread.unlock();
                render::mainThreadFinnishedDataTransfer = true;
            }
            std::this_thread::sleep_for (std::chrono::milliseconds(15)); // Sleep 16 ms
        }
    }
    render::DeInitialize();
    return 0;
}