#pragma execution_character_set("utf-8")
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define COMPILE_WITH_TOOLS

#define RENDER_VERSION "0.4.0"

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
    if (!mapadiasz.is_open()) {
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
        if (i % 29 == 0) {
            mapBuffer += "\n";
        }
        if (MAP[i].tileType == e_Plains) {
            if (MAP[i].tileID == id) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " -";
            }
        }
        else if (MAP[i].tileType == e_Water) {
            if (MAP[i].tileID == id) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " ~";
            }
        }
        else if (MAP[i].tileType == e_Base) {
            if (MAP[i].tileID == id) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " U";
            }
        }
        else if (MAP[i].tileType == e_MainBase) {
            if (MAP[i].tileID == id) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " O";
            }
        }
        else if (MAP[i].tileType == e_Bridge) {
            if (MAP[i].tileID == id) {
                mapBuffer += " Y";
            }
            else {
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

void playerTurn(uint8_t playerNum) {
    /*
        1000 // arrow up
        1001 // arrow down
        1002 // arrow right
        1003 // arrow left
    */

    int id = 0;
    int komenda = rawInput::readKey();
    if (komenda == 1000) { //move arrow up
        if (id > 29) {
            id -= 29;
            render::Log("id: " + std::to_string(id));
            render::Log("\n");
            updateMap(id);
        }
    }
    else if (komenda == 1001) {
        if (id < (MAP.size() - 1) - 29) { //move arrow down
            id += 29;
            render::Log("id: " + std::to_string(id));
            render::Log("\n");
            updateMap(id);
        }
    }
    else if (komenda == 1002) { //move arrow right
        if (id != MAP.size() - 1) {
            id++;
            render::Log("id: " + std::to_string(id));
            render::Log("\n");
            updateMap(id);
        }
    }
    else if (komenda == 1003) { //move arrow left
        if (id != 0) {
            id--;
            render::Log("id: " + std::to_string(id));
            render::Log("\n");
            updateMap(id);
        }
    }
}


int main()
{
    render::Log("initializing the render namespace");
    render::Initialize();

    GenerateMap();

    render::Log("EEEEEEEEEEEEEEEEEEEEEEEE");
    render::Log(std::to_string((MAP.size())));
    // render::VideoStreamMenager bootVideo = render::VideoStreamMenager("./bad apple/frame-#####.bmp", 6549, render::VideoStreamMenager::e_BMP);
    // bootVideo.exportAsDzadzV("./badExport/apple.dzadzV", true);
    //render::VideoStreamMenager bootVideo2 = render::VideoStreamMenager("./badExport/apple.dzadzV", 6549, render::VideoStreamMenager::e_DzadzV);
    //render::VideoStreems.push_back(bootVideo2);


    static bool gameRunning = true;
    while (gameRunning) {
        {

            render::StartNewFrame();
            playerTurn(2); //<--- Uncomment this to test the movement of arrows on the map

            {
                int16_t currentLine = 0;
                std::u32string mapBuffer = U"mapBuffer";

                for (int i = 0; i < MAP.size(); i++) {
                    if (i % 29 == 0) {
                        currentLine++;
                        render::AddLabel(mapBuffer, currentLine, 0, 255);
                        mapBuffer = U"";
                    }
                    if (MAP[i].tileType == e_Plains) {
                        mapBuffer += U" -";
                    }
                    else if (MAP[i].tileType == e_Water) {
                        mapBuffer += U" ~";
                    }
                    else if (MAP[i].tileType == e_Base) {
                        mapBuffer += U" U";
                    }
                    else if (MAP[i].tileType == e_MainBase) {
                        mapBuffer += U" O";
                    }
                    else if (MAP[i].tileType == e_Bridge) {
                        mapBuffer += U" |";
                    }
                    else {
                        mapBuffer += U" ";
                    }
                }
                currentLine++;
                render::AddLabel(mapBuffer, currentLine, 0, 255);

            }

            render::EndFrame();

            std::this_thread::sleep_for(std::chrono::microseconds(1)); // Sleep 16 ms
        }
    }
    render::DeInitialize();
}
