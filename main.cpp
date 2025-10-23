#include <iostream>
#include <vector>
#include <string>
#include <fstream>

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
    int numID = 0;
    while (!mapadiasz.eof()) {
        std::string line;
        getline(mapadiasz, line);
        std::cout << "LINIA" << "\n" << line << "\n";
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


    for (int i = 0; i < MAP.size(); i++) {
        if(i % 29 == 0) {
            std::cout << "\n";
        }
        if (MAP[i].tileType == e_Plains) {
            std::cout << "-";
        }
        else if(MAP[i].tileType == e_Water) {
            std::cout << "~";
        }
        else if (MAP[i].tileType == e_Base) {
            std::cout << "U";
        }
        else if (MAP[i].tileType == e_MainBase) {
            std::cout << "O";
        }
        else if (MAP[i].tileType == e_Bridge) {
            std::cout << "|";
        }
        else {
            std::cout << " ";
        }
    }
}
