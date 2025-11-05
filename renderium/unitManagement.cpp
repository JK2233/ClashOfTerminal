#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <locale>
#include <sys/types.h>
#include <type_traits>
#pragma execution_character_set("utf-8")
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "render_UTF_and_Loging_utils.cpp"

enum UnitTypes : uint8_t{ // WHEN CHECKING THIS, EDIT VALUE ASSIGNING | check ctrl + f -> STATS ASSIGNING
    e_Infantry = 0,
    e_LightTank,
    e_MedTank,
    e_HeavyTank,
    e_Artillery,
    e_ATArtilery,
    e_Marines,
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
    //Unit general (moves left is here cause its how many moves does a unit have left so its not really a stat)
    uint8_t player;
    uint8_t movesLeft;
    enum UnitTypes unitType;

    //Unit info
    uint16_t unitID; //Mabye to remove? whats the point of this
    uint16_t tileID;

    //Unit stats
    uint8_t health;
    uint8_t damage;
    uint8_t range;
    uint8_t price; // i have realised that this is pointless uh, mabye to remove later?
};

struct Tile {
    int tileID;
    enum TileTypes tileType;
    int8_t bridge = -1;
};

//Map/Units vector
std::vector<Tile> MAP;
std::vector<Unit> UNITS;

int CURSOR = 0;


//Player information
uint8_t currentPlayerTurn;
uint16_t playersCash[2];
uint16_t playersIncome[2];

//"Showcase" variables (for the players to see next to the game meny)
UnitTypes SELECTED_UNIT = e_Artillery;


//Generating the base for the map (see mapa.diasz for the map)
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
                switch(numID) {

                    //LEFT BRIDGE

                    case 381:
                        t1.bridge = 1;
                        break;
                    case 382:
                        t1.bridge = 1;
                        break;
                    case 383:
                        t1.bridge = 1;
                        break;
                    case 410:
                        t1.bridge = 1;
                        break;
                    case 411:
                        t1.bridge = 1;
                        break;
                    case 412:
                        t1.bridge = 1;
                        break;
                    case 439:
                        t1.bridge = 1;
                        break;
                    case 440:
                        t1.bridge = 1;
                        break;
                    case 441:
                        t1.bridge = 1;
                        break;
                    case 468:
                        t1.bridge = 1;
                        break;
                    case 469:
                        t1.bridge = 1;
                        break;
                    case 470:
                        t1.bridge = 1;
                        break;

                        //RIGHT BRIDGE
                    case 393:
                        t1.bridge = 2;
                        break;
                    case 394:
                        t1.bridge = 2;
                        break;
                    case 395:
                        t1.bridge = 2;
                        break;
                    case 422:
                        t1.bridge = 2;
                        break;
                    case 423:
                        t1.bridge = 2;
                        break;
                    case 424:
                        t1.bridge = 2;
                        break;
                    case 451:
                        t1.bridge = 2;
                        break;
                    case 452:
                        t1.bridge = 2;
                        break;
                    case 453:
                        t1.bridge = 2;
                        break;
                    case 480:
                        t1.bridge = 2;
                        break;
                    case 481:
                        t1.bridge = 2;
                        break;
                    case 482:
                        t1.bridge = 2;
                        break;
                }
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

//STATS (+ moves) ASSIGNING

inline uint8_t setMoves(UnitTypes unit){
    switch (unit) {
        case e_Artillery:
            return 1;
        case e_Farm:
            return 1;
        case e_Infantry:
            return 1;
        case e_LightTank:
            return 3;
        case e_MedTank:
            return 2;
        case e_HeavyTank:
            return 1;
        case e_ATArtilery:
            return 1;
        case e_Marines:
            return 2;
        default:
            render::ReportError("Unknown unit type hes entered the battle fild (missingno?)", true, true);
            return 0;
    }
}

char32_t detectUnitType(UnitTypes unit){
    switch (unit) {
        case e_Artillery:
            return U'A';
        case e_Infantry:
            return U'I';
        case e_Farm:
            return U'F';
        case e_LightTank:
            return U'L';
        case e_MedTank:
            return U'M';
        case e_HeavyTank:
            return U'H';
        case e_ATArtilery:
            return U'T';
        case e_Marines:
            return U'W';
        default:
            return U'"';
    }
}
uint8_t assignStrenght(UnitTypes unit){
    switch (unit) {
        case e_Artillery:
            return  0;
        case e_Infantry:
            return 5;
        case e_Farm:
            return 0;
        case e_LightTank:
            return 10;
        case e_MedTank:
            return 15;
        case e_HeavyTank:
            return 20;
        case e_ATArtilery:
            return 5;
        case e_Marines:
            return 5;
        default:
            return 0;
    }
}

uint8_t assingHealth(UnitTypes unit){
        switch (unit) {
        case e_Artillery:
            return 5;
        case e_Infantry:
            return 10;
        case e_Farm:
            return 1;
        case e_LightTank:
            return 30;
        case e_MedTank:
            return 50;
        case e_HeavyTank:
            return 90;
        case e_ATArtilery:
            return 5;
        case e_Marines:
            return 10;
        default:
            return 0;
    }
}

uint8_t assignCost(UnitTypes unit){
            switch (unit) {
        case e_Artillery:
            return 50;
        case e_Infantry:
            return 10;
        case e_Farm:
            return 10;
        case e_LightTank:
            return 50;
        case e_MedTank:
            return 100;
        case e_HeavyTank:
            return 200;
        case e_ATArtilery:
            return 75;
        case e_Marines:
            return 25;
        default:
            return 0;
    }
}

uint8_t assingRange(UnitTypes unit){
        switch (unit) {
        case e_Artillery:
            return 3;
        case e_Farm:
            return 0;
        case e_ATArtilery:
            return 2;
        default:
            return 1;
    }
}

//
// end of stats assigning
//

// Unit spawning

void spawnUnit(uint16_t place, UnitTypes unit){
    if(playersCash[currentPlayerTurn-1] > assignCost(unit)){ //If the player has cash
        playersCash[currentPlayerTurn-1] -= assignCost(unit);
        Unit u = {currentPlayerTurn, setMoves(unit), unit, (uint16_t)UNITS.size(), place, assingHealth(unit), assignStrenght(unit), assingRange(unit), assignCost(unit)};
        UNITS.push_back(u);
    }
    else { //If the player doesnt have cash
        render::Log("Player" + std::to_string(currentPlayerTurn) + " did not have enough money: " + std::to_string(playersCash[currentPlayerTurn-1]));
    }
}

//debug function for infinite cash (technicaly speaking it could overflow, and it will 100% but i dont care its for debug only)

void cashExploit(){
    playersCash[0] = 65535;
    playersCash[1] = 65535;
    playersIncome[0] = 65535;
    playersIncome[1] = 65535;
}


bool isWaterTile(int tile) {
    if(MAP[tile].tileType == e_Water) return true;

    return false;
}

//The bad function
void moveUnit(){
    for(uint16_t i = 0; i < UNITS.size(); i++){ //checking if the cursor is on a tile with a unit
        if(CURSOR == UNITS[i].tileID){ //up
            if(UNITS[i].movesLeft != 0){ //if the unit has moves left
                uint16_t unitIdToDelete = 10000; //placeholder
                UNITS[i].movesLeft--;
                char32_t komenda = rawInput::readKey();
                if (komenda == 1000 || komenda == 'w') { //move arrow up
                    if(isWaterTile(UNITS[i].tileID - 29) || UNITS[i].unitType == e_Marines) {
                        UNITS[i].movesLeft++;
                        break;
                    }
                    if (UNITS[i].tileID >= 29) {
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID - 29){ //is the tile occupied?
                                if(UNITS[i].player != UNITS[j].player && (UNITS[i].unitType != e_ATArtilery || UNITS[i].unitType != e_Artillery)){ //is it an ally or not, and is it arti or not
                                    if(UNITS[j].health <= UNITS[i].damage){  //enemy - can we kill it?
                                        render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                                        unitIdToDelete = j;
                                    }
                                    else {
                                        UNITS[j].health -= UNITS[i].damage; //enemy - it survives
                                        render::Log("The unit is left at: " + std::to_string(UNITS[j].health) + "hp");
                                        isMoving++;
                                    }
                                }
                                else { // ally - ILLEGAL MOVE
                                    UNITS[i].movesLeft++;
                                    render::Log("Unit from" +  std::to_string(UNITS[i].tileID) + " tried to do an illegal move");
                                    isMoving++;
                                }
                                break;
                            }
                        }
                        if(isMoving == 0){ //if the isMoving variable didnt change, the unit is allowed to move
                            UNITS[i].tileID -= 29;
                            CURSOR -= 29;
                            render::Log("UNIT MOVED TO: " + std::to_string(UNITS[i].tileID));
                            render::Log("\n");
                        }
                    }
                }
                else if (komenda == 1001 || komenda == 's') { //Same as everything above, but for moving down
                    if(isWaterTile(UNITS[i].tileID + 29) || UNITS[i].unitType == e_Marines) { 
                        UNITS[i].movesLeft++; 
                        break; 
                    }
                    if (UNITS[i].tileID <= (MAP.size() - 1) - 29) { //move arrow down
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID + 29){
                                if(UNITS[i].player != UNITS[j].player && (UNITS[i].unitType != e_ATArtilery || UNITS[i].unitType != e_Artillery)){
                                    if(UNITS[j].health <= UNITS[i].damage){
                                        render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                                        unitIdToDelete = j;
                                    }
                                    else {
                                        UNITS[j].health -= UNITS[i].damage;
                                        render::Log("The unit is left at: " + std::to_string(UNITS[j].health) + "hp");
                                        isMoving++;
                                    }
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
                    if(isWaterTile(UNITS[i].tileID + 1) || UNITS[i].unitType == e_Marines) { 
                        UNITS[i].movesLeft++; 
                        break; 
                    }
                    if (UNITS[i].tileID != MAP.size() - 1) {
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID + 1){
                                if(UNITS[i].player != UNITS[j].player && (UNITS[i].unitType != e_ATArtilery || UNITS[i].unitType != e_Artillery)){
                                    if(UNITS[j].health <= UNITS[i].damage){
                                        render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                                        unitIdToDelete = j;
                                    }
                                    else {
                                        UNITS[j].health -= UNITS[i].damage;
                                        render::Log("The unit is left at: " + std::to_string(UNITS[j].health) + "hp");
                                        isMoving++;
                                    }
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
                    if(isWaterTile(UNITS[i].tileID - 1) || UNITS[i].unitType == e_Marines) { 
                        UNITS[i].movesLeft++; 
                        break;
                    }
                    if (UNITS[i].tileID != 0) {
                        uint8_t isMoving = 0; // 0 - yes; 1 - no
                        for(int j = 0; j < UNITS.size(); j++){
                            if(UNITS[j].tileID == UNITS[i].tileID - 1){
                                if(UNITS[i].player != UNITS[j].player && (UNITS[i].unitType != e_ATArtilery || UNITS[i].unitType != e_Artillery)){
                                    if(UNITS[j].health <= UNITS[i].damage){
                                        render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                                        unitIdToDelete = j;
                                    }
                                    else {
                                        UNITS[j].health -= UNITS[i].damage;
                                        render::Log("The unit is left at: " + std::to_string(UNITS[j].health) + "hp");
                                        isMoving++;
                                    }
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
                if(unitIdToDelete != 10000){ //if the variable was changed, then an unit was killed, so we remove it
                    UNITS.erase(UNITS.begin() + unitIdToDelete);
                }
            }

            break;
        }
    }
}

//The 2nd bad function
void unitShooting(){
    for(uint16_t i = 0; i < UNITS.size(); i++){ //checking if the cursor is on a tile with a unit
        if(CURSOR == UNITS[i].tileID){ //up
            if(UNITS[i].movesLeft != 0){ //if the unit has moves left
                if(UNITS[i].range == 0){
                    return;
                }
                else if(UNITS[i].range == 1){
                    moveUnit();
                    return;
                }
                UNITS[i].movesLeft--;
                for(uint8_t j = 0; j < UNITS[i].range; j++){
                    char32_t komenda = rawInput::readKey();
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
                    else{
                        break;
                    }
                    // I forgot to ask how to do it whoops
                    // render::AddPoint(detectUnitType(UNITS[i].unitType), UNITS[i].tileID/29 + 2, UNITS[i].tileID%29*2+2, 150, 16);

                }
                uint8_t dmgMod = 1;
                for(uint16_t j = 0; j < UNITS.size(); j++){
                    if(CURSOR == UNITS[j].tileID && UNITS[j].player != UNITS[i].player){
                        if(UNITS[i].unitType == e_ATArtilery && (UNITS[j].unitType == e_LightTank || UNITS[j].unitType == e_MedTank ||UNITS[j].unitType == e_HeavyTank)){
                            dmgMod = 6;
                        }
                        if(UNITS[j].health <= UNITS[i].damage * dmgMod){
                            render::Log("Unit from " + std::to_string(UNITS[j].tileID) + " has been deleted by " + std::to_string(UNITS[i].tileID));
                            UNITS.erase(UNITS.begin() + j);
                            return;
                        }
                        else {
                            UNITS[j].health -= UNITS[i].damage * dmgMod;
                            render::Log("The unit is left at: " + std::to_string(UNITS[j].health) + "hp");
                            return;
                        }
                    }
                }      
                UNITS[i].movesLeft++;
                }
                break;
            }
    }
}


void BridgeCashCheckOut() {
        uint8_t numOfP1UnitsOnBridge1 = 0;
        uint8_t numOfP1UnitsOnBridge2 = 0;
        uint8_t numOfP2UnitsOnBridge1 = 0;
        uint8_t numOfP2UnitsOnBridge2 = 0;
        for(uint16_t i = 0; i < UNITS.size(); i++) {
            if(UNITS[i].player == currentPlayerTurn && MAP[UNITS[i].tileID].bridge > 0) {
                if(MAP[UNITS[i].tileID].bridge == 1) {
                    if(currentPlayerTurn == 1) {
                        numOfP1UnitsOnBridge1++;
                    } else {
                        numOfP2UnitsOnBridge1++;
                    }
                } else {
                   if(currentPlayerTurn == 1) {
                        numOfP1UnitsOnBridge2++;
                    } else {
                        numOfP2UnitsOnBridge2++;
                    }
                }
            }
        }
        if(numOfP1UnitsOnBridge1 > numOfP2UnitsOnBridge1) {
            playersCash[0] += 10;
        } else if(numOfP1UnitsOnBridge1 < numOfP2UnitsOnBridge1){
            playersCash[1] += 10;
        }
        if(numOfP1UnitsOnBridge2 > numOfP2UnitsOnBridge2) { 
            playersCash[0] += 15;
        } else if(numOfP1UnitsOnBridge2 < numOfP2UnitsOnBridge2){
            playersCash[1] += 15;
        }
        render::Log("kasa gracza 1: "+ std::to_string(playersCash[0]));
        render::Log("kasa gracza 2: "+ std::to_string(playersCash[1]));
}

//Cleanup(move reset + income + player change)
void endTurn(){
    playersCash[currentPlayerTurn-1] += playersIncome[currentPlayerTurn-1]; //the player can be 1 or 2
    if(currentPlayerTurn == 1){
        currentPlayerTurn++;
    }
    else{
        BridgeCashCheckOut();
        currentPlayerTurn--;
    }
    for(uint_fast16_t i = 0; i < UNITS.size(); i++){
        UNITS[i].movesLeft = setMoves(UNITS[i].unitType);
    }
    render::Log("kasa gracza 1: "+ std::to_string(playersCash[0]));
    render::Log("kasa gracza 2: "+ std::to_string(playersCash[1]));
}
//reading inputs from the player
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
        //     SELECTED_UNIT = e_LightTank;
        //     break;
        // case e_LightTank:
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
    else if(komenda == 'q'){
        cashExploit();
    }
    else if(komenda == 'x'){
        unitShooting();
    }
    #ifdef TRUEDEBUG
    updateMap(CURSOR);
    #endif
}