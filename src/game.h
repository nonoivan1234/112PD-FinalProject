#define KEY_ENTER 13
#define KEY_LEFT 75
#define KEY_RIGHT 77

const int DeltaT = 50;
const int GameTime = 30;
const double BorderLeft = 10;
const double BorderRight = 50;
const double WindowWidth = 90;
const double BorderTop = 2;
const double BorderBottom = 28;
const double PlayerSpeed = 3;
const double BulletSpeed = 0.6;
const double EnemySpeed = 0.5;
const double EnemySpawnRate = 0.04;
const int ExplodeTime = 100;
const double xDiffAcceptable = 2;
const double yDiffAcceptable = 1;

void gotoxy(double x, double y);

class Character
{
protected:
    double x, y;

public:
    Character(double x, double y);
    double getX();
    double getY();
    virtual bool OutOfBorder();
    virtual void Erase();
    virtual ~Character(){};
};

class Bullet : public Character
{
private:
    double speed;

public:
    Bullet(double x, double y);
    void Draw();
    void Move();
    bool Hit(Character *character);
    bool OutOfBorder();
    void explode();
    bool TouchDeadline();
};

class Player : public Character
{
    friend class Bullet;
    friend class Game;

private:
    std::vector<Bullet *> bullets;

public:
    Player(double x, double y);
    void Draw();
    void Move(char key);
    void Move();
    void Shoot(std::vector<Bullet *> &bullets);
};

class Enemy : public Character
{
    friend class Game;

private:
    double speed;
    char pic;

public:
    Enemy(double x, double y);
    void Draw();
    void Move();
};

class Game
{
    friend class Player;
    friend class Bullet;
    friend class Enemy;

private:
    int gameScore;
    Player *player;
    std::vector<Enemy *> enemies;

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