#define KEY_ENTER 13
#define KEY_LEFT 75
#define KEY_RIGHT 77

const int NormalTime = 20;
const int MidtermTime = 20;
const int FinalTime = 20;
const double InitScore = 4.3;
const double MinusPerMiss = 0.5;
const double EndScore = 1;
const int SleepPerLoop = 50;
const int GameTime = 30;
const double BorderLeft = 10;
const double BorderRight = 50;
const double WindowWidth = 120;
const double BorderTop = 2;
const double BorderBottom = 28;
const double PlayerSpeed = 4;
const double BulletSpeed = 0.8;
const double NormalEnemySpeed = 0.05;
const double NormalEnemySpawnRate = 0.05;
const double MidtermEnemySpeed = 0.3;
const double MidtermEnemySpawnRate = 0.05;
const double FinalEnemySpeed = 0.3;
const double FinalEnemySpawnRate = 0.25;
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
    Enemy(double x, double y, double speed);
    void Draw();
    void Move();
};

class Game
{
    friend class Player;
    friend class Bullet;
    friend class Enemy;

private:
    double gameScore;
    int status; // 0: normal, 1: midterm, 2: final
    double EnemySpeed;
    double EnemySpawnRate;
    Player *player;
    std::vector<Enemy *> enemies;

public:
    Game();
    void Run();
    void DrawBackground();
    void UserClick();
    void DrawDeadline();
    void UpdateInfoBar(double gameScore, int leftTime);
    void DrawWhiteSpace(int a_x, int a_y, int b_x, int b_y);
    void EnemiesSpawn();
    void EnemiesMove();
    void ChangeStatus(int status);
    double GetEnemySpeed();
    double GetEnemySpawnRate();
    void ChangeEnemySpeed();
    void ChangeEnemySpawnRate();
    void BulletsOutOfBorderCheck();
    void ReadNextPage();
    int NewWindow(std::string file);
    void GameStart(std::chrono::system_clock::time_point &end);
    void Welcome();
    void GameOver();
    ~Game(){};
};