#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include "game.h"
using namespace std;

int main()
{
    Game game;

    try
    {
        game.Run();
    }
    catch (const char *msg)
    {
        cerr << msg << endl;
    }

    system("pause");
    return 0;
}

// Note: Online Cool Text Generator: https://www.askapache.com/online-tools/figlet-ascii/