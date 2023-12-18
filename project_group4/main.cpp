#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <string>
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