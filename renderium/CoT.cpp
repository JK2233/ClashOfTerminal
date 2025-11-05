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
#include "unitManagement.cpp"





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
    currentPlayerTurn = 1;
    //MONEY!
    playersCash[0] = 100;
    playersCash[1] = 100;
    playersIncome[0] = 10;
    playersIncome[1] = 10;
    /**/
    {
        std::vector<render::Texel> tmpTexture;
        render::Texel tmpTexel;
        tmpTexel.character = U'W';
        tmpTexel.bacColor = 250;
        tmpTexel.forColor = 100;
        tmpTexture.push_back(tmpTexel);
        tmpTexture.push_back(tmpTexel);
        tmpTexture.push_back(tmpTexel);
        tmpTexel.character = U'Z';
        tmpTexture.push_back(tmpTexel);
        tmpTexture.push_back(tmpTexel);
        tmpTexture.push_back(tmpTexel);
        tmpTexel.character = U'K';
        tmpTexture.push_back(tmpTexel);
        tmpTexture.push_back(tmpTexel);
        tmpTexture.push_back(tmpTexel);

        render::AddTextureTemplate("test", 3, 3, 0, tmpTexture);
    }
    /**/
    while (gameRunning) {
        {

            playerTurn(currentPlayerTurn); //<--- Uncomment this to test the movement of arrows on the map
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
                    if(UNITS[i].player == 1){
                        render::AddPoint(detectUnitType(UNITS[i].unitType), UNITS[i].tileID/29 + 2, UNITS[i].tileID%29*2+2, 100, 16);
                    }
                    else {
                        render::AddPoint(detectUnitType(UNITS[i].unitType), UNITS[i].tileID/29 + 2, UNITS[i].tileID%29*2+2, 150, 16);
                    }
                }

                render::AddCursor(U'▉', CURSOR/29 + 2, CURSOR%29*2 + 2, 100, 16);
                render::AddPoint(detectUnitType(SELECTED_UNIT), 16, 31*2,  100,  16);

            }

            /**/
            render::AddTextureInstance(40, 40, "test");
            /**/
            render::EndFrame();

            std::this_thread::sleep_for(std::chrono::microseconds(1)); // Sleep 16 ms
        }
    }
    render::DeInitialize();
}
