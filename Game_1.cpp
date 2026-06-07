// Game_1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 控制台俄罗斯方块游戏

/* 
操作说明：
W - 旋转方块
A - 向左移动
S - 加速下落
D - 向右移动
R - 重新开始游戏
Q - 退出游戏
SPACE - 暂停/继续游戏
*/

#include <iostream>
#include <windows.h>
#include <conio.h>
#include <random>
#include <ctime>
#include <fstream>


const int WIDTH = 10;    
const int HEIGHT = 20;
int BLOCKS[HEIGHT][WIDTH] = {0};
const int SHAPES[7][4][4] = {
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0} },        // I
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0} },        // O
    {{1,1,1,0},{0,1,0,0},{0,0,0,0},{0,0,0,0} },        // T
    {{0,0,0,0},{0,1,1,0},{0,0,1,1},{0,0,0,0} },        // Z
    {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0} },        // S
    {{0,0,0,0},{0,0,0,1},{0,0,0,1},{0,0,1,1} },        // J
    {{0,0,0,0},{0,0,1,0},{0,0,1,0},{0,0,1,1} }         // L   
};
int ROTATEDSHAPES[7][4][4][4] = {0};
int SHOWBLOCKS[HEIGHT][WIDTH] = {0};
int SCORE = 0;



struct DowningBlock {
    int x;
    int y;
    int shapeID;
    int rotation;
};
// "■"

// 碰撞检测
bool CheckCollision(int x, int y, int shapeid, int rotation) {
    for (int i = 0; i < 4;i++) {
        for (int j = 0; j < 4;j++) {
            if (ROTATEDSHAPES[shapeid][rotation][i][j] == 1) {
                int new_x = x + j;
                int new_y = y + i;
                if (new_x < 0 || new_x > WIDTH - 1 || new_y > HEIGHT - 1) return true;
                if (new_y >= 0 && BLOCKS[new_y][new_x] != 0) return true;
            }
        }
    }
    return false;
}

// 固定方块
void FixBlock(int x, int y, int shapeid, int rotation) {
    for (int i = 0; i < 4;i++) {
        for (int j = 0; j < 4;j++) {
            if (ROTATEDSHAPES[shapeid][rotation][i][j] == 1) {
                int new_x = x + j;
                int new_y = y + i;
                if (new_x >= 0 && new_x <= WIDTH - 1 && new_y <= HEIGHT - 1)
                    BLOCKS[new_y][new_x] = 1;
                    SHOWBLOCKS[new_y][new_x] = 1;
            }
        }
    }
}

// 生成新方块
bool SpawnNewBlock(DowningBlock& block) {
    block.x = WIDTH / 2 - 2;
    block.y = 0;
    block.rotation = 0;
    block.shapeID = rand() % 7;
    if (CheckCollision(block.x, block.y, block.shapeID, block.rotation))
        return false;
    return true;
}

// 旋转方块
void Rotate90(int blocks[4][4], int dst[4][4]) {
    for(int i = 0; i < 4;i++){
        for (int j = 0; j < 4; j++) {
            dst[j][3 - i] = blocks[i][j];
        }
    }
}

// 消行
int ClearLines() {
    int linesCleared = 0;
    for (int i = HEIGHT - 1; i >= 0; i--) {
        bool fullLine = true;
        for (int j = 0; j < WIDTH; j++) {
            if (BLOCKS[i][j] == 0) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            linesCleared++;
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < WIDTH; j++) {
                    BLOCKS[k][j] = BLOCKS[k - 1][j];
                    SHOWBLOCKS[k][j] = SHOWBLOCKS[k - 1][j];
                }
            }
            for (int j = 0; j < WIDTH; j++) {
                BLOCKS[0][j] = 0;
                SHOWBLOCKS[0][j] = 0;
            }
            i++; // 检查当前行是否再次满行
        }
    }
    return linesCleared;
}

// 计分
int CaculateScore(int lines) {
    switch (lines) {
    case 1: return 100;
    case 2: return 300;
    case 3: return 500;
    case 4: return 800;
    default: return 0;
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

// 加载最高分
int LoadHighScore() {
    std::ifstream file("highscore.txt");
    int score = 0;
    if (file.is_open()) {
        file >> score;
    }
    return score;
}

// 保存最高分
void SaveHighScore(int score) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
    }
}

// 主函数
int main()
{   

    // 生成旋转后的方块
    for(int i = 0 ; i < 7; i++){
        for(int j = 0 ; j < 4; j ++){
            for(int k = 0; k < 4 ; k++){
                ROTATEDSHAPES[i][0][j][k] = SHAPES[i][j][k];
            }
        }
    }
    for(int i = 0; i < 7; i++){
        for(int j = 1; j < 4; j++){
            Rotate90(ROTATEDSHAPES[i][j-1], ROTATEDSHAPES[i][j]);
        }
    }

    srand(time(0));
    DowningBlock  block1;
    block1.x = WIDTH / 2 - 2;
    block1.y = 0;
    block1.rotation = 0;
    block1.shapeID = rand()%7;
	std::cout << "按下任意键开始游戏..." << std::endl;
    int bo = _getch();
    int highScore = LoadHighScore();
    if (bo) {
        system("cls");
        HideCursor();
        DWORD lastTime = GetTickCount();
        while (true) {
            if (_kbhit()) {
                int ch = _getch();
                if (ch == 'a') {
                    int newX = block1.x - 1;
                    if (!CheckCollision(newX, block1.y, block1.shapeID, block1.rotation))
                        block1.x = newX;
                }
                else if (ch == 'd') {
                    int newX = block1.x + 1;
                    if (!CheckCollision(newX, block1.y, block1.shapeID, block1.rotation))
                        block1.x = newX;
                }
                else if (ch == 's') {
                    int newY = block1.y + 1;
                    if (!CheckCollision(block1.x, newY, block1.shapeID, block1.rotation))
                        block1.y = newY;
                }
                else if (ch == 'w') {
                    if (!CheckCollision(block1.x, block1.y, block1.shapeID, (block1.rotation + 1) % 4))
                        block1.rotation = (block1.rotation + 1) % 4;
                }
                else if (ch == 'q') {
                    std::cout<< '\n' << "这将会终止...（Y/N）" << std::endl;
                    char choice = _getch();
                    if (choice == 'Y' || choice == 'y') break;
                    else{
                        std::cout << "继续游戏..." << std::endl;
                    }
                }
                else if (ch == 'r') {
                    std::cout << "\n" << "真的要重新开始吗？Y/N" << std::endl;
                    char choice = _getch();
                    if (choice == 'Y' || choice == 'y') {
                        SCORE = 0;
                        for (int i = 0; i < HEIGHT; i++) {
                            for (int j = 0; j < WIDTH; j++) {
                                BLOCKS[i][j] = 0;
                                SHOWBLOCKS[i][j] = 0;
                            }
                        }
                        SpawnNewBlock(block1);
                        continue;
                    }
                    else{
                        std::cout << "继续游戏..." << std::endl;
                    }
                }
                else if (ch == ' ') {
                     std::cout<< '\n' << "游戏已暂停...（按任意键继续）" << std::endl;
                     _getch();
                }
            }

            if (GetTickCount() - lastTime >= 400) {
                if (!CheckCollision(block1.x, block1.y + 1, block1.shapeID, block1.rotation)) {
                    // 方块下落
                    block1.y += 1;
                }
                else {
                    // 固定方块
                    FixBlock(block1.x, block1.y, block1.shapeID, block1.rotation);
                    // 检查行消除
                    int linesCleared = ClearLines();
                    // 计分
                    SCORE = SCORE + CaculateScore(linesCleared);
                    // 生成新方块
                    if (!SpawnNewBlock(block1)) {
                        std::cout << "Game Over!" << std::endl;
                        break;
                    }
                }
                lastTime = GetTickCount();
            }

            system("cls");
            // 绘制方块
            std::cout << "当前得分: " << SCORE << std::endl;
            if (SCORE > highScore) {
                highScore = SCORE;
            }
            std::cout<< "最高分: " << highScore << std::endl;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (ROTATEDSHAPES[block1.shapeID][block1.rotation][i][j] == 1) {
                        SHOWBLOCKS[block1.y + i][block1.x + j] = 1;
                    }
                }
            }

            for (int i = 0; i < WIDTH + 2; i++) std::cout << "=";
            std::cout << std::endl;
            for (int row = 0; row < HEIGHT; row++) {
                std::cout << "|";
                for (int col = 0; col < WIDTH; col++) {
                    if (SHOWBLOCKS[row][col] == 1) {
                        std::cout << "■";
                    }
                    else {
                        std::cout << " ";
                    }
                }
                std::cout << "|" << std::endl;
            }
            for (int i = 0; i < WIDTH + 2; i++) std::cout << "#";
            // 更新缓冲区
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    SHOWBLOCKS[i][j] = BLOCKS[i][j];
                }
            }
            Sleep(400);

        }
    }
    // 保存分数
    if (SCORE > highScore) {
        SaveHighScore(highScore);
        std::cout << "创造了新的最高分: " << highScore << std::endl;
    }
    std::cout << "已终止游戏" << std::endl;
    return 0;
}


