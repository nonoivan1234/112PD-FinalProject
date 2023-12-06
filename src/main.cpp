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

const double BorderLeft = 2;
const double BorderRight = 50;
const double WindowWidth = 80;
const double BorderTop = 2;
const double BorderBottom = 28;
const double PlayerSpeed = 2;
const double BulletSpeed = 0.3;
const double EnemySpeed = 0.07;
const double EnemySpawnRate = 0.015;
const double xDiffAcceptable = 2;
const double yDiffAcceptable = 1;

void gotoxy(double x, double y)
{
    COORD coord = {x, y};
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
    bool OutOfBorder() { return y >= BorderBottom - 3; };
    virtual void Erase();
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
    bool IsHit(Character *character);
    bool OutOfBorder();
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
    void Move();
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
        Erase();

        char key = getch();
        if (key == 'a' && x > BorderLeft)
            x -= PlayerSpeed;
        if (key == 'd' && x < BorderRight)
            x += PlayerSpeed;
    }

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

bool Bullet::IsHit(Character *character)
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

public:
    Enemy(double x, double y) : Character(x, y)
    {
        this->x = x;
        this->y = y;
        this->speed = EnemySpeed;
    };
    void Draw();
    void Move();
};

void Enemy::Draw()
{
    gotoxy(x, y);
    cout << "X";
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
    Player *player;
    vector<Enemy *> enemies;

public:
    Game();
    void Run();
    void DrawBackground();
    void click();
    void DrawDeadline();
    void UpdateInfoBar(int gameScore, std::chrono::seconds leftTime);
    void DrawWhiteSpace(int a_x, int a_y, int b_x, int b_y);
    void EnemiesMove();
    void BulletsOutOfBorderCheck();
    void Welcome();
    void GameOver();
};

Game::Game()
{
    player = new Player(10, BorderBottom - 1);
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

        if (rand() % (int)(1.0 / EnemySpawnRate) == 0)
        {
            enemies.push_back(new Enemy(BorderLeft + rand() % (int)(BorderRight - BorderLeft), BorderTop));
            enemies[enemies.size() - 1]->Draw();
        }

        EnemiesMove();

        BulletsOutOfBorderCheck();

        click();

        player->Move();

        UpdateInfoBar(gameScore, std::chrono::duration_cast<std::chrono::seconds>(endTime - std::chrono::system_clock::now()));
        DrawDeadline();

        Sleep(1);
    }

    DrawWhiteSpace(0, 0, BorderRight + 1, BorderBottom + 1);
    GameOver();
}

void Game::EnemiesMove()
{
    for (int i = 0; i < enemies.size(); i++)
    {
        enemies[i]->Move();

        if (enemies[i]->OutOfBorder())
        {
            gameScore -= 20;
            enemies[i]->Erase();
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }

        for (int j = 0; j < player->bullets.size(); j++)
        {
            if (player->bullets[j]->IsHit(enemies[i]))
            {
                enemies[i]->Erase();
                delete enemies[i];
                enemies.erase(enemies.begin() + i);
                player->bullets[j]->Erase();
                delete player->bullets[j];
                player->bullets.erase(player->bullets.begin() + j);
                gameScore += 10;
            }
        }
    }
}

void Game::BulletsOutOfBorderCheck()
{
    for (auto bullet : player->bullets)
    {
        bullet->Move();
        if (bullet->OutOfBorder())
        {
            bullet->Erase();
            delete bullet;
            player->bullets.erase(player->bullets.begin());
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

void Game::click()
{
    if (kbhit())
    {
        char key = getch();
        if (key == ' ')
        {
            player->bullets.push_back(new Bullet(player->getX(), player->getY() - 1));
            player->bullets[player->bullets.size() - 1]->Draw();
        }
        else
        {
            player->Erase();
            if (key == 'a' && player->getX() > BorderLeft)
                player->x -= PlayerSpeed;
            if (key == 'd' && player->getX() < BorderRight)
                player->x += PlayerSpeed;
            player->Draw();
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
        Sleep(3);
    }
}

void Game::Welcome()
{
    fstream file;
    file.open("welcome.txt", ios::in);
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
    getch();

    DrawWhiteSpace(0, 0, WindowWidth + 1, BorderBottom + 1);
}

void Game::GameOver()
{
    gotoxy(0, 0);
    cout << "Game Over!\n";
    cout << "Your score is: " << gameScore << "\n";

    while (1)
    {
        getch();
    }
}

int main()
{
    Game game;
    game.Run();

    return 0;
}

// Note: Online Cool Text Generator: https://www.askapache.com/online-tools/figlet-ascii/