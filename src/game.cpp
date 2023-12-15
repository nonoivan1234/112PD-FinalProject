#include <iostream>
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include <vector>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>
#include "game.h"
using namespace std;

double HistoryMaxScore = 0;

void gotoxy(double x, double y) // to move the cursor to the certain position in the terminal
{
    COORD coord = {(short)x, (short)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

Character::Character(double x, double y)    // Character constructor
{
    this->x = x;
    this->y = y;
}

double Character::getX()
{
    return x;
}

double Character::getY()
{
    return y;
}

bool Character::OutOfBorder()   // check if the character is out of the border
{
    if (y >= BorderBottom - 3 || y < BorderTop)
        return true;
    return false;
}

void Character::Erase() // erase the character
{
    gotoxy(x, y);
    cout << "  ";
}

Bullet::Bullet(double x, double y) : Character(x, y)
{
    this->speed = BulletSpeed;
}

bool Bullet::TouchDeadline()
{
    return abs(y - BorderBottom) <= yDiffAcceptable * 2;
}

Player::Player(double x, double y) : Character(x, y){}

void Player::Draw()
{
    gotoxy(x, y);
    cout << "傑";
}

void Player::Move()
{
    if (kbhit())
    {
        char key = getch();
        Move(key);
    }
}

void Player::Move(char key)
{
    Erase();

    if ((key == 'a' || key == KEY_LEFT) && x - PlayerSpeed > BorderLeft + 2)
        x -= PlayerSpeed;
    if ((key == 'd' || key == KEY_RIGHT) && x + PlayerSpeed < BorderRight - 2)
        x += PlayerSpeed;

    Draw();
}

void Bullet::Draw()
{
    gotoxy(x, y);
    cout << "*";
}

void Bullet::Move()
{
    Erase();
    y -= speed;
    Draw();
}

void Bullet::explode()
{
    gotoxy(x, y);
    cout << "+";
    gotoxy(x - 1, y);
    cout << "-";
    gotoxy(x + 1, y);
    cout << "-";
    gotoxy(x, y - 1);
    cout << "|";
    gotoxy(x, y + 1);
    cout << "|";

    Sleep(ExplodeTime);

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            gotoxy(x + i, y + j);
            cout << " ";
        }
    }
}

void Player::Shoot(vector<Bullet *> &bullets)
{
    bullets.push_back(new Bullet(x, y - 1));
    bullets.back()->Draw();
}

bool Bullet::Hit(Character *character)
{
    if (abs(x - character->getX()) <= xDiffAcceptable && abs(y - character->getY()) <= yDiffAcceptable)
        return true;
    return false;
}

bool Bullet::OutOfBorder()
{
    if (y < BorderTop)
        return true;
    return false;
}

Enemy::Enemy(double x, double y) : Character(x, y)
{
    this->x = x;
    this->y = y;
    speed = NormalEnemySpeed;
    pic = "作";
}

Enemy::Enemy(double x, double y, double speed) : Character(x, y)
{
    this->x = x;
    this->y = y;
    this->speed = speed;
    pic = "作";
}

void Enemy::Draw()
{
    gotoxy(x, y);
    cout << this->pic;
}

void Enemy::Move()
{
    Erase();
    y += speed;
    Draw();
}

Game::Game()
{
    player = new Player((BorderLeft + BorderRight) / 2, BorderBottom);
    gameScore = InitScore;
    status = 0;
    EnemySpawnRate = NormalEnemySpawnRate;
    EnemySpeed = NormalEnemySpeed;
}

double Game::GetEnemySpawnRate()
{
    if (status == 0)
        return NormalEnemySpawnRate;
    else if (status == 1)
        return MidtermEnemySpawnRate;
    else if (status == 2)
        return FinalEnemySpawnRate;
    else
    {
        throw("status error");
        return 0;
    }
}

double Game::GetEnemySpeed()
{
    if (status == 0)
        return NormalEnemySpeed;
    else if (status == 1)
        return MidtermEnemySpeed;
    else if (status == 2)
        return FinalEnemySpeed;
    else
    {
        throw("status error");
        return 0;
    }
}

void Game::ChangeStatus(int status)
{
    this->status = status;
    ChangeEnemySpawnRate();
    ChangeEnemySpeed();
}

void Game::ChangeEnemySpeed()
{
    EnemySpeed = GetEnemySpeed();

    for (int i = 0; i < (int)enemies.size(); i++)
    {
        enemies[i]->speed = EnemySpeed;
    }
}

void Game::ChangeEnemySpawnRate()
{
    EnemySpawnRate = GetEnemySpawnRate();
}


void Game::GameStart(std::chrono::system_clock::time_point &end)
{
    while (std::chrono::system_clock::now() < end)
    {
        if (gameScore <= EndScore)
            break;

        EnemiesSpawn();

        EnemiesMove();

        BulletsOutOfBorderCheck();

        UserClick();

        player->Move();

        auto nowTime = std::chrono::system_clock::now();
        int leftTime = std::chrono::duration_cast<std::chrono::seconds>(end - nowTime).count();

        if(status == 0) leftTime += MidtermTime + FinalTime;
        else if(status == 1) leftTime += FinalTime;

        UpdateInfoBar(gameScore, leftTime);

        Sleep(SleepPerLoop);
    }
}
void Game::Run()
{
    system("chcp 65001");
    system("cls");

    Welcome();

    DrawBackground();
    player->Draw();

    auto startTime = std::chrono::system_clock::now();
    auto endTime = startTime + std::chrono::seconds(NormalTime);

    ChangeStatus(0);

    GameStart(endTime);

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
    
    NewWindow("GamePage/mid.txt");
    Sleep(1500);

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);

    DrawBackground();

    startTime = std::chrono::system_clock::now();
    endTime = startTime + std::chrono::seconds(MidtermTime);
    
    ChangeStatus(1);

    GameStart(endTime);

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);

    NewWindow("GamePage/final.txt");
    Sleep(1500);

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);

    DrawBackground();

    startTime = std::chrono::system_clock::now();
    endTime = startTime + std::chrono::seconds(FinalTime);
    
    ChangeStatus(2);

    GameStart(endTime);

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
    
    GameOver();
}

void Game::EnemiesSpawn()
{
    if (rand() % (int)(1.0 / EnemySpawnRate) == 0)
    {
        enemies.push_back(new Enemy(BorderLeft + rand() % (int)(BorderRight - BorderLeft - 6) + 3, BorderTop, EnemySpeed));
        enemies[enemies.size() - 1]->Draw();
    }
}

void Game::EnemiesMove()
{
    for (int i = 0; i < (int)enemies.size(); i++)
    {
        enemies[i]->Move();

        if (enemies[i]->OutOfBorder())
        {
            gameScore -= MinusPerMiss;
            enemies[i]->Erase();
            enemies.erase(enemies.begin() + i);
        }

        for (int j = 0; j < (int)(player->bullets.size()); j++)
        {
            if (player->bullets[j]->Hit(enemies[i]))
            {
                enemies[i]->Erase();
                enemies.erase(enemies.begin() + i);
                player->bullets[j]->explode();
                player->bullets[j]->Erase();
                player->bullets.erase(player->bullets.begin() + j);
            }
        }
    }
}

void Game::BulletsOutOfBorderCheck()
{
    for (int i = 0; i < (int)player->bullets.size(); i++)
    {
        player->bullets[i]->Move();
        if (player->bullets[i]->TouchDeadline())
        {
            DrawDeadline();
        }
        if (player->bullets[i]->OutOfBorder())
        {
            player->bullets[i]->Erase();
            player->bullets.erase(player->bullets.begin() + i);
        }
    }
}

void Game::DrawDeadline()
{
    for (int i = BorderLeft - 1; i < (BorderLeft + BorderRight) / 2 - 3; i++)
    {
        gotoxy(i, BorderBottom - 3);
        cout << "-";
    }

    gotoxy((BorderLeft + BorderRight) / 2 - 3, BorderBottom - 3);
    cout << "DEADLINE";

    for (int i = (BorderLeft + BorderRight) / 2 + 5; i <= BorderRight + 1; i++)
    {
        gotoxy(i, BorderBottom - 3);
        cout << "-";
    }
}

void Game::DrawBackground()
{
    DrawDeadline();

    for (int i = BorderTop - 1; i <= BorderBottom + 1; i++)
    {
        gotoxy(BorderLeft - 1, i);
        cout << "|";
        gotoxy(BorderRight + 1, i);
        cout << "|";
    }
}

void Game::UpdateInfoBar(double gameScore, int leftTime)
{
    gotoxy(BorderLeft, 0);
    cout << "GPA: " << setw(3) << gameScore;

    gotoxy(BorderLeft + 20, 0);
    cout << "Time: " << setw(2) << leftTime << "s";
}

void Game::UserClick()
{
    if (kbhit())
    {
        char key = getch();
        if (key == ' ' || key == KEY_ENTER)
        {
            player->Shoot(player->bullets);
        }
        else
        {
            player->Move(key);
        }
    }
}

void Game::DrawWhiteSpace(int a_x, int a_y, int b_x, int b_y) // to clean a certain space in the terminal
{
    for (int i = a_x; i <= b_x; i++)
    {
        for (int j = a_y; j <= b_y; j++)
        {
            gotoxy(i, j);
            cout << " ";
        }
        Sleep(0.5);
    }
}

int Game::NewWindow(string File)
{
    fstream file;

    file.open(File, ios::in);
    if (!file.is_open())
        throw(File + " File not found");

    string line;
    int i = 0;
    while (getline(file, line))
    {
        int len = line.size() / sizeof(char);

        // keep the text in the middle of the screen
        int pos = (WindowWidth - len) / 2;

        if (WindowWidth - len < 0)
            throw("Window Width is too small");

        gotoxy(pos, BorderBottom / 2 - 5 + i++);
        cout << line << "\n";
    }
    file.close();

    return i;
}

void Game::ReadNextPage()
{
    while (1)
    {
        if (kbhit())
        {
            char key = getch();
            if (key == 'r' || key == 'R')
                break;
        }
    }
}

void Game::Welcome()
{
    NewWindow("welcome.txt");

    while (1)
    {
        if (kbhit())
        {
            char key = getch();
            if (key == 's' || key == 'S')
                break;
            if (key == 'r' || key == 'R')
            {
                DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
                NewWindow("Guide/Page1.txt");

                ReadNextPage();

                DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
                NewWindow("Guide/Page2.txt");

                ReadNextPage();
                break;
            }
        }
    }

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
}

void Game::GameOver()
{
    int i = NewWindow("gameover.txt");

    gotoxy((WindowWidth - 18) / 2, BorderBottom / 2 - 5 + i++);
    cout << "Your GPA is: " << gameScore << "\n";

    if (gameScore > HistoryMaxScore)
    {
        HistoryMaxScore = gameScore;
        gotoxy((WindowWidth - 15) / 2, BorderBottom / 2 - 5 + i++);
        cout << "New Record!!!\n";
    }
    else
    {
        gotoxy((WindowWidth - 23) / 2, BorderBottom / 2 - 5 + i++);
        cout << "Your best GPA is: " << HistoryMaxScore << "\n";
    }

    gotoxy((WindowWidth - 28) / 2, BorderBottom / 2 - 5 + i++);
    cout << "Press E/e to exit the game\n";

    gotoxy((WindowWidth - 34) / 2, BorderBottom / 2 - 5 + i++);
    cout << "Or press S/s to start a new game\n";

    if (WindowWidth - 34 < 0)
        throw("Window Width is too small");

    while (1)
    {
        if (kbhit())
        {
            char key = getch();
            if (key == 'e' || key == 'E')
                break;
            else if (key == 's' || key == 'S')
            {
                DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
                Game game;
                try
                {
                    game.Run();
                }
                catch (const char *msg)
                {
                    cerr << msg << endl;
                }
            }
        }
    }
}