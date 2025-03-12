#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>

// 💀🍎🍏🍗▒🧱▄ ☠💣☠■
// map size
#define MAP_SIZE_X 40
#define MAP_SIZE_Y 20

#define MAP_MIN_WIDTH 5
#define MAP_MIN_HEIGHT 2
#define MAP_WIDTH (MAP_SIZE_X + MAP_MIN_WIDTH)
#define MAP_HEIGHT (MAP_SIZE_Y + MAP_MIN_HEIGHT)
#define MAP_XPOS_CENTER (MAP_SIZE_X / 2 + MAP_MIN_WIDTH)
#define MAP_YPOS_CENTER (MAP_SIZE_Y / 2 + MAP_MIN_HEIGHT)

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

#define FOOD 0
#define OBSTACLE 1
#define WALL 2
#define SNAKE 3

int xPos_snake[50];
int yPos_snake[50];
int snakeLen;
clock_t snakeTime;

int foodXpos[100];
int foodYpos[100];
int foodType[100];
int foodLen;
time_t foodTime;

int obstacleXpos[25];
int obstacleYpos[25];
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

void Goto_XY(float x, int y, char* str);// xy좌표에 출력
void GetKey(void); // 키 입력
bool SelectYorN(int yOffset);  // Yes = 1, No = 2 반환 함수
void ShowScore(void); // 점수 출력

void InitSnake(void);   // 뱀 초기값
void MoveSnake(void);   // 뱀 move

void RandomPos(int* xPtr, int* yPtr);   // 랜덤 좌표 생성
void GenerateFood(void);    // 먹이 생성
void CheckFoodCollision(void);  // 먹이와 충돌 검사

void GenerateObstacle(void);    // 장애물 생성
void DelectObstacle(void);      // 장애물 삭제

void CheckObstacleCollision(void);   // 장애물과 충돌 검사
void CheckWallCollision(void);   // 벽과 충돌 검사
void CheckSnakeCollision(void);  // 몸통과 출동 검사

void PauseGame(void);    // 게임정지
void GameOver(void);    // 게임오버


int main(void)
{   
    hideCursor();   // 커서 지우기
    ShowMap();  // Map 출력
    InitSnake();    // 게임 초기화
    srand(time(NULL));  // 난수 생성
    

    while(1)
    {
        if(kbhit()) GetKey();   // 키 입력 반환    
        MoveSnake();    // 뱀 이동 후 출력
        
        GenerateFood(); // 먹이 생성
        CheckFoodCollision();   // 먹이와 충돌 검사

        GenerateObstacle(); // 장애물 생성
        DelectObstacle();   // 일정시간 경과 후 장애물 삭제
        
        
        CheckObstacleCollision();   // 장애물과 충돌 검사
        CheckSnakeCollision();      // 몸통과 출동 검사
        CheckWallCollision();       // 벽과 충돌 검사
    }
}

void ShowMap(void)
{
    system("clear");
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

void hideCursor(void)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void Goto_XY(float x, int y, char* str)
{
    COORD pos = {x*2, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("%s", str);
}

void GetKey(void)
{
    int input = _getch();
    
    if(input == ESC_KEY)    PauseGame();

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
    }
}

bool SelectYorN(int yOffset)
{
    // Yes = 1, No = 2 반환 함수
    int flag = 1;
    Goto_XY(MAP_XPOS_CENTER-2, MAP_YPOS_CENTER+(yOffset)-1, "            ");
    Goto_XY(MAP_XPOS_CENTER-2, MAP_YPOS_CENTER+(yOffset),   "  > Yes     ");
    Goto_XY(MAP_XPOS_CENTER-2, MAP_YPOS_CENTER+(yOffset)+1, "    NO      ");
    Goto_XY(MAP_XPOS_CENTER-2, MAP_YPOS_CENTER+(yOffset)+2, "            ");

    
    while(1)
    {
        int key = _getch();
        if(key == 0 || key == 224)
        {
            key = _getch();
            if(key == UP_KEY)
                flag = 1;
            else if(key == DOWN_KEY)
                flag = 0;
        }
        else if(key == ENTER_KEY)
        {
            Goto_XY(MAP_XPOS_CENTER-7, MAP_YPOS_CENTER+(yOffset)-2, "                          ");
            Goto_XY(MAP_XPOS_CENTER-2, MAP_YPOS_CENTER+(yOffset),   "         ");
            Goto_XY(MAP_XPOS_CENTER-2, MAP_YPOS_CENTER+(yOffset)+1, "         ");

            // 문구가 뱀이랑 겹치면 뱀모양 다시 그리기
            Goto_XY(xPos_snake[0], yPos_snake[0], "🟢");    
            for(int i = 1 ; i < snakeLen ; i++)
            {
                if(MAP_XPOS_CENTER-10 <= xPos_snake[i] <= MAP_XPOS_CENTER+10 && MAP_YPOS_CENTER-10 <= yPos_snake[i] <= MAP_YPOS_CENTER+10)
                {
                    Goto_XY(xPos_snake[i], yPos_snake[i], "🟩");    
                }       
            }
            
            // 문구가 먹이랑 겹치면 다시 그리기
            for(int i = 0 ; i < foodLen ; i++)
            {
                if(MAP_XPOS_CENTER-10 <= foodXpos[i] <= MAP_XPOS_CENTER+10 && MAP_YPOS_CENTER-10 <= foodYpos[i] <= MAP_YPOS_CENTER+10)
                {
                    if(foodType[i] == SPECIAL_FOOD)    
                        Goto_XY(foodXpos[i], foodYpos[i], "🍗");
                    else
                        Goto_XY(foodXpos[i], foodYpos[i], "🍎");
                }
            }

            // 문구가 장애물이랑 겹치면 다시 그리기
            for(int i = 0 ; i < obstacleLen ; i++)
            {
                if(MAP_XPOS_CENTER-10 <= obstacleXpos[i] <= MAP_XPOS_CENTER+10 && MAP_YPOS_CENTER-10 <= obstacleYpos[i] <= MAP_YPOS_CENTER+10)
                {
                    Goto_XY(obstacleXpos[i], obstacleYpos[i], "💀");
                }
            }

            if(flag == 1)
                return true;
                
            else  
                return false;

        }

        if(flag == 1)
        {
            Goto_XY(MAP_XPOS_CENTER-1, MAP_YPOS_CENTER+(yOffset),   "> Yes");
            Goto_XY(MAP_XPOS_CENTER-1, MAP_YPOS_CENTER+(yOffset)+1, "  NO");
        }
        else
        {
            Goto_XY(MAP_XPOS_CENTER-1, MAP_YPOS_CENTER+(yOffset),   "  Yes");
            Goto_XY(MAP_XPOS_CENTER-1, MAP_YPOS_CENTER+(yOffset)+1, "> NO");
        }
    }
}

void ShowScore(void)
{
    Goto_XY(MAP_WIDTH-4, MAP_HEIGHT+1, "Score: ");
    printf("%d", score);
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
    ShowScore();

    foodTime = time(NULL) + FOOD_TIME;
    obstacleTime = time(NULL) + OBSTACLE_TIME;
    obstacleDelectTime = obstacleTime + OBSTACLE_DELECT_TIME;


    for(int i = 1 ; i < snakeLen ; i++)
    {
        xPos_snake[i] = xPos_snake[i-1] - 1;
        yPos_snake[i] = yPos_snake[0];
    }
}

void MoveSnake(void)
{
    if(clock() >= snakeTime)
    {
        Goto_XY(xPos_snake[snakeLen-1], yPos_snake[snakeLen-1], " ");

        for(int i = snakeLen ; i > 0 ; i--)
        {
            xPos_snake[i] = xPos_snake[i-1];
            yPos_snake[i] = yPos_snake[i-1];
        }

        if(currntKey == UP_KEY)
            yPos_snake[0]--;

        else if(currntKey == LEFT_KEY)
            xPos_snake[0]--;

        else if(currntKey == DOWN_KEY)
            yPos_snake[0]++;

        else if(currntKey == RIGHT_KEY)
            xPos_snake[0]++;

        
        Goto_XY(xPos_snake[0], yPos_snake[0], "🟢");
        Goto_XY(xPos_snake[1], yPos_snake[1], "🟩");

        lastKey = currntKey;
        snakeTime = clock() + speed;
    }
}

void RandomPos(int* xPtr, int* yPtr)
{
    while(1)
    {
        int flag = 0;

        // 랜덤 좌표
        *xPtr = rand() % ((MAP_WIDTH - 1) - (MAP_MIN_WIDTH + 1) + 1) + (MAP_MIN_WIDTH + 1);
        *yPtr = rand() % ((MAP_HEIGHT - 1) - (MAP_MIN_HEIGHT + 1) + 1) + (MAP_MIN_HEIGHT + 1);

        for(int i = 0 ; i < snakeLen ; i++) // 생성이 뱀과 겹치지 않게
        {
            if(*xPtr == xPos_snake[i] && *yPtr == yPos_snake[i])
            { 
                flag = 1;
                break;
            }
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

void CheckFoodCollision(void)       
{
    int flag = 0;
    int i = 0;
    for(i ; i < foodLen ; i++)
    {
        if(xPos_snake[0] == foodXpos[i] && yPos_snake[0] == foodYpos[i] && flag == 0)
        {
            if(foodType[i] == SPECIAL_FOOD)    // 보너스 먹이
            {   
                if(snakeLen > 3)
                {
                    Goto_XY(xPos_snake[snakeLen-1], yPos_snake[snakeLen-1], " ");
                    snakeLen--;
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
            
            ShowScore();
            speed = speed < MAX_SPEED ? MAX_SPEED : speed;  // 뱀 최대 속도 제한
            break;
        }
    }

    if(flag == 1)
    {
        for(int k = i ; k < foodLen-1 ; k++)
        {    
            foodXpos[k] = foodXpos[k+1];
            foodYpos[k] = foodYpos[k+1];
            foodType[k] = foodType[k+1];
        }
        foodLen--;
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
    
    // 뱀 속도 빠를수록 장애물 생성 속도 증가 (단위: sec)
    obstacleSpawnSpeed = speed < 50 ? 4 : speed < 150 ? 2 : 0;
}

void DelectObstacle(void)
{
    if(time(NULL) >= obstacleDelectTime && obstacleLen > 0)
    {
        Goto_XY(obstacleXpos[0], obstacleYpos[0], " ");
        
        for(int i = 0 ; i < obstacleLen-1 ; i++)
        {
            obstacleXpos[i] = obstacleXpos[i+1];
            obstacleYpos[i] = obstacleYpos[i+1];
        }
        obstacleLen--;
        obstacleDelectTime = time(NULL) + OBSTACLE_TIME;   // 다음 장애물 삭제 시간
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

void CheckWallCollision(void)
{
    if(xPos_snake[0] < MAP_MIN_WIDTH+1 || xPos_snake[0] > MAP_WIDTH-1 || yPos_snake[0] < MAP_MIN_HEIGHT+1 || yPos_snake[0] > MAP_HEIGHT-1)
        GameOver();
}

void CheckSnakeCollision(void)
{
    for(int i = 1 ; i < snakeLen ; i++)
    {
        if(xPos_snake[0] == xPos_snake[i] && yPos_snake[0] == yPos_snake[i])
            GameOver();
    }
}

void PauseGame(void)
{
    Goto_XY(MAP_XPOS_CENTER-6, MAP_YPOS_CENTER-3, "  <  Continue Game ?  >  ");

    if(SelectYorN(-1) == false)    // No 선택 시
        GameOver();
}

void GameOver(void)
{
    Goto_XY(MAP_XPOS_CENTER-3, MAP_YPOS_CENTER-3, "<  Game Over  >");
    Goto_XY(MAP_XPOS_CENTER-1, MAP_YPOS_CENTER-1,   "Score: ");
    printf("%d", score);

    int key;
    while(key != ENTER_KEY) // Enter 키를 누르면 종료
    {
        key = _getch();
    }
    
    Goto_XY(MAP_XPOS_CENTER-3, MAP_YPOS_CENTER-3, "<  New Game?  >");

    if(SelectYorN(-1) == false)
        exit(0);
        
    ShowMap();
    InitSnake();

}