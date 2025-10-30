#include <cstdint>
#include <cstdio>
#pragma execution_character_set("utf-8")
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define COMPILE_WITH_TOOLS

//#define RENDER_VERSION "0.4.0"

#define TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING false
#define SCREEN_SIZE_X 200
#define SCREEN_SIZE_Y 60

#define SCREEN_PIXEL_COUNT SCREEN_SIZE_X*SCREEN_SIZE_Y
#define LOG_OBJECTS_RENDERED false

#include "render.h"


enum UnitTypes : uint8_t{
    e_Infantry = 0,
    e_Tank,
    e_Artillery,
    e_Farm //Remember child, thou shall not moveth thine e_Farms in mine Uniteth.
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
std::vector<Unit> UNITS;

int CURSOR = 0;
UnitTypes SELECTED_UNIT = e_Artillery;

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
#ifdef TRUEDEBUG
void updateMap(int CURSOR) { //this function is for debuging until map render will be ready
    std::string mapBuffer = "";
    for (int i = 0; i < MAP.size(); i++) {
        if (i % 29 == 0) {
            mapBuffer += "\n";
        }
        if (MAP[i].tileType == e_Plains) {
            if (MAP[i].tileID == CURSOR) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " -";
            }
        }
        else if (MAP[i].tileType == e_Water) {
            if (MAP[i].tileID == CURSOR) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " ~";
            }
        }
        else if (MAP[i].tileType == e_Base) {
            if (MAP[i].tileID == CURSOR) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " U";
            }
        }
        else if (MAP[i].tileType == e_MainBase) {
            if (MAP[i].tileID == CURSOR) {
                mapBuffer += " Y";
            }
            else {
                mapBuffer += " O";
            }
        }
        else if (MAP[i].tileType == e_Bridge) {
            if (MAP[i].tileID == CURSOR) {
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
#endif

void spawnUnit(int place, UnitTypes unit){
    Unit u = {1, (int)UNITS.size(), place, unit};
    UNITS.push_back(u);
}
char32_t detectUnitType(UnitTypes unit){
    switch (unit) {
        case e_Artillery:
            return U'A';
        case e_Infantry:
            return U'I';
        case e_Farm:
            return U'F';
        case e_Tank:
            return U'T';
        default:
            return U'"';
    }
}

void playerTurn(uint8_t playerNum) {
    /*
        1000 // arrow up
        1001 // arrow down
        1002 // arrow right
        1003 // arrow left
    */

    
    int komenda = rawInput::readKey();
    if (komenda == 1000 || komenda == 'w') { //move arrow up
        if (CURSOR >= 29) {
            CURSOR -= 29;
            render::Log("CURSOR: " + std::to_string(CURSOR));
            render::Log("\n");
        }
    }
    else if (komenda == 1001 || komenda == 's') {
        if (CURSOR <= (MAP.size() - 1) - 29) { //move arrow down
            CURSOR += 29;
            render::Log("CURSOR: " + std::to_string(CURSOR));
            render::Log("\n");
        }
    }
    else if (komenda == 1002 || komenda == 'd') { //move arrow right
        if (CURSOR != MAP.size() - 1) {
            CURSOR++;
            render::Log("CURSOR: " + std::to_string(CURSOR));
            render::Log("\n");
        }
    }
    else if (komenda == 1003 || komenda == 'a') { //move arrow left
        if (CURSOR != 0) {
            CURSOR--;
            render::Log("CURSOR: " + std::to_string(CURSOR));
            render::Log("\n");
        }
    }
    else if(komenda == 'u'){
        // switch (SELECTED_UNIT) {
        // case e_Artillery:
        //     SELECTED_UNIT = e_Infantry;
        //     break;
        // case e_Infantry:
        //     SELECTED_UNIT = e_Farm;
        //     break;
        // case e_Farm:
        //     SELECTED_UNIT = e_Tank;
        //     break;
        // case e_Tank:
        //     SELECTED_UNIT = e_Artillery;
        //     break;
        // default:
        //     SELECTED_UNIT = e_Infantry;
        //     break;
        SELECTED_UNIT = (UnitTypes)((uint8_t)SELECTED_UNIT + (uint8_t)1);
        if (SELECTED_UNIT > e_Farm) {
            SELECTED_UNIT = e_Infantry;
        }
    }
    else if (komenda == 'r'){
        spawnUnit(CURSOR, SELECTED_UNIT);
    }
    #ifdef TRUEDEBUG
    updateMap(CURSOR);
    #endif
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

            playerTurn(2); //<--- Uncomment this to test the movement of arrows on the map
            render::StartNewFrame();
            {
                int16_t currentLine = 0;
                std::u32string mapBuffer = U"mapBuffer";

                for (int i = 0; i < MAP.size(); i++) {
                    
                    if (i % 29 == 0) {
                        currentLine++;
                        render::AddLabel(mapBuffer, currentLine, 0, 255);
                        mapBuffer = U" ";
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
                }
                currentLine++;
                render::AddLabel(mapBuffer, currentLine, 0, 255);

                for(int i = 0; i < UNITS.size(); i++){
                    render::AddPoint(detectUnitType(UNITS[i].unitType), UNITS[i].tileID/29 + 2, UNITS[i].tileID%29*2+2, 100, 16);
                }

                render::AddPoint(U'▉', CURSOR/29 + 2, CURSOR%29*2 + 2, 100, 16);
                render::AddPoint(detectUnitType(SELECTED_UNIT), 16, 31*2,  100,  16);

            }

            render::EndFrame();

            std::this_thread::sleep_for(std::chrono::microseconds(1)); // Sleep 16 ms
        }
    }
    render::DeInitialize();
}
