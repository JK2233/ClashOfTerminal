#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <sys/types.h>
#pragma execution_character_set("utf-8")
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "render_UTF_and_Loging_utils.cpp"

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
    uint8_t player;
    uint8_t movesLeft;
    enum UnitTypes unitType;

    uint16_t unitID; //Mabye to remove? whats the point of this
    uint16_t tileID;
};

struct Tile {
    int tileID;
    enum TileTypes tileType;
};

std::vector<Tile> MAP;
std::vector<Unit> UNITS;

int CURSOR = 0;
uint8_t currentPlayerTurn;
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

inline uint8_t setMoves(UnitTypes unit){
    switch (unit) {
        case e_Artillery:
            return 1;
        case e_Farm:
            return 1;
        case e_Infantry:
            return 1;
        case e_Tank:
            return 3;
        default:
            render::ReportError("Unknown unit type hes entered the battle fild (missingno?)", true, true);
            return 0;
    }
}

void spawnUnit(uint16_t place, UnitTypes unit){
    Unit u = {currentPlayerTurn, setMoves(unit), unit, (uint16_t)UNITS.size(), place, };
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

void moveUnit(){
    for(uint16_t i = 0; i < UNITS.size(); i++){
        if(CURSOR == UNITS[i].tileID){
            if(UNITS[i].movesLeft != 0){
                uint16_t unitIdToDelete = 10000;
                UNITS[i].movesLeft--;
                char32_t komenda = rawInput::readKey();
                if (komenda == 1000 || komenda == 'w') { //move arrow up
                    if (UNITS[i].tileID >= 29) {
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID - 29){
                                if(UNITS[i].player != UNITS[j].player){
                                    render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                                    unitIdToDelete = j;
                                }
                                else { // ILLEGAL MOVE
                                    UNITS[i].movesLeft++;
                                    render::Log("Unit from" +  std::to_string(UNITS[i].tileID) + " tried to do an illegal move");
                                    isMoving++;
                                }
                                break;
                            }
                        }
                        if(isMoving == 0){
                            UNITS[i].tileID -= 29;
                            CURSOR -= 29;
                            render::Log("UNIT MOVED TO: " + std::to_string(UNITS[i].tileID));
                            render::Log("\n");
                        }
                    }
                }
                else if (komenda == 1001 || komenda == 's') {
                    if (UNITS[i].tileID <= (MAP.size() - 1) - 29) { //move arrow down
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID + 29){
                                if(UNITS[i].player != UNITS[j].player){
                                    render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                                    unitIdToDelete = j;
                                }
                                else { // ILLEGAL MOVE
                                    UNITS[i].movesLeft++;
                                    render::Log("Unit from" +  std::to_string(UNITS[i].tileID) + " tried to do an illegal move");
                                    isMoving++;
                                }
                                break;
                            }
                        }
                        if(isMoving == 0){
                            UNITS[i].tileID += 29;
                            CURSOR += 29;
                            render::Log("UNIT MOVED TO: " + std::to_string(UNITS[i].tileID));
                            render::Log("\n");
                        }
                    }
                }
                else if (komenda == 1002 || komenda == 'd') { //move arrow right
                    if (UNITS[i].tileID != MAP.size() - 1) {
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID + 1){
                                if(UNITS[i].player != UNITS[j].player){
                                    render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                                    unitIdToDelete = j;
                                }
                                else { // ILLEGAL MOVE
                                    UNITS[i].movesLeft++;
                                    render::Log("Unit from" +  std::to_string(UNITS[i].tileID) + " tried to do an illegal move");
                                    isMoving++;
                                }
                                break;
                            }
                        }
                        if(isMoving == 0){
                            UNITS[i].tileID++;
                            CURSOR++;
                            render::Log("UNIT MOVED TO: " + std::to_string(UNITS[i].tileID));
                            render::Log("\n");
                        }
                    }
                }
                else if (komenda == 1003 || komenda == 'a') { //move arrow left
                    if (UNITS[i].tileID != 0) {
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID - 1){
                                if(UNITS[i].player != UNITS[j].player){
                                    render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID) + " " + std::to_string(UNITS[i].player));
                                    unitIdToDelete = j;
                                }
                                else { // ILLEGAL MOVE
                                    UNITS[i].movesLeft++;
                                    render::Log("Unit from" +  std::to_string(UNITS[i].tileID) + " tried to do an illegal move");
                                    isMoving++;
                                }
                                break;
                            }
                        }
                        if(isMoving == 0){
                            UNITS[i].tileID--;
                            CURSOR--;
                            render::Log("UNIT MOVED TO: " + std::to_string(UNITS[i].tileID));
                            render::Log("\n");
                        }
                    }
                }
                if(unitIdToDelete != 10000){
                    UNITS.erase(UNITS.begin() + unitIdToDelete);
                }
            }

            break;
        }
    }
}
void endTurn(){
    if(currentPlayerTurn == 1){
        currentPlayerTurn++;
    }
    else{
        currentPlayerTurn--;
    }
    for(uint_fast16_t i = 0; i < UNITS.size(); i++){
        UNITS[i].movesLeft = setMoves(UNITS[i].unitType);
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
    else if(komenda == 'm'){
        moveUnit();
    }
    else if(komenda == 'e'){
        endTurn();
    }
    #ifdef TRUEDEBUG
    updateMap(CURSOR);
    #endif
}

