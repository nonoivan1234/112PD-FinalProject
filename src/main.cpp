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
using namespace std;

#define KEY_ENTER 13
#define KEY_LEFT 75
#define KEY_RIGHT 77

const int DeltaT = 50;
const double BorderLeft = 2;
const double BorderRight = 50;
const double WindowWidth = 100;
const double BorderTop = 2;
const double BorderBottom = 28;
const double PlayerSpeed = 2;
const double BulletSpeed = 0.5;
const double EnemySpeed = 0.3;
const double EnemySpawnRate = 0.03;
const double xDiffAcceptable = 2;
const double yDiffAcceptable = 1;

void gotoxy(double x, double y)
{
    COORD coord = {(short)x, (short)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

class Bullet;
class Character;
class Player;
class Enemy;
class Game;

class Character
{
protected:
    double x, y;

public:
    Character(double x, double y)
    {
        this->x = x;
        this->y = y;
    };
    double getX() { return x; };
    double getY() { return y; };
    bool OutOfBorder() { return (y >= BorderBottom - 3) || (y < BorderTop); };
    virtual void Erase();
    virtual ~Character(){};
};

void Character::Erase()
{
    gotoxy(x, y);
    cout << " ";
}

class Bullet : public Character
{
private:
    double speed;

public:
    Bullet(double x, double y) : Character(x, y)
    {
        this->x = x;
        this->y = y;
        this->speed = BulletSpeed;
    };
    void Draw();
    void Move();
    double getX() { return x; };
    double getY() { return y; };
    bool Hit(Character *character);
    bool OutOfBorder();
    void explode();
    bool TouchDeadline() { return abs(y - BorderBottom) <= yDiffAcceptable * 2; };
};

class Player : public Character
{
    friend class Bullet;
    friend class Game;

private:
    vector<Bullet *> bullets;

public:
    Player(double x, double y) : Character(x, y)
    {
        this->x = x;
        this->y = y;
    };
    void Draw();
    void Move(char key);
    void Move();
    void Shoot(vector<Bullet *> &bullets);
};

void Player::Draw()
{
    gotoxy(x, y);
    cout << "O";
}

void Player::Move()
{
    if (kbhit())
    {
        char key = getch();
        if (key == 'a' || key == 'd' || key == KEY_LEFT || key == KEY_RIGHT)
            Move(key);
    }
}

void Player::Move(char key)
{
    Erase();

    if ((key == 'a' || key == KEY_LEFT) && x > BorderLeft + 1)
        x -= PlayerSpeed;
    if ((key == 'd' || key == KEY_RIGHT) && x < BorderRight - 1)
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
    
    Sleep(100);

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
    bullets[bullets.size() - 1]->Draw();
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

class Enemy : public Character
{
    friend class Game;

private:
    double speed;
    char pic;

public:
    Enemy(double x, double y) : Character(x, y)
    {
        this->x = x;
        this->y = y;
        this->pic = 'X';
        this->speed = EnemySpeed;
    };
    void Draw();
    void Move();
};

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

class Game
{
    friend class Player;
    friend class Bullet;
    friend class Enemy;

private:
    int gameScore;
    Player* player;
    vector<Enemy *> enemies;

public:
    Game();
    void Run();
    void DrawBackground();
    void UserClick();
    void DrawDeadline();
    void UpdateInfoBar(int gameScore, std::chrono::seconds leftTime);
    void DrawWhiteSpace(int a_x, int a_y, int b_x, int b_y);
    void EnemiesSpawn();
    void EnemiesMove();
    void BulletsOutOfBorderCheck();
    void Welcome();
    void GameOver();
    ~Game(){};
};

Game::Game()
{
    player = new Player((BorderLeft + BorderRight) / 2, BorderBottom);
    gameScore = 0;
}

void Game::Run()
{
    Welcome();

    DrawBackground();
    player->Draw();

    auto startTime = std::chrono::system_clock::now();
    auto endTime = startTime + std::chrono::seconds(60);

    while (std::chrono::system_clock::now() < endTime)
    {
        if (gameScore < 0)
            break;

        EnemiesSpawn();

        EnemiesMove();

        BulletsOutOfBorderCheck();

        UserClick();

        player->Move();

        UpdateInfoBar(gameScore, std::chrono::duration_cast<std::chrono::seconds>(endTime - std::chrono::system_clock::now()));
        // DrawDeadline();

        Sleep(DeltaT);
    }

    DrawWhiteSpace(0, 0, BorderRight + 1, BorderBottom + 1);
    GameOver();
}

void Game::EnemiesSpawn()
{
    if (rand() % (int)(1.0 / EnemySpawnRate) == 0)
    {
        enemies.push_back(new Enemy(BorderLeft + rand() % (int)(BorderRight - BorderLeft - 2) + 2, BorderTop));
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
            gameScore -= 20;
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
                gameScore += 10;
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

void Game::UpdateInfoBar(int gameScore, std::chrono::seconds leftTime)
{
    gotoxy(BorderLeft, 0);
    cout << "Score: " << setw(3) << gameScore;

    gotoxy(BorderLeft + 20, 0);
    cout << "Time: " << setw(2) << leftTime.count() << "s";
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
        Sleep(2);
    }
}

void Game::Welcome()
{
    fstream file;

    file.open("welcome.txt", ios::in);
    if(!file.is_open())
    {
        throw("welcome.txt File not found");
    }

    string line;
    int i = 0;
    while (getline(file, line))
    {
        int len = line.length();

        // keep the text in the middle of the screen
        int pos = (WindowWidth - len) / 2;

        gotoxy(pos, BorderBottom / 2 - 5 + i++);
        cout << line << "\n";
    }
    file.close();

    while (1)
    {
        if (kbhit())
        {
            char key = getch();
            if (key == 's' || key == 'S')
                break;
        }
    }

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
}

void Game::GameOver()
{
    fstream file;
    file.open("gameover.txt", ios::in);
    if(!file.is_open())
    {
        throw("gameover.txt File not found");
    }

    string line;
    int i = 0;
    while (getline(file, line))
    {
        int len = line.length();

        // keep the text in the middle of the screen
        int pos = (WindowWidth - len) / 2;

        gotoxy(pos, BorderBottom / 2 - 5 + i++);
        cout << line << "\n";
    }
    file.close();

    gotoxy((WindowWidth - 18) / 2, BorderBottom / 2 - 5 + i++);
    cout << "Your score is: " << gameScore << "\n";

    gotoxy((WindowWidth - 28) / 2, BorderBottom / 2 - 5 + i++);
    cout << "Press E/e to exit the game\n";

    gotoxy((WindowWidth - 34) / 2, BorderBottom / 2 - 5 + i++);
    cout << "Or press S/s to start a new game\n";
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
                try{
                    game.Run();
                }
                catch(const char* msg)
                {
                    cerr << msg << endl;
                }
            }
        }
    }
}

int main()
{
    Game game;

    try{
        game.Run();
    }
    catch(const char* msg)
    {
        cerr << msg << endl;
    }

    return 0;
}

// Note: Online Cool Text Generator: https://www.askapache.com/online-tools/figlet-ascii/