#ifdef TRUEDEBUG
#include <iostream>
#endif
//LINUX LIBRARIES

#ifdef __unix__

#include <unistd.h>
#include <termios.h>

/* Initialize new terminal i/o settings */
namespace rawInput {
    static struct termios old, v_new;
    void changeTerminalState(int echo) {
        tcgetattr(0, &old); // backup old terminal settings
        v_new = old; // set new settings as old so it has a base to edit it from
        v_new.c_lflag &= ~ICANON; // turning off enter baisically
        v_new.c_lflag &= echo ? ECHO : ~ECHO; // letters wont show up in terminal if set to 0 (don't change)
        tcsetattr(0, TCSANOW, &v_new); // use these new terminal i/o settings now
    }

    // always put at the end of the program or else problems
    void resetTerminal(void) {
        tcsetattr(0, TCSANOW, &old); //TCSANOW - apply immidietly
    }


    int readKey() {
        char c[3];
        read(0, &c[0], 1);
        if (c[0] == 27) {

            //----------------------------------------------------------------------
            struct termios oldt, newt;                                                  //<--- I will put this to diffrent method later
            tcgetattr(STDIN_FILENO, &oldt);                               //you have to birng back the old terminal settings for read to work
            newt = oldt;
            newt.c_cc[VMIN] = 0;                                                        // set non-blocing mode
            newt.c_cc[VTIME] = 1;                                                       // time limit 0.1s

            tcsetattr(STDIN_FILENO, TCSANOW, &newt);    // set to new
            //----------------------------------------------------------------------
            read(0, &c[1], 2);
            //std::cout << c[1] << " i " << c[2];
            if (c[1] == '[') {
                //std::cout<<"IM LOSING MY MIND";
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt); //shit go back
                switch (c[2]) {
                case 'A':
                    return 1000; //arrow up
                case 'B':
                    return 1001; // arrow down
                case 'C':
                    return 1002; //arrow right
                case 'D':
                    return 1003; //arrow left
                default:
                    return 27; //when not arrow just return ESCape
                }
            }
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt); //shit go back
        }
        return (int)c[0];
    }
#ifdef TRUEDEBUG
    //DEBUG FUNCTION TO OUTPUT ARROWS FOR BOTH LINUX AND WINDOWS
    std::string arrowsRead(int c) {
        switch (c) {
        case 1000:
            return "arrow up";
        case 1001:
            return "arrow down";
        case 1002:
            return "arrow right";
        case 1003:
            return "arrow left";
        default:
            return "???";
        }
        return "???";
    }
    //-----------------------------------------------
#endif
}
//WINDOWS LIBRARIES

#elif defined(_WIN32)
#include <conio.h>

namespace rawInput {
    void changeTerminalState(int echo) {
        //Its empty here since windows handles it by itself
    }
    void resetTerminal(void) {
        //same here
    }

    int readKey() {
        int c = _getch(); // _getch zwraca INT, co jest lepsze. 
        if (c == 0 || c == 224) { // Klawisz specjalny wykryty 
            c = _getch(); // Odczytaj faktyczny kod klawisza 
            switch (c) { 
            case 72: 
                return 1000; // arrow up
            case 80: 
                return 1001; // arrow down
            case 77: 
                return 1002; // arrow right
            case 75: 
                return 1003; // arrow left
            default: 
                return c; // something different 
            } 
        } 
        return c; // Zwróć zwykły kod ASCII 
    }
    //DEBUG FUNCTION TO OUTPUT ARROWS FOR BOTH LINUX AND WINDOWS
    #ifdef TRUEDEBUG
    std::string arrowsRead(int c) {
        switch (c) {
        case 1000:
            return "arrow up";
        case 1001:
            return "arrow down";
        case 1002:
            return "arrow right";
        case 1003:
            return "arrow left";
        default:
            return "???";
        }
        return "???";
    }
    //-----------------------------------------------
    #endif
}
#endif

/*int main()
{
    int c;
    rawInput::changeTerminalState(0);
    while (true) {
        c = rawInput::readKey();
        if (c == 10 || c == 13) { //10 - enter on Linux | 13 - enter on windows
            break;
        }
        else if (c == 1000 || c == 1001 || c == 1002 || c == 1003) {
            std::cout << rawInput::arrowsRead(c) << "\n";
        }
        else if (c == 27) {
            std::cout << "ESC" << "\n";
        }
        else {
            std::cout << (char)c << "\n";
        }
    }
    rawInput::resetTerminal();
}*/