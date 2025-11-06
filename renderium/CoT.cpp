#pragma execution_character_set("utf-8")
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>

#define COMPILE_WITH_TOOLS

//#define RENDER_VERSION "0.4.0"

#define TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING false
#define SCREEN_SIZE_X 200
#define SCREEN_SIZE_Y 45

#define SCREEN_PIXEL_COUNT SCREEN_SIZE_X*SCREEN_SIZE_Y
#define LOG_OBJECTS_RENDERED false

//produces alays the same squence unless you do something wrong, also don't do that cuz you will break it for all others
std::mt19937 StableRandom{ 15454 };
//std::random_device TrueRandom;
//std::mt19937 Random{ TrueRandom };


#include "render_UTF_and_Loging_utils.cpp"
#include "render.h"
#include "textures.cpp"
#include "unitManagement.cpp"




int main()
{
    render::Log("initializing the render namespace");
    render::Initialize();

    LoadTextures();

    GenerateMap();

    render::Log(std::to_string((MAP.size())));
    //render::VideoStreamMenager bootVideo = render::VideoStreamMenager("./bad apple/frame-#####.bmp", 6549, render::VideoStreamMenager::e_BMP);
    //bootVideo.exportAsDzadzV("./badExport/apple.dzadzV", true);
    //render::VideoStreamMenager bootVideo2 = render::VideoStreamMenager("./badExport/apple.dzadzV", 6549, render::VideoStreamMenager::e_DzadzV);
    //render::VideoStreems.push_back(bootVideo);
    
    static bool gameRunning = true;
    currentPlayerTurn = 1;
    //MONEY!
    playersCash[0] = 100;
    playersCash[1] = 100;
    playersIncome[0] = 20;
    playersIncome[1] = 20;

    while (gameRunning) {
        {

            playerTurn(currentPlayerTurn);
            render::StartNewFrame();
            {
                int16_t currentLine = 0;
                render::AddLabel(U"Tura gracza " + render::toUString(currentPlayerTurn), 0, 0, playersColors[currentPlayerTurn-1]);

                for (int i = 0; i < MAP.size(); i++) {
                    
                    if (i % 29 == 0) {
                        currentLine++;
                    }
                    if (MAP[i].tileType == e_Plains) {
                        render::AddTextureInstance(currentLine, (i % 29) * 2, "map_plains");
                    }
                    else if (MAP[i].tileType == e_Water) {
                        render::AddTextureInstance(currentLine, (i % 29) * 2, "map_water");
                    }
                    else if (MAP[i].tileType == e_Base) {
                        render::AddTextureInstance(currentLine, (i % 29) * 2, "map_base");
                    }
                    else if (MAP[i].tileType == e_MainBase) {
                        render::AddTextureInstance(currentLine, (i % 29) * 2, "map_mainBase");
                    }
                    else if (MAP[i].tileType == e_Bridge) {
                        render::AddTextureInstance(currentLine, (i % 29) * 2, "map_bridge");
                    }
                }
                currentLine++;
                uint8_t unitExisting[2] = {0, 0};
                for(int i = 0; i < UNITS.size(); i++){
                    if(UNITS[i].tileID == CURSOR){
                        lastShownUnits[UNITS[i].player-1] = UNITS[i];
                        unitExisting[UNITS[i].player-1]++;
                    }
                    else if(lastShownUnits[0].tileID == UNITS[i].tileID)
                    {
                        lastShownUnits[0] = UNITS[i];
                        unitExisting[0]++;
                    }
                    else if(lastShownUnits[1].tileID == UNITS[i].tileID)
                    {
                        lastShownUnits[1] = UNITS[i];
                        unitExisting[1]++;
                    }
                    uint_fast8_t bgColor = 16;
                    if(UNITS[i].unitID == 0){
                        bgColor = 117;
                    }
                    if(UNITS[i].player == 1){
                        render::AddPoint(detectUnitType(UNITS[i].unitType), UNITS[i].tileID/29 + 2, UNITS[i].tileID%29*2+2, playersColors[0], bgColor);
                    }
                    else {
                        render::AddPoint(detectUnitType(UNITS[i].unitType), UNITS[i].tileID/29 + 2, UNITS[i].tileID%29*2+2, playersColors[1], bgColor);
                    }
                }
                //Wyswietlanie akcji
                render::AddLabel(U"Dostępne akcje: m - ruch jednostki; x - strzał (artyleri); e - zakończ ture; r - kup jednostkę; u/y - zmień jednostkę; p - zamień farmera na farme", currentLine + 3, 0, 255);
                
                //Wyswietlanie pieniedzy -> pieniadze 1; income 1; --- pieniadze 2; income 2;
                render::AddLabel(U"Player 1 cash: " + render::toUString(playersCash[0]), 3, 60, playersColors[0]);
                render::AddLabel(U"Player 1 income: " + render::toUString(playersIncome[0]), 4,60, playersColors[0]);
                render::AddLabel(U"Player 2 cash: " + render::toUString(playersCash[1]), currentLine-1,60, playersColors[1]);
                render::AddLabel(U"Player 2 income: " + render::toUString(playersIncome[1]), currentLine,60, playersColors[1]);
                
                //Obecnie wybrana jednostka do zrespienia
                if(SELECTED_UNIT-1 < e_Infantry){
                    render::AddLabel(U"Poprzednia jednostka (Y): " + getUnitName((UnitTypes)(e_Farm)), 14, 31*2,  playersColors[currentPlayerTurn-1]);
                }
                else{
                    render::AddLabel(U"Poprzednia jednostka (Y): " + getUnitName((UnitTypes)(SELECTED_UNIT-1)), 14, 31*2,  playersColors[currentPlayerTurn-1]);
                }
                render::AddLabel(U"Obecnie wybrana jednostka: " + getUnitName((UnitTypes)SELECTED_UNIT), 15, 31*2,  playersColors[currentPlayerTurn-1]);
                if(SELECTED_UNIT+1 > e_Farm){
                    render::AddLabel(U"Kolejna jednostka (U): " + getUnitName((UnitTypes)(e_Infantry)), 16, 31*2,  playersColors[currentPlayerTurn-1]);
                }
                else {
                    render::AddLabel(U"Kolejna jednostka (U): " + getUnitName((UnitTypes)(SELECTED_UNIT+1)), 16, 31*2,  playersColors[currentPlayerTurn-1]);
                }

                //Wyświetlanie wszystkich dostępnych jednostek (i na które cie stać)
                render::AddLabel(U"Lista dostępnych jednostek", 11, 126, playersColors[currentPlayerTurn-1]);
                render::AddLabel(U"-->", 12 + SELECTED_UNIT, 126, 231);
                for(int i = 0; i <= e_Farm; i++){
                    //12
                    if(assignCost((UnitTypes)i) <= playersCash[currentPlayerTurn-1]){
                        render::AddLabel(getUnitName((UnitTypes)(i)) + U"; znak-> " + detectUnitType((UnitTypes)(i)) + U"; Koszt-> " + render::toUString(assignCost((UnitTypes)i)), 12+ i, 130, playersColors[currentPlayerTurn-1]);
                    }
                    else{
                        render::AddLabel(getUnitName((UnitTypes)(i)) + U"; znak-> " + detectUnitType((UnitTypes)(i)) + U"; Koszt-> " + render::toUString(assignCost((UnitTypes)i)), 12+ i, 130, 196);
                    }
                }

                //Staty jednostki na ktora najezdza kursor - druzyna 1
                if(lastShownUnits[0].health >= 0 && unitExisting[0] != 0 && lastShownUnits[0].unitID != 0){
                    render::AddLabel(U"Statystyki jednostki: ", 6, 60, playersColors[lastShownUnits[0].player-1]);
                    render::AddLabel(U"Rodzaj jednostki: " + getUnitName(lastShownUnits[0].unitType), 7, 60, playersColors[lastShownUnits[0].player-1]);
                    render::AddLabel(U"Zdrowie: " + render::toUString(lastShownUnits[0].health), 8, 60, playersColors[lastShownUnits[0].player-1]);
                    render::AddLabel(U"Zadawane obrażenia: " + render::toUString(lastShownUnits[0].damage), 9, 60, playersColors[lastShownUnits[0].player-1]);
                    render::AddLabel(U"Pozostały ruch: " + render::toUString(lastShownUnits[0].movesLeft), 10, 60, playersColors[lastShownUnits[0].player-1]);
                    render::AddLabel(U"Zasięg: " + render::toUString(lastShownUnits[0].range), 11, 60, playersColors[lastShownUnits[0].player-1]);
                    if(lastShownUnits[0].unitType == e_Artillery || lastShownUnits[0].unitType == e_ATArtilery){
                    render::AddLabel(U"Może atakować?: " + render::toUString(lastShownUnits[0].movesLeft), 12, 60, playersColors[lastShownUnits[0].player-1]);
                    }
                    else {
                    render::AddLabel(U"Może atakować?: " + render::toUString(lastShownUnits[0].movesLeft), 12, 60, playersColors[lastShownUnits[0].player-1]);  
                    }
                    render::AddPoint(detectUnitType(lastShownUnits[0].unitType), lastShownUnits[0].tileID/29 + 2, lastShownUnits[0].tileID%29*2+2, 16, playersColors[0]);
                }
                //Staty jednostki na ktora najechal kursor - druzyna 2
                if(lastShownUnits[1].health >= 0 && unitExisting[1] != 0 && lastShownUnits[0].unitID != 0){
                    render::AddLabel(U"Statystyki jednostki: ", 18, 60, playersColors[lastShownUnits[1].player-1]);
                    render::AddLabel(U"Rodzaj jednostki: " + getUnitName(lastShownUnits[1].unitType), 19, 60, playersColors[lastShownUnits[1].player-1]);
                    render::AddLabel(U"Zdrowie: " + render::toUString(lastShownUnits[1].health), 20, 60, playersColors[lastShownUnits[1].player-1]);
                    render::AddLabel(U"Zadawane obrażenia: " + render::toUString(lastShownUnits[1].damage), 21, 60, playersColors[lastShownUnits[1].player-1]);
                    render::AddLabel(U"Pozostały ruch: " + render::toUString(lastShownUnits[1].movesLeft), 22, 60, playersColors[lastShownUnits[1].player-1]);
                    render::AddLabel(U"Zasięg: " + render::toUString(lastShownUnits[1].range), 23, 60, playersColors[lastShownUnits[1].player-1]);
                    if(lastShownUnits[1].unitType == e_Artillery || lastShownUnits[1].unitType == e_ATArtilery){
                    render::AddLabel(U"Może atakować?: " + render::toUString(lastShownUnits[1].movesLeft), 24, 60, playersColors[lastShownUnits[1].player-1]);
                    }
                    else {
                    render::AddLabel(U"Może atakować?: " + render::toUString(lastShownUnits[1].movesLeft), 24, 60, playersColors[lastShownUnits[1].player-1]);  
                    }
                    render::AddPoint(detectUnitType(lastShownUnits[1].unitType), lastShownUnits[1].tileID/29 + 2, lastShownUnits[1].tileID%29*2+2, 16, playersColors[1]);
 
                }
                //Kursor
                render::AddCursor(U'▉', CURSOR/29 + 2, CURSOR%29*2 + 2, 100, 16);
                //
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
