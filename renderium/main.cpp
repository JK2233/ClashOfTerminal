#pragma execution_character_set("utf-8")
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define COMPILE_WITH_TOOLS

#define RENDER_VERSION "b_0.4.0"

#define TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING false
#define SCREEN_SIZE_X 200
#define SCREEN_SIZE_Y 60

#define SCREEN_PIXEL_COUNT SCREEN_SIZE_X*SCREEN_SIZE_Y
#define LOG_OBJECTS_RENDERED false

#include "render.h"


enum UnitTypes {
   e_Infantry,
   e_Tank,
   e_Artillery,
   e_Farm
};

enum TileTypes {
    e_Plains,
    e_Bridge,
    e_MainBase,
    e_Base,
    e_Water
};

struct Unit {
    int player, unitID, tileID;
    std::string nazwa;
    enum UnitTypes unitType;
};

struct Tile {
    int tileID;
    enum TileTypes tileType;
};

std::vector<Tile> MAP;

void GenerateMap() {
    std::ifstream mapadiasz;
    mapadiasz.open("mapa.diasz");
    if(!mapadiasz.is_open()){
        render::Log("error with file opening");
        return;
    }
    int numID = 0;
    while (!mapadiasz.eof()) {
        std::string line;
        getline(mapadiasz, line);
        #ifdef TRUEDEBUG
        std::cout << "LINIA" << "\n" << line << "\n";
        #endif
        for (int i = 0; i < line.length(); i++) {
            Tile t1;
            
            //HAHAHAHAHHAHAHAHHAHAHHA
            switch (line[i])
            {
            case '-':
                t1.tileID = ++numID;
                t1.tileType = e_Plains;
                MAP.push_back(t1);
                break;
            case '~':
                t1.tileID = ++numID;
                t1.tileType = e_Water;
                MAP.push_back(t1);
                break;
            case 'U':
                t1.tileID = ++numID;
                t1.tileType = e_Base;
                MAP.push_back(t1);
                break;
            case 'O':
                t1.tileID = ++numID;
                t1.tileType = e_MainBase;
                MAP.push_back(t1);
                break;
            case '|':
                t1.tileID = ++numID;
                t1.tileType = e_Bridge;
                MAP.push_back(t1);
                break;
            default:
                break;
            }
        }
        numID+=100;
    }
    mapadiasz.close();
}

void playerTurn(int playerNum) {

}


int main()
{
    render::Log("initializing the render namespace");
    render::Initialize();

    GenerateMap();
    
    render::Log("EEEEEEEEEEEEEEEEEEEEEEEE");
    render::Log(std::to_string((MAP.size())));
    for (int i = 0; i < MAP.size(); i++) {
        if(i % 29 == 0) {
            render::Log("\n");
        }
        if (MAP[i].tileType == e_Plains) {
            render::Log("-");
        }
        else if(MAP[i].tileType == e_Water) {
            render::Log("~");
        }
        else if (MAP[i].tileType == e_Base) {
            render::Log("U");
        }
        else if (MAP[i].tileType == e_MainBase) {
            render::Log("O");
        }
        else if (MAP[i].tileType == e_Bridge) {
            render::Log("|");
        }
        else {
            render::Log(" ");
        }
    }

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

}
