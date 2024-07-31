#pragma once

#include <vector>
#include <windows.h>
#include <string>

#define M_NPC 50
#define M_PLAYER 1
#define M_WALL -1
#define M_ENEMY -5
#define M_TEXT 100

#define ORIGINAL_WINDOW_WIDTH 355
#define ORIGINAL_WINDOW_HEIGHT 408

extern HANDLE hConsole;
extern HWND console;
extern RECT r;

extern int windowWidth;
extern int windowHeight;

extern int rows, cols;

struct Pos {
    int r, c;

    bool operator==(Pos p) {
        return r == p.r && c == p.c;
    }

    /*bool operator==(Pos* p) {
        return r == p->r && c == p->c;
    }*/
};
extern Pos* playerPos;

struct Cell {
    char type;
    char value;
    int fore;
    int back;
    bool isPlayer;
    bool isEnemy;
    bool isNPC;
    bool isStart;

    bool operator==(Cell c) {
        return c.type == type /*&& c.value == value && c.fore == fore && c.back == back && c.isPlayer == isPlayer && c.isEnemy == isEnemy*/;
    }
};

extern std::vector<std::vector<Cell>> frames[2];
extern std::vector<std::vector<int>> mapCoord;
extern std::vector<std::string> mapText;
extern int textIndex;

extern Pos screenPos;
Pos screenToMap(Pos pos);
Pos mapToScreen(Pos pos);

void loopFunctions(int n, int startDelay, int delay, void (*startFunc)(), std::vector<void (*)()> funcs);
void setCursor(short r, short c);
void setColor(std::string background, std::string text);
void setColor(int val);
void setColorCell(Cell c);
void reset();
void removeScrollbar();