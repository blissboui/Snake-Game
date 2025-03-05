#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>

// 💀🍎🍏🍗▒🧱▄ ☠💣☠■
// map size
#define MAP_MIN_WIDTH 5
#define MAP_WIDTH 40
#define MAP_MIN_HEIGHT 2
#define MAP_HEIGHT 20
#define MAP_XPOS_CENTER (MAP_WIDTH - MAP_MIN_WIDTH)/2 + MAP_MIN_WIDTH
#define MAP_YPOS_CENTER (MAP_HEIGHT - MAP_MIN_HEIGHT)/2 + MAP_MIN_HEIGHT

#define ESC_KEY 27
#define ENTER_KEY 13
#define UP_KEY 72
#define LEFT_KEY 75
#define DOWN_KEY 80
#define RIGHT_KEY 77

// 먹이 생산 속도(s)
#define FOOD_TIME 2

// 장애물 생산 / 삭제 속도(s)
#define OBSTACLE_TIME 7
#define OBSTACLE_DELECT_TIME 10

// 뱀 최대속도 / 장애물 생성 최소시간
#define MAX_SPEED 30
#define MAX_OBSTACLE_SPEED 3

#define NORMAL_FOOD 0
#define SPECIAL_FOOD 1

int xPos_snake[50];
int yPos_snake[50];
int snakeLen;

int foodXpos[100];
int foodYpos[100];
int foodType[100];
int foodLen;
bool foodFlag;
time_t foodTime;

int obstacleXpos[10];
int obstacleYpos[10];
int obstacleLen;
time_t obstacleTime;
time_t obstacleDelectTime;
int obstacleSpawnSpeed;

int score;
int speed;

int currntKey;
int lastKey;

void ShowMap(void); // 맵 그리기
void hideCursor(void);  // 커서 지우기

void Goto_XY(int x, int y, char* str);// xy좌표에 출력
void GetKey(void); // 키 입력
void ShowScore(void); // 점수 출력

void InitSnake(void);   // 뱀 초기값
void MoveSnake(void);   // 뱀 move
void ShowSnake(void);    // 뱀 출력

void RandomPos(int* xPtr, int* yPtr);   // 랜덤 좌표 생성
void GenerateFood(void);    // 먹이 생성
void CheckFoodCollision(void);  // 먹이와 충돌 검사

void GenerateObstacle(void);    // 장애물 생성
void DelectObstacle(void);      // 장애물 삭제

void CheckObstacleCollision(void);   // 장애물과 충돌 검사
void CheckWallCollision(void);   // 벽과 충돌 검사
void CheckSnakeCollision(void);  // 몸통과 출동 검사

// void ExitGame(void);    // 게임종료
// void PauseGame(void);   // 일시정지
void GameOver(void);    // 게임오버


int main(void)
{   
    hideCursor();   // 커서 지우기
    ShowMap();  // Map 출력
    InitSnake();    // 게임 초기화
    srand(time(NULL));  // 난수 생성

    

    while(1)
    {
        ShowScore();    // 점수 표시
        if(kbhit()) GetKey();   // 키 입력 반환    
        MoveSnake();    // 키 입력에 따른 방향으로 뱀머리 이동
        
        GenerateFood(); // 먹이 생성
        CheckFoodCollision();   // 먹이와 충돌 검사

        GenerateObstacle(); // 장애물 생성
        DelectObstacle();   // 일정시간 경과 후 장애물 삭제
        
        
        CheckObstacleCollision();   // 장애물과 충돌 검사
        CheckSnakeCollision();      // 몸통과 출동 검사
        CheckWallCollision();       // 벽과 충돌 검사
        
        // if(currntKey == ESC_KEY)    ExitGame();
        // if(currntKey == ENTER_KEY)
        
        ShowSnake();    // 뱀 출력
        Sleep(speed);   // 뱀 속도 조절
    }
}
// void ExitGame(void)
// {
//     int flag = 0;
//     Goto_XY(MAP_XPOS_CENTER - 3, MAP_YPOS_CENTER - 3, "<  Exit Game ? >");
//     Goto_XY(MAP_XPOS_CENTER - 1, MAP_YPOS_CENTER - 1, "> Yes");
//     Goto_XY(MAP_XPOS_CENTER - 1, MAP_YPOS_CENTER - 2, "  NO");

//     while(1)
//     {
//         char key = getch();
//         if(key == 0 || key == 224)
//         {
//             if(key == UP_KEY)
//                 flag = 0;
//             else if(key == DOWN_KEY)
//                 flag = 1;
//         }
//         if(flag == 0)
//         {
//             Goto_XY(MAP_XPOS_CENTER - 1, MAP_YPOS_CENTER - 1, "> Yes");
//             Goto_XY(MAP_XPOS_CENTER - 1, MAP_YPOS_CENTER - 2, "  NO");
//         }
//         else
//         {
//             Goto_XY(MAP_XPOS_CENTER - 1, MAP_YPOS_CENTER - 1, " Yes");
//             Goto_XY(MAP_XPOS_CENTER - 1, MAP_YPOS_CENTER - 2, ">  NO");
//         }
//     }
// }
void GameOver(void)
{
    Goto_XY(MAP_XPOS_CENTER - 3, MAP_YPOS_CENTER - 3, "<  Game Over  >");
    Goto_XY(MAP_XPOS_CENTER - 1, MAP_YPOS_CENTER - 1, "Score: ");
    printf("%d", score);

    getch(); 

    system("cls");
    
    ShowMap();
    InitSnake();

}
void ShowScore(void)
{
    Goto_XY(MAP_WIDTH-4, MAP_HEIGHT+1, "Score: ");
    printf("%d", score);
}

void CheckFoodCollision(void)       
{
    int flag = 0;
    for(int i = 0 ; i < foodLen ; i++)
    {
        if(xPos_snake[0] == foodXpos[i] && yPos_snake[0] == foodYpos[i] && flag == 0)
        {
            if(foodType[i] == SPECIAL_FOOD)    // 보너스 먹이
            {   
                if(snakeLen > 3)
                {
                    snakeLen--;
                    foodFlag = true;
                }
                score += 10;
                speed -= 5;
                flag = 1;
            }

            else
            {
                snakeLen++;
                score++;
                speed -= 5;
                flag = 1; 
            }
            
            speed = speed < MAX_SPEED ? MAX_SPEED : speed;  // 뱀 최대 속도 제한
        }

        if(flag == 1)
        {
            foodXpos[i] = foodXpos[i+1];
            foodYpos[i] = foodYpos[i+1];
            foodType[i] = foodType[i+1];
        }
    }

    if(flag == 1)
        foodLen--;
}

void hideCursor(void)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void RandomPos(int* xPtr, int* yPtr)
{
    int flag = 0;
    while(1)
    {
    // 랜덤 좌표
    *xPtr = rand() % ((MAP_WIDTH - 1) - (MAP_MIN_WIDTH + 1) + 1) + (MAP_MIN_WIDTH + 1);
    *yPtr = rand() % ((MAP_HEIGHT - 1) - (MAP_MIN_HEIGHT + 1) + 1) + (MAP_MIN_HEIGHT + 1);

    for(int i = 0 ; i < snakeLen ; i++) // 생성이 몸통과 겹치지 않게
    {
        if(*xPtr == xPos_snake[i] && *yPtr == yPos_snake[i]) 
            flag = 1;
    }
    
    if(flag == 0)   
        break;
    }
}

void GenerateFood(void)
{
    if(time(NULL) >= foodTime)
    {
        RandomPos(&foodXpos[foodLen], &foodYpos[foodLen]);

        if(rand() % 10 == 5)    // 10% 확률로 보너스 먹이 생성
        {
            Goto_XY(foodXpos[foodLen], foodYpos[foodLen], "🍗");
            foodType[foodLen] = SPECIAL_FOOD;
        }

        else
        {
            Goto_XY(foodXpos[foodLen], foodYpos[foodLen], "🍎");
            foodType[foodLen] = NORMAL_FOOD;
        }
        foodLen++;
        foodTime = time(NULL) + FOOD_TIME;
    }
}

void GenerateObstacle(void)
{
    if(time(NULL) >= obstacleTime)
    {
        RandomPos(&obstacleXpos[obstacleLen], &obstacleYpos[obstacleLen]);
        Goto_XY(obstacleXpos[obstacleLen], obstacleYpos[obstacleLen], "💀");
        
        obstacleLen++;
        obstacleTime = time(NULL) + (OBSTACLE_TIME - obstacleSpawnSpeed);
    }
    
    // 뱀 속도 빠를수록 장애물 생성 속도 증가
    obstacleSpawnSpeed = speed < 150 ? 2 : 0;
    obstacleSpawnSpeed = speed < 50 ? 4 : 2;
}

void DelectObstacle(void)
{
    if(time(NULL) >= obstacleDelectTime)
    {
        Goto_XY(obstacleXpos[0], obstacleYpos[0], " ");
        
        for(int i = 0 ; i < obstacleLen ; i++)
        {
            obstacleXpos[i] = obstacleXpos[i+1];
            obstacleYpos[i] = obstacleYpos[i+1];
        }
        obstacleLen--;
        obstacleDelectTime = time(NULL) + OBSTACLE_TIME;   // 다음 장애물 삭제 시간
    }
}

void InitSnake(void)
{
    xPos_snake[0] = MAP_XPOS_CENTER;
    yPos_snake[0] = MAP_YPOS_CENTER;
    currntKey = RIGHT_KEY;
    snakeLen = 3;
    foodLen = 0;
    obstacleLen = 0;
    score = 0;
    speed = 250;
    obstacleSpawnSpeed = 0;

    foodTime = time(NULL) + FOOD_TIME;
    obstacleTime = time(NULL) + OBSTACLE_TIME;
    obstacleDelectTime = obstacleTime + OBSTACLE_DELECT_TIME;


    for(int i = 1 ; i < snakeLen ; i++)
    {
        xPos_snake[i] = xPos_snake[i-1] - 1;
        yPos_snake[i] = yPos_snake[0];
    }
}

void CheckObstacleCollision(void)
{
    for(int i = 0 ; i < obstacleLen ; i++)
    {
        if(xPos_snake[0] == obstacleXpos[i] && yPos_snake[0] == obstacleYpos[i])
            GameOver();
    }
}

void CheckSnakeCollision(void)
{
    for(int i = 1 ; i < snakeLen ; i++)
    {
        if(xPos_snake[0] == xPos_snake[i] && yPos_snake[0] == yPos_snake[i])
            GameOver();
    }
}

void CheckWallCollision(void)
{
    if(xPos_snake[0] < MAP_MIN_WIDTH+1 || xPos_snake[0] > MAP_WIDTH-1 || yPos_snake[0] < MAP_MIN_HEIGHT+1 || yPos_snake[0] > MAP_HEIGHT-1)
        GameOver();
}
void MoveSnake(void)
{
    Goto_XY(xPos_snake[snakeLen-1], yPos_snake[snakeLen-1], " ");

    if(foodFlag == true)    // 보너스 먹이를 먹으면
    {
        Goto_XY(xPos_snake[snakeLen], yPos_snake[snakeLen], " ");
    }
    for(int i = snakeLen ; i > 0 ; i--)
    {
        xPos_snake[i] = xPos_snake[i-1];
        yPos_snake[i] = yPos_snake[i-1];
    }

    if(currntKey == UP_KEY)
        yPos_snake[0] = --yPos_snake[0];

    else if(currntKey == LEFT_KEY)
        xPos_snake[0] = --xPos_snake[0];

    else if(currntKey == DOWN_KEY)
        yPos_snake[0] = ++yPos_snake[0];

    else if(currntKey == RIGHT_KEY)
        xPos_snake[0] = ++xPos_snake[0];
}

void GetKey(void)
{
    int input = _getch();
    
    if(input == ESC_KEY)    currntKey = ESC_KEY;

    else if(input == ENTER_KEY) currntKey = ENTER_KEY;

    else if(input == 0 || input == 224)
    {
        input = _getch();
        if(input == UP_KEY && lastKey != DOWN_KEY)
            currntKey = UP_KEY;
        else if(input == LEFT_KEY && lastKey != RIGHT_KEY)
            currntKey = LEFT_KEY;
        else if(input == DOWN_KEY && lastKey != UP_KEY)
            currntKey = DOWN_KEY;
        else if(input == RIGHT_KEY && lastKey != LEFT_KEY)
            currntKey = RIGHT_KEY;

        lastKey = currntKey;
    }
}
void ShowSnake(void)
{
    Goto_XY(xPos_snake[0], yPos_snake[0], "🟢");    // 뱀 머리 출력

    for(int i = 1 ; i < snakeLen ; i++)
    {
        Goto_XY(xPos_snake[i], yPos_snake[i], "🟩");
    }
    
}
void Goto_XY(int x, int y, char* str)
{
    COORD pos = {x*2, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("%s", str);
}

void ShowMap(void)
{
    for(int y = 0 ; y < MAP_HEIGHT+1 ; y++)
    {
        for(int x = 0 ; x < MAP_WIDTH+1 ; x++)
        {
            if(x >= MAP_MIN_WIDTH && y >= MAP_MIN_HEIGHT)
            {
                if(y == MAP_MIN_HEIGHT || y == MAP_HEIGHT || x == MAP_MIN_WIDTH || x == MAP_WIDTH)
                {
                    Goto_XY(x, y, "◼");
                }
            }
        }
    }
}