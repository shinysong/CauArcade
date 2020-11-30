#include <iostream>
#include <bangtal>
#include <ctime>
#include <Windows.h>

#define EMPTY 0 // 빈 칸
#define BOMB 1 //물풍선
#define SPLASH 2 //물줄기
#define BLOCK 3 // 블록

#define WIDTH 12
#define HEIGHT 12

#define BOMB_TIME 2.5f
#define EXPLODE 3.0f
#define CLEAR 2.0f

#define VERTICAL 0
#define HORIZONTAL 1

#define UP 84
#define DOWN 85
#define LEFT 82
#define RIGHT 83

using namespace bangtal;
using namespace std;

class Board;
typedef std::shared_ptr<Board> BoardPtr;

class Board : public Object {
protected:
    Board(
        const string& _img,
        ScenePtr _scene,
        int _x, int _y, bool _shown,
        int _status
    ) : Object(_img, _scene, _x, _y, _shown), status(_status) {};
    int status;
    int size = 0; // 물줄기의 길이
public:
    static BoardPtr create(
        const string& _img,
        ScenePtr _scene,
        int _x, int _y, bool _shown,
        int _status
    ) {
        auto object = BoardPtr(new Board(_img, _scene, _x, _y, _shown, _status));
        Object::add(object);

        return object;
    };

    const int getStatus() { return status; };
    void setStatus(int _status) { status = _status; };
    const int getSize() { return size; };
    void setSize(int _size) { size = _size; };

};

extern void lobby_main();

ScenePtr oneScene;
ObjectPtr character;
BoardPtr board[WIDTH][HEIGHT];
TimerPtr boardTimer[WIDTH][HEIGHT], creater, gameTimer, charTimer;
SoundPtr background;

bool live = true;

int characterX, characterY;

void setStatus(int x, int y, int status, int way = 0) {
    auto bomb = board[x][y];
    bomb->setStatus(status);
    switch (status) {
        case EMPTY: bomb->setImage("Images/blank.png"); break;
        case BOMB: bomb->setImage("Images/물풍선.png"); break;
        case SPLASH: 
            if (way == HORIZONTAL) bomb->setImage("Images/물줄기가로.png");
            else bomb->setImage("Images/물줄기세로.png"); 
            break;
        case BLOCK: bomb->setImage("Images/상자.png"); break;
        default: break;
    }
}

void setEnd(int x, int y, int* d) {
    switch (d[0]) {
    case 0:
        if (d[1] == 1) board[x][y]->setImage("Images/물줄기우.png");
        else board[x][y]->setImage("Images/물줄기좌.png");
        break;
    case 1:
        board[x][y]->setImage("Images/물줄기위.png");
        break;
    case -1:
        board[x][y]->setImage("Images/물줄기아래.png");
        break;
    default:  board[x][y]->setImage("Images/물줄기세로.png"); break;
    }
    return;
}

bool check(int x, int y) {

    int i = (y - 58) / 51;
    int j = (x - 37) / 79;

    int checkerOne[4][2] = {
        { 7, 7},
        {57, 7},
        { 7,73},
        {57,73}
    };

    int checkerTwo[4][2] = {
        { 7, 7},
        {57, 7},
        { 7,73},
        {57,73}
    };


    for (auto d : checkerOne) {
        i = (y + d[1] - 58) / 51;
        j = (x + d[0] - 37) / 79;

        if ((y + d[1] - 58) < 0 || (x + d[0] - 37) < 0) return false;

        if (i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT) return false;

        if (board[i][j]->getStatus() == SPLASH) {
            auto loseSound = Sound::create("music/Lose.mp3");
            character->setImage("Images/Trap.png");
            background->stop();
            loseSound->play();
            gameTimer->stop();
            for (int i = 0;i < WIDTH;i++) {
                for (int j = 0;j < HEIGHT;j++) boardTimer[i][j]->stop();
            }
            creater->stop();
            hideTimer();
            live = false;
            return false;
        }
    }

    for (auto d : checkerTwo) {
        i = (y + d[1] - 58) / 51;
        j = (x + d[0] - 37) / 79;

        if (board[i][j]->getStatus() == BLOCK || board[i][j]->getStatus() == BOMB) {
            return false;
        }
    }

    

    return true;
}

void one_main() {
    live = true;

    srand((unsigned int)time(0));
    oneScene = Scene::create("", "Images/1탄.png");
    background = Sound::create("music/1.mp3");


    background->play();
    
    //캐릭터 위치
    characterX = 510;
    characterY = 510;
    character = Object::create("Images/배찌앞.png", oneScene, characterX, characterY);

    //물폭탄 생성용 타이머 //몇 초마다 물풍선 나타날지
    creater = Timer::create(BOMB_TIME);
    gameTimer = Timer::create(90.0f);
    charTimer = Timer::create(0.1f);

    for (int i = 0;i < WIDTH;i++) {
        for (int j = 0; j < HEIGHT;j++) {
            //보드 초기화. 타이머 초기화 및 타이머 콜백함수 설정
            board[i][j] = Board::create("Images/blank.png", oneScene, 37 + 79*j, 58 + 51*i, true, EMPTY);
            boardTimer[i][j] = Timer::create(EXPLODE);
            boardTimer[i][j]->setOnTimerCallback([&, i, j](TimerPtr)-> bool {

                if (!live) return true;
                int charX = characterX;
                int charY = characterY;
                int size = board[i][j]->getSize();
                int status = board[i][j]->getStatus();

                int diff[4][2] = {
                    {1, 0},
                    {0 ,1},
                    {-1, 0},
                    {0, -1}
                };

                if (status == BOMB) {
                    auto sound = Sound::create("music/wave.mp3");
                    for (auto d : diff) {
                        int x = i;
                        int y = j;
                        for (int k = 0;k < size;k++) {
                            if (x >= WIDTH || x < 0 || y >= WIDTH || y < 0) {
                                x -= d[0];
                                y -= d[1];
                                if (x == i && y == j) setStatus(x, y, SPLASH);
                                else setEnd(x, y, d);
                                break;
                            }
                            if (board[x][y]->getStatus() == BLOCK || board[x][y]->getStatus() == BOMB) {
                                if (x != i || y != j) {
                                    x -= d[0];
                                    y -= d[1];
                                    if (x == i && y == j) setStatus(x, y, SPLASH);
                                    else setEnd(x, y, d);
                                    break;
                                }
                            }

                            if (d[0] == 0) setStatus(x, y, SPLASH, HORIZONTAL);
                            else setStatus(x, y, SPLASH, VERTICAL);

                            if (k == size - 1) {
                                setEnd(x, y, d);
                            }
                            
                            x += d[0];
                            y += d[1];
                        }

                    }

                    boardTimer[i][j]->set(CLEAR);
                    boardTimer[i][j]->start();
                    sound->play();
                }
                else {
                    for (auto d : diff) {
                        int x = i;
                        int y = j;
                        for (int k = 0;k < size;k++) {
                            if (x >= WIDTH || x < 0 || y >= WIDTH || y < 0) break;
                            if (board[x][y]->getStatus() == BLOCK || board[x][y]->getStatus() == BOMB) break;

                            setStatus(x, y, EMPTY);
                            x += d[0];
                            y += d[1];
                        }
                    }

                }

                check(charX, charY);

                

                return true;
            });
        }
    }

    setStatus(0, 0, BLOCK);
    setStatus(0, 1, BLOCK);
    setStatus(1, 0, BLOCK);
    setStatus(1, 1, BLOCK);

    setStatus(10, 10, BLOCK);
    setStatus(10, 11, BLOCK);
    setStatus(11, 10, BLOCK);
    setStatus(11, 11, BLOCK);

    setStatus(10, 1, BLOCK);
    setStatus(11, 1, BLOCK);
    setStatus(10, 0, BLOCK);
    setStatus(11, 0, BLOCK);

    setStatus(0, 10, BLOCK);
    setStatus(0, 11, BLOCK);
    setStatus(1, 10, BLOCK);
    setStatus(1, 11, BLOCK);

    setStatus(6, 6, BLOCK);
    setStatus(6, 7, BLOCK);
    setStatus(7, 6, BLOCK);
    setStatus(7, 7, BLOCK);
    board[6][6]->setImage("Images/블록.png");
    board[6][7]->setImage("Images/블록.png");
    board[7][6]->setImage("Images/블록.png");
    board[7][7]->setImage("Images/블록.png");

    

    oneScene->setOnKeyboardCallback([&](ScenePtr, int key, bool) -> bool {
        //캐릭터 움직임 및 물줄기 확인

        if (!live) return true;

        int prevX = characterX;
        int prevY = characterY;
        string image;

        switch (key) {
            case UP: characterY += 10; image="Images/배찌뒤.png"; break;
            case DOWN: characterY -= 10; image = "Images/배찌앞.png"; break;
            case LEFT: characterX -= 10; image = "Images/배찌왼.png"; break;
            case RIGHT: characterX += 10; image = "Images/배찌오.png"; break;
            default: return true;
        }

        if (check(characterX, characterY)) {
            character->locate(oneScene, characterX, characterY);
            character->setImage(image);
        }
        else {
            characterX = prevX;
            characterY = prevY;
        }
        return true;
    });

    creater->setOnTimerCallback([&](TimerPtr) -> bool {

        //랜덤으로 폭탄 생길 위치 지정
        if (!live) return true;

        int bombX = rand() % WIDTH;
        int bombY = rand() % HEIGHT;

        while (board[bombX][bombY]->getStatus() != EMPTY) {
            bombX = rand() % WIDTH;
            bombY = rand() % HEIGHT;
        }

        int size = (rand() % 9) + 3;

        board[bombX][bombY]->setStatus(BOMB);
        board[bombX][bombY]->setSize(size);
        board[bombX][bombY]->setImage("Images/물풍선.png");

        boardTimer[bombX][bombY]->set(EXPLODE);
        //일정 시간동안만 물풍선 및 물줄기 존재
        boardTimer[bombX][bombY]->start();

        creater->set(BOMB_TIME);
        creater->start();

        return true;
     });

    gameTimer->setOnTimerCallback([&](TimerPtr)-> bool {
        if (!live) return true;

        auto winSound = Sound::create("music/Win.mp3");

        showMessage("WIN!\n다음 스테이지로 이동!");

        winSound->play();
        background->stop();
        gameTimer->stop();
        for (int i = 0;i < WIDTH;i++) {
            for (int j = 0;j < HEIGHT;j++) boardTimer[i][j]->stop();
        }
        creater->stop();
        charTimer->stop();
        hideTimer();
        Sleep(1000);
        lobby_main();

        return true;
    });

    charTimer->setOnTimerCallback([&](TimerPtr)->bool {
        if (live) {
            charTimer->set(0.5f);
            charTimer->start();
            return true;
        }
        Sleep(3000);
        lobby_main();
        return true;
        
    });
        
    creater->start();
    gameTimer->start();
    charTimer->start();
    showTimer(gameTimer);
    enterScene(oneScene->ID());

}
