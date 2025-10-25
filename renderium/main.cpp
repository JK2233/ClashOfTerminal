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
    //std::string nazwa;
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
        //std::cout << "LINIA" << "\n" << line << "\n";
        #endif
        for (int i = 0; i < line.length(); i++) {
            Tile t1;
            
            //HAHAHAHAHHAHAHAHHAHAHHA
            switch (line[i])
            {
            case '-':
                t1.tileID = numID++;
                t1.tileType = e_Plains;
                MAP.push_back(t1);
                break;
            case '~':
                t1.tileID = numID++;
                t1.tileType = e_Water;
                MAP.push_back(t1);
                break;
            case 'U':
                t1.tileID = numID++;
                t1.tileType = e_Base;
                MAP.push_back(t1);
                break;
            case 'O':
                t1.tileID = numID++;
                t1.tileType = e_MainBase;
                MAP.push_back(t1);
                break;
            case '|':
                t1.tileID = numID++;
                t1.tileType = e_Bridge;
                MAP.push_back(t1);
                break;
            default:
                break;
            }
        }
    }
    mapadiasz.close();
}

void updateMap(int id) { //this function is for debuging until map render will be ready
    std::string mapBuffer = "";
    for (int i = 0; i < MAP.size(); i++) {
        if(i % 29 == 0) {
            mapBuffer += "\n";
        }
        if (MAP[i].tileType == e_Plains) {
            if(MAP[i].tileID == id) {
                mapBuffer += " Y";
            } else {
                mapBuffer += " -";
            }
        }
        else if(MAP[i].tileType == e_Water) {
            if(MAP[i].tileID == id) {
                mapBuffer += " Y";
            } else {
                mapBuffer += " ~";
            }
        }
        else if (MAP[i].tileType == e_Base) {
            if(MAP[i].tileID == id) {
                mapBuffer += " Y";
            } else {
                mapBuffer += " U";
            }
        }
        else if (MAP[i].tileType == e_MainBase) {
            if(MAP[i].tileID == id) {
                mapBuffer += " Y";
            } else {
                mapBuffer += " O";
            }
        }
        else if (MAP[i].tileType == e_Bridge) {
            if(MAP[i].tileID == id) {
                mapBuffer += " Y";
            } else {
                mapBuffer += " |";
            }
        }
        else {
            mapBuffer += " ";
        }
    }
    render::Log(mapBuffer);
    render::Log("\n");
}

void playerTurn(int playerNum) {
    /*
        1000 // arrow up
        1001 // arrow down
        1002 // arrow right
        1003 // arrow left
    */
    int id = 0;
    rawInput::changeTerminalState(0);
    while(true) {
        int komenda = rawInput::readKey();
        if((char)komenda == 'f')/*press f every time you finnish moving on the map so it won't destroy your terminal (It should't but do it still) */ { //<---- I can turn it into a switch later
            break;
        } else if(komenda == 1000) { //move arrow up
            if(id > 29) {
                id -= 29;
                render::Log("id: " +std::to_string(id));
                render::Log("\n");
                updateMap(id);
            }
        } else if(komenda == 1001) {
            if(id < (MAP.size()-1)-29) { //move arrow down
                id += 29;
                render::Log("id: " +std::to_string(id));
                render::Log("\n");
                updateMap(id);
            }
        } else if(komenda == 1002) { //move arrow right
            if(id != MAP.size() -1) {
                id++;
                render::Log("id: " +std::to_string(id));
                render::Log("\n");
                updateMap(id);
            }
        } else if(komenda == 1003) { //move arrow left
            if(id != 0) {
                id--;
                render::Log("id: " +std::to_string(id));
                render::Log("\n");
                updateMap(id);
            }
        }
    }
    rawInput::resetTerminal();
}


int main()
{
    render::Log("initializing the render namespace");
    render::Initialize();

    GenerateMap();
    //playerTurn(2); //<--- Uncomment this to test the movement of arrows on the map
    
    render::Log("EEEEEEEEEEEEEEEEEEEEEEEE");
    render::Log(std::to_string((MAP.size())));
    std::string mapBuffer = "";
    for (int i = 0; i < MAP.size(); i++) {
        if(i % 29 == 0) {
            mapBuffer += "\n";
        }
        if (MAP[i].tileType == e_Plains) {
            mapBuffer += " -";
        }
        else if(MAP[i].tileType == e_Water) {
            mapBuffer += " ~";
        }
        else if (MAP[i].tileType == e_Base) {
            mapBuffer += " U";
        }
        else if (MAP[i].tileType == e_MainBase) {
            mapBuffer += " O";
        }
        else if (MAP[i].tileType == e_Bridge) {
            mapBuffer += " |";
        }
        else {
            mapBuffer += " ";
        }
    }
    render::Log(mapBuffer + "\n");

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
