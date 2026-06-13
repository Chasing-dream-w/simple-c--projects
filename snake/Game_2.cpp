// Game_2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 控制台贪吃蛇
// 食物 *
// 蛇头 @
// 蛇身 ■

#include <iostream>
#include <random>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <queue>

const int WIDTH = 15;
const int HEIGHT = 20;
int MAP[HEIGHT][WIDTH] = { 0 };
int GENERATE_SNAKE[HEIGHT][WIDTH] = { 0 };
int SCORE = 0;
std::queue<std::pair<int, int>> snake_body;

struct FOOD {
    int x;
    int y;
    int value;
};

struct SNAKE {
    int x;
    int y;
    int direction;
};

// 碰撞检测
bool CheckCollision(SNAKE snake) {
    if (snake.x < 0 || snake.x >= WIDTH || snake.y < 0 || snake.y >= HEIGHT)
        return true;  // 碰到边界
    if (GENERATE_SNAKE[snake.y][snake.x] == 1) 
        return true;  // 碰到自己
    return false;
}

//生成食物
void GenerateFood(FOOD& food) {
    food.x = rand() % WIDTH;
    food.y = rand() % HEIGHT;
    food.value = rand() % 5 + 1; // 食物的分值在1到5之间
}

// 吃饭
bool EatFood(SNAKE snake, FOOD food) {
    if (snake.x == food.x && snake.y == food.y) {
        return true;
    }
    return false;
}

// 读取最高分
int GetHighestScore() {
    std::ifstream file("highscore.txt");
    int score=0;
    if (file.is_open()) {
        file>>score;
    }
    return score;
}

// 记录最高分
void SaveHighScore(int score) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
    }
}

// 隐藏光标
void HideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE;   // 隐藏光标
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

int main()
{   
    srand(time(0));
    SNAKE snake;
    snake.x = WIDTH / 2;
    snake.y = HEIGHT / 2;
    snake.direction = 0;
    FOOD food;
    GenerateFood(food);
    // 初始化蛇身队列，长度为 3（蛇尾 -> 蛇头）
    for (int i = 2; i >= 0; i--) {
        snake_body.push({ snake.y, snake.x - i });
    }
    // 标记 GENERATE_SNAKE：身体为 1，蛇头为 -1
    memset(GENERATE_SNAKE, 0, sizeof(GENERATE_SNAKE));
    int idx = 0;
    std::queue<std::pair<int, int>> temp = snake_body;
    while (!temp.empty()) {
        auto p = temp.front(); temp.pop();
        if (idx == 0) GENERATE_SNAKE[p.first][p.second] = -1; // 蛇头
        else GENERATE_SNAKE[p.first][p.second] = 1;
        idx++;
    }
    std::pair<int, int> pre_head = { snake.y, snake.x };
    system("cls");
    HideCursor();
    DWORD time = GetTickCount64();
    std::cout << "按下任意键开始游戏" << std::endl;
    int H = GetHighestScore();
    int bo = _getch();
    if (bo) {
        while (true) {
            int flag = 0;
            if (_kbhit()) {
                int ch = _getch();
                switch (ch) {
                case 'w':
                    if (snake.direction == 1) {
                        snake.y += 1;
                        if (!CheckCollision(snake)) {}
                        else flag = 1;
                        break;
                    }
                    snake.direction = 3;
                    snake.y -= 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                    break;
                case 'a':
                    if (snake.direction == 0) {
                        snake.x += 1;
                        if (!CheckCollision(snake)) {}
                        else flag = 1;
                        break;
                    }
                    snake.direction = 2;
                    snake.x -= 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                    break;
                case 's':
                    if (snake.direction == 3) {
                        snake.y -= 1;
                        if (!CheckCollision(snake)) {}
                        else flag = 1;
                        break;
                    }
                    snake.direction = 1;
                    snake.y += 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                    break;
                case 'd':
                    if (snake.direction == 2) {
                        snake.x -= 1;
                        if (!CheckCollision(snake)) {}
                        else flag = 1;
                        break;
                    }
                    snake.direction = 0;
                    snake.x += 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                    break;
                case 'q':
                    std::cout << "确定终止吗？Y/N" << std::endl;
                    while (true) {
                        int ch = _getch();
                        if (ch == 'Y' || ch == 'y') {
                            flag = 1;
                            break;
                        }
                        else if (ch == 'N' || ch == 'n') {
                            break;
                        }
                    }
                    break;
                case ' ':
                    std::cout << "游戏已暂停，按任意键继续" << std::endl;
                    _getch();
                    break;
                }
            }
            else {
                if (snake.direction == 0) {
                    snake.x += 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                }
                else if (snake.direction == 1) {
                    snake.y += 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                }
                else if (snake.direction == 2) {
                    snake.x -= 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                }
                else if (snake.direction == 3) {
                    snake.y -= 1;
                    if (!CheckCollision(snake)) {}
                    else flag = 1;
                }
            }

            snake_body.push({ snake.y, snake.x });
            if (flag) {
                std::cout << "游戏结束！得分：" << SCORE << std::endl;
                SaveHighScore(SCORE);
                break;
            }
            if (GetTickCount64() - time > 400) {
                MAP[food.y][food.x] = 2;
                if (EatFood(snake, food)) {
                    SCORE += food.value;
                    MAP[food.y][food.x] = 0;
                    GenerateFood(food);
                    MAP[food.y][food.x] = 2;
                    GENERATE_SNAKE[snake_body.front().first][snake_body.front().second] = 1;
                }
                else {
                    GENERATE_SNAKE[snake_body.front().first][snake_body.front().second] = 0;
                    snake_body.pop();
                }
                GENERATE_SNAKE[snake.y][snake.x] = -1;
                pre_head.first = snake.y;
                pre_head.second = snake.x;
                time = GetTickCount64();
            }

            system("cls");

            std::cout << "当前得分:" << SCORE << std::endl;
            if (H > SCORE) {}
            else 
                H = SCORE;
            std::cout << "最高分:" << H << std::endl;
            for (int j = 0; j < WIDTH+2; j++) std::cout << "-";
            std::cout << std::endl;
            for (int i = 0; i < HEIGHT; i++) {
                std::cout << "#";
                for (int j = 0; j < WIDTH; j++) {
                    if (MAP[i][j] == 2) std::cout << "*";
                    else if (GENERATE_SNAKE[i][j] == 1) std::cout << "■";
                    else if (GENERATE_SNAKE[i][j] == 0) std::cout << " ";
                    else if (GENERATE_SNAKE[i][j] == -1) std::cout << "@";
                }
                std::cout << "#" << std::endl;
            }
            for (int j = 0; j < WIDTH+2; j++) std::cout << "-";
            std::cout << std::endl;
            GENERATE_SNAKE[pre_head.first][pre_head.second] = 1;
            Sleep(400);
        }

    }
    return 0;
}

