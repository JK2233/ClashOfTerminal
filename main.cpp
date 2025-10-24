#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#ifdef UNITED
#include "gate2.h"
#endif

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
      nieMain();
}
