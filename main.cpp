#include "main.h"

std::string map;
std::vector<std::string> mapText;
int textIndex = 0;

std::vector<Pos> redrawList;
struct {
    bool reprint;
    int rChange;
    int cChange;
} reprint;

int main() {
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, windowWidth, windowHeight, TRUE);
    DWORD style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
    SetWindowPos(console, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED|SWP_NOZORDER);
    removeScrollbar();
    SetWindowTextW(console, L"Square RPG");

    //PlaySoundW(L"Beep Speech.wav", NULL, SND_FILENAME | SND_ASYNC);

    //std::ifstream m("C:/Users/ellys/source/repos/SquareRPG/map1.txt");
    //std::ifstream m("map3.txt");
    std::ifstream m("newMap2.txt");
	map = std::string((std::istreambuf_iterator<char>(m)), std::istreambuf_iterator<char>());
    /*std::fflush(stdout);
    _setmode(_fileno(stdout), 0x00020000); // _O_U16TEXT
    std::wcout << L"Hello, ĐĄßĞĝ!\n";
    std::fflush(stdout);
    _setmode(_fileno(stdout), _O_TEXT);*/
    /*initMap(map);
    loadAnimation();*/
    initMap2(map);
    printBox();
    printScreen2();
    //std::cout << playerPos->r << "," << playerPos->c;
    keyPress();
}

void initCell(Cell c) {
    c.type = ' ';
    c.value = 0;
    c.fore = 0;
    c.back = 0;
    c.isPlayer = false;
    c.isEnemy = false;
    c.isNPC = false;
    c.isStart = false;
}

Cell copyCell(Cell c) {
    Cell newCell;
    newCell.type = c.type;
    newCell.value = c.value;
    newCell.fore = c.fore;
    newCell.back = c.back;
    newCell.isPlayer = c.isPlayer;
    newCell.isEnemy = c.isEnemy;
    newCell.isNPC = c.isNPC;
    newCell.isStart = c.isStart;
    return newCell;
}

void initMap2(std::string map) {
    playerPos->r = -1;
    playerPos->c = -1;
    screenPos.r = -1;
    screenPos.c = -1;
    rows = std::stoi(map.substr(0, 2));
    cols = std::stoi(map.substr(3, 2));
    int idx = 6, r = 0;
    struct cell2 {
        char type;
        Cell cell;
    };
    cell2 lastCell = {
        0,
        {
            ' ',
            0,
            0,
            0,
            false,
            false,
            false,
            false
        }
    };
    initCell(lastCell.cell);
    frames[0] = std::vector<std::vector<Cell>>(rows, std::vector<Cell>());
    frames[1] = std::vector<std::vector<Cell>>(rows, std::vector<Cell>());
    bool start = false;
    while (idx < (int)map.length()) {
        const char type = map[idx];
        if (type == '^') {
            start = true;
            idx++;
            continue;
        } else if (type == '|') {
            if (lastCell.type != 0 && lastCell.type != '*')
                frames[1][r].push_back(copyCell(lastCell.cell));
            idx++;
            r++;
            lastCell.type = 0;
            initCell(lastCell.cell);
            continue;
        }
        const std::string cellStr = map.substr(idx + 1, 3);
        cell2 cell = {
            type,
            {
                type,
                cellStr[0],
                std::stoi(std::string(1, cellStr[1]), 0, 16),
                std::stoi(std::string(1, cellStr[2]), 0, 16),
                type == '+',
                type == '-',
                type == '!',
                start
            }
        };
        //std::cout << cell.type << "," << cell.cell.value << "," << cell.cell.fore << "," << cell.cell.back << "," << cell.cell.isPlayer << "," << cell.cell.isNPC << "," << cell.cell.isStart << "\n";
        start = false;
        if (type == '*') {
            animChangeList.push_back({
                r,
                (int)frames[0][r].size() - 1
            });
            cell.cell.isPlayer = lastCell.cell.isPlayer;
            cell.cell.isEnemy = lastCell.cell.isEnemy;
            if (lastCell.cell.isNPC) {
                cell.cell.isNPC = lastCell.cell.isNPC;
                cell.cell.value = lastCell.cell.value;
                cell.cell.fore = lastCell.cell.fore;
            }
            cell.cell.isStart = lastCell.cell.isStart;
            frames[1][r].push_back(copyCell(cell.cell));
        } else {
            frames[0][r].push_back(copyCell(cell.cell));
            if (cell.cell.isPlayer && playerPos->r == -1) {
                playerPos->r = r;
                playerPos->c = frames[0][r].size() - 1;
            }
            if (cell.cell.isEnemy) {
                Pos* p = (Pos*)malloc(sizeof(Pos));
                p->r = r;
                p->c = frames[0][r].size() - 1;
                enemyPos.push_back(p);
            }
            if (cell.cell.isStart && screenPos.r == -1) {
                screenPos.r = r;
                screenPos.c = frames[0][r].size() - 1;
            }
            if (lastCell.type != 0 && lastCell.type != '*')
                frames[1][r].push_back(copyCell(lastCell.cell));
        }
        lastCell = {
            cell.type,
            copyCell(cell.cell)
        };
        idx += 4;
    }
    if (lastCell.type != 0 && lastCell.type != '*')
        frames[1][r].push_back(copyCell(lastCell.cell));
    /*for (int i = 0; i < 2; i++) {
        for (int j = 0; j < rows; j++) {
            for (int k = 0; k < cols; k++)
                std::cout << frames[i][j][k].type << "," << frames[i][j][k].value << "," << frames[i][j][k].fore << "," << frames[i][j][k].back << "," << frames[i][j][k].isPlayer << "," << frames[i][j][k].isNPC << "," << frames[i][j][k].isStart << "\n";
        }
    }*/
}

void initMap(std::string map) {
    rows = 1, cols = 0;
	for (int i = 0; i < (int)map.length(); i++) {
		if (map.at(i) == '\n') {
			rows++;
            if (!cols)
                cols = i / 2;
        }
	}
    if (rows <= screenSize && cols <= screenSize)
        screenPos = {0, 0};
    screenPos = {0, 0};
    mapCoord = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0));
    std::istringstream f(map);
	std::string line;
    int r = 0;
    while (getline(f, line)) {
        for (int c = 0; c < (int)line.length(); c += 2) {
            char ch = line.at(c);
            int val = 0;
            if (ch == '\\') {
                ch = line.at(c + 1);
                switch (ch) {
                    case '!':
                        val = M_NPC;
                        break;
                    case '+':
                        val = M_PLAYER;
                        playerPos->r = r;
                        playerPos->c = c / 2;
                        break;
                    case '1':
                        screenPos.r = r;
                        screenPos.c = c / 2;
                    case '*':
                        val = M_WALL;
                        break;
                    case '-': {
                        val = M_ENEMY;
                        Pos* p = (Pos*)malloc(sizeof(Pos));
                        p->r = r;
                        p->c = c / 2;
                        enemyPos.push_back(p);
                        break;
                    } case '0':
                        screenPos.r = r;
                        screenPos.c = c / 2;
                        break;
                }
            } else if (ch != ' ' || line.at(c + 1) != ' ') {
                val = M_TEXT + textIndex++;
                mapText.push_back(line.substr(c, 2));
            }
            mapCoord[r][c / 2] = val;
        }
        r++;
    }
}

void updateDisplay(int oldR, int oldC, int newR, int newC) {
    if (oldR >= screenPos.r && oldR < screenPos.r + screenSize && oldC >= screenPos.c && oldC < screenPos.c + screenSize * 2) {
        Pos oldPos = mapToScreen({oldR, oldC});
        setCursor(rOffset + oldPos.r, cOffset + oldPos.c);
        for (int i = 0; i < 2; i++) {
            setColor2(frames[currFrame][oldR][oldC + i]);
            const char val = frames[currFrame][oldR][oldC + i].value;
            printf("%c", oldPos.r == screenSize - 1 && val == ' ' ? '_' : val);
        }
    }
    Pos newPos = mapToScreen({newR, newC});
    setCursor(rOffset + newPos.r, cOffset + newPos.c);
    setColor2(frames[currFrame][newR][newC]);
    printf("  ");
    reset();
}

void changePos(Pos* pos, int newR, int newC, bool player) {
    if (newR >= 0 && newR < rows && newC >= 0 && newC < cols && frames[currFrame][newR][newC].type == ' ') {
        int rChange = 0, cChange = 0;
        if (player) {
            for (int i = 0; i < 2; i++) {
                frames[i][pos->r][pos->c].type = ' ';
                frames[i][pos->r][pos->c + 1].type = ' ';
                frames[i][newR][newC].type = '+';
                frames[i][newR][newC + 1].type = '+';
            }
            if (newR >= screenPos.r + screenSize - screenThreshold && screenPos.r + screenSize < rows) {
                rChange++;
            } else if (screenPos.r - 1 >= 0 && newR < screenPos.r + screenThreshold) {
                rChange--;
            } else if (newC >= screenPos.c + screenSize * 2 - screenThreshold * 2 && screenPos.c + screenSize * 2 < cols) {
                cChange++;
            } else if (screenPos.c - 1 >= 0 && newC < screenPos.c + screenThreshold * 2) {
                cChange--;
            }
        } else {
            for (int i = 0; i < 2; i++) {
                frames[i][pos->r][pos->c].type = ' ';
                frames[i][pos->r][pos->c + 1].type = ' ';
                frames[i][newR][newC].type = '-';
                frames[i][newR][newC + 1].type = '-';
            }
        }
        if (rChange + cChange != 0) {
            reprint = {
                true,
                rChange,
                cChange * 2
            };
        } else if (newR >= screenPos.r && newR < screenPos.r + screenSize && newC >= screenPos.c && newC < screenPos.c + screenSize * 2) {
            redrawList.push_back({
                pos->r,
                pos->c
            });
            redrawList.push_back({
                newR,
                newC
            });
        }
        pos->r = newR;
        pos->c = newC;
    }
}

void updateScreen(int dir) {
    if (reprint.reprint) {
        for (int i = 0; i < (int)redrawList.size(); i += 2) {
            Pos pos = redrawList[i];
            if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
                Pos oldPos = mapToScreen({pos.r, pos.c});
                setCursor(rOffset + oldPos.r, cOffset + oldPos.c);
                for (int i = 0; i < 2; i++) {
                    setColor2(frames[currFrame][pos.r][pos.c + i]);
                    const char val = frames[currFrame][pos.r][pos.c + i].value;
                    printf("%c", oldPos.r == screenSize - 1 && val == ' ' ? '_' : val);
                }
            }
        }
        redrawList.clear();
        screenPos.r += reprint.rChange;
        screenPos.c += reprint.cChange;
        for (int r = 0; r < screenSize; r++) {
            for (int c = 0; c < screenSize * 2; c++) {
                Pos pos = screenToMap({r, c});
                if (dir == 0 && pos.r + 1 != rows && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r + 1][pos.c]) {
                    continue;
                } else if (dir == 1 && pos.r - 1 != -1 && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r - 1][pos.c]) {
                    continue;
                } else if (dir == 2 && pos.c + 2 < cols && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r][pos.c + 2]) {
                    continue;
                } else if (dir == 3 && pos.c - 2 >= 0 && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r][pos.c - 2]) {
                    continue;
                } else {
                    setCursor(rOffset + r, cOffset + c);
                    char val = frames[currFrame][pos.r][pos.c].value;
                    setColor2(frames[currFrame][pos.r][pos.c]);
                    printf("%c", r == screenSize - 1 && val == ' ' ? '_' : val);
                }
            }
        }
        reprint.reprint = false;
    } else {
        for (Pos pos : redrawList) {
            if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
                Pos oldPos = mapToScreen({pos.r, pos.c});
                setCursor(rOffset + oldPos.r, cOffset + oldPos.c);
                for (int i = 0; i < 2; i++) {
                    setColor2(frames[currFrame][pos.r][pos.c + i]);
                    const char val = frames[currFrame][pos.r][pos.c + i].value;
                    printf("%c", oldPos.r == screenSize - 1 && val == ' ' ? '_' : val);
                }
            }
        }
        redrawList.clear();
        reset();
    }
}

void keyPress() {
    int kbCode = 0;
    std::chrono::duration<double> elapsed = std::chrono::duration<double>::zero();
    while (kbCode != KB_ESCAPE) {
        auto start = std::chrono::system_clock::now();
        if (kbhit()) {
            kbCode = getch();
            if (kbMode == MOVE) {
                if (kbCode == 0 || kbCode == 224) {
                    int rChange = 0, cChange = 0;
                    int dir = -1;
                    switch (getch()) {
                        case KB_UP:
                            rChange--;
                            dir = 0;
                            break;
                        case KB_DOWN:
                            rChange++;
                            dir = 1;
                            break;
                        case KB_LEFT:
                            cChange--;
                            dir = 2;
                            break;
                        case KB_RIGHT:
                            cChange++;
                            dir = 3;
                            break;
                    }
                    changePos(playerPos, playerPos->r + rChange, playerPos->c + cChange * 2, true);
                    enemyAI();
                    updateScreen(dir);
                } else if (kbCode == KB_SPACE) {
                    enemyAI();
                    updateScreen(-1);
                } else if (kbCode == KB_TAB) {
                    kbMode = MENU;
                    updateSelection();
                } else if (kbCode == KB_ENTER) {
                    bool foundNPC = false;
                    const int options[4][2] = {{1, 0}, {-1, 0}, {0, 2}, {0, -2}};
                    for (int i = 0; !foundNPC && i < 4; i++)
                        foundNPC = frames[currFrame][playerPos->r + options[i][0]][playerPos->c + options[i][1]].type == '!';
                    if (foundNPC) {
                        changeWindow(890, 700);
                        Sleep(500);
                        conversation("Dialogue1.txt");
                        Sleep(1000);
                        screenClose(convoSize, screenSize, conversationROffset, conversationCOffset);
                        changeWindow(ORIGINAL_WINDOW_WIDTH, 700);
                    }
                }
            } else if (kbMode == MENU) {
                if (kbCode == 0 || kbCode == 224) {
                    switch (getch()) {
                        case KB_LEFT:
                            if (selection > 0)
                                updateSelection('L');
                            break;
                        case KB_RIGHT:
                            if (selection < MENU_SIZE - 1)
                                updateSelection('R');
                            break;
                    }
                } else if (kbCode == KB_TAB) {
                    kbMode = MOVE;
                    printMenu(0);
                }
            }
        }
        auto end = std::chrono::system_clock::now();
        elapsed += end - start;
        if (elapsed >= std::chrono::milliseconds(500)) {
            currFrame = -currFrame + 1;
            for (Pos pos : animChangeList) {
                if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
                    Pos oldPos = mapToScreen({pos.r, pos.c});
                    setCursor(rOffset + oldPos.r, cOffset + oldPos.c);
                    setColor2(frames[currFrame][pos.r][pos.c]);
                    const char val = frames[currFrame][pos.r][pos.c].value;
                    printf("%c", oldPos.r == screenSize - 1 && val == ' ' ? '_' : val);
                }
            }
            reset();
            elapsed = std::chrono::duration<double>::zero();
        }
    }
}

Node* nodeInit(int r, int c, int f, int g, Node* prev) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->r = r;
    n->c = c;
    n->f = f;
    n->g = g;
    n->prev = prev;
    return n;
}

std::vector<Node*> createPath(Node* current) {
    std::vector<Node*> totalPath;
    totalPath.push_back(current);
    while (current->prev != NULL) {
        current = current->prev;
        totalPath.insert(totalPath.begin(), current);
    }
    return totalPath;
}

int heuristic(Node node) {
    return std::abs(node.r - playerPos->r) + (std::abs(node.c - playerPos->c) / 2);
}

Node* findMin(std::vector<Node*> list) {
    Node* min = nodeInit(0, 0, INT_MAX, 0, NULL);
    for (Node* n : list) {
        if (n->f < min->f)
            min = n;
    }
    return min;
}

std::vector<Node*> pathfind(Node start, Node goal) {
    std::vector<Node*> openSet;
    std::vector<Pos> closedSet;
    start.g = 0;
    start.f = heuristic(start);
    openSet.push_back(&start);
    while (!openSet.empty()) {
        Node* current = findMin(openSet);
        if (*current == goal)
            return createPath(current);
        openSet.erase(std::find(openSet.begin(), openSet.end(), current));
        Pos pos = {current->r, current->c};
        if (std::find(closedSet.begin(), closedSet.end(), pos) != closedSet.end())
            continue;
        closedSet.push_back(pos);
        const int options[4][2] = {{1, 0}, {-1, 0}, {0, 2}, {0, -2}};
        for (int i = 0; i < 4; i++) {
            int newR = current->r + options[i][0];
            int newC = current->c + options[i][1];
            if (newR < 0 || newR == rows || newC < 0 || newC == cols || (frames[currFrame][newR][newC].type != ' ' && frames[currFrame][newR][newC].type != '+'))
                continue;
            Pos p = {newR, newC};
            if (std::find(closedSet.begin(), closedSet.end(), p) != closedSet.end())
                continue;
            Node* neighbor = nodeInit(newR, newC, INT_MAX, INT_MAX, current);
            for (Node* n : openSet) {
                if (*n == *neighbor) {
                    neighbor = n;
                    break;
                }
            }
            int newG = current->g + 1;
            if (newG < neighbor->g) {
                neighbor->prev = nodeInit(current->r, current->c, current->f, current->g, current->prev);
                neighbor->g = newG;
                neighbor->f = newG + heuristic(*neighbor);
                bool in = false;
                for (int j = 0; !in && j < (int)openSet.size(); j++) {
                    if (*openSet[j] == *neighbor) {
                        openSet[j] = neighbor;
                        in = true;
                    }
                }
                if (!in)
                    openSet.push_back(neighbor);
            }
        }
    }
    return std::vector<Node*>();
}

void enemyAI() {
    for (Pos* pos : enemyPos) {
        std::vector<Node*> list = pathfind({pos->r, pos->c, INT_MAX, INT_MAX, NULL}, {playerPos->r, playerPos->c, 0, 0, NULL});
        if (!list.empty() && !(list[1]->r == playerPos->r && list[1]->c == playerPos->c))
            changePos(pos, list[1]->r, list[1]->c, false);
    }
}

void updateSelection() {
    setCursor(menuPos[selection].r, menuPos[selection].c);
    setColor("LIGHT_GRAY", "BLACK");
    printf(menu[selection].c_str());
    reset();
}

void updateSelection(char dir) {
    setCursor(menuPos[selection].r, menuPos[selection].c);
    setColor(0);
    printf(menu[selection].c_str());
    if (dir == 'L') {
        selection--;
    } else if (dir == 'R') {
        selection++;
    }
    updateSelection();
}
