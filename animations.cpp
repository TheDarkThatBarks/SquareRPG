#include "animations.hpp"

const int screenSize = 15;
const int rOffset = 3;
const int cOffset = 6;

const int menuROffset = 2;
const int menuCOffset = 3;
const std::string menu[MENU_SIZE] = {"TEST1", "TEST2", "TEST3"};
std::vector<Pos> menuPos;

const int conversationROffset = 3;
const int conversationCOffset = cOffset + screenSize * 2 + 7;
const int convoSize = screenSize * 2;
const int maxCharsConvo = convoSize * 2 - 9;
std::string currentDialogue;

int currFrame = 0;
std::vector<Pos> animChangeList;

std::vector<Pos> redrawList;
std::vector<Projectile> projectileList;
Reprint reprint;
std::vector<npcID> npcIDs;
std::vector<EnemyPos*> enemyPos;

// Runs start animation
void loadAnimation() {
    std::vector<void (*)()> funcs;

    Sleep(500);
    screenLoad(screenSize, screenSize, rOffset, cOffset);

    funcs.push_back(&clearScreen);
    funcs.push_back(&printBox);
    //loopFunctions(3, 500, 50, &printBox, funcs);

    funcs.push_back(&printMapBasic);
    loopFunctions(3, 500, 50, &printMapBasic, funcs);

    funcs.clear();
    funcs.push_back(&printMapBasic);
    funcs.push_back(&printScreen);
    loopFunctions(3, 500, 50, &printScreen, funcs);

    Sleep(500);
    printMenu(1);

    Sleep(500);
    /*changeWindow(890, 700);
    Sleep(500);
    conversation("Dialogue1.txt");
    Sleep(1000);
    //clearConvoBox();
    screenClose(convoSize, screenSize, conversationROffset, conversationCOffset);
    changeWindow(ORIGINAL_WINDOW_WIDTH, 700);*/
}

// Displays the section of the map contained within the screen bounds
void printScreen() {
    for (int r = 0; r < screenSize; r++) {
        for (int c = 0; c < screenSize * 2; c++)
            printCell(screenToMap({r, c}), {r, c});
    }
}

char computePrintVal(Pos pos, Pos coord) {
    char val = frames[currFrame][pos.r][pos.c].value;
    if (frames[currFrame][pos.r][pos.c / 2 * 2].type == '-') {
        for (EnemyPos* enemy : enemyPos) {
            if (Pos(pos.r, pos.c / 2 * 2) == *(enemy->pos)) {
                val = enemy->type;
                break;
            }
        }
    }
    return coord.r == screenSize - 1 && val == ' ' ? '_' : val;
}

// Prints the cell at a position on the map at a coordinate on the screen
void printCell(Pos pos, Pos coord) {
    /*WORD color = computeColor(frames[currFrame][pos.r][pos.c]);
    char buffer[] = {computePrintVal(pos, coord)};
    DWORD written;
    WriteConsoleOutputAttribute(hConsole, &color, 1, {(short)(cOffset + coord.c), (short)(rOffset + coord.r)}, &written);
    WriteConsoleOutputCharacterA(hConsole, buffer, 1, {(short)(cOffset + coord.c), (short)(rOffset + coord.r)}, &written);*/
    /*CHAR_INFO buffer[1];
    CHAR_INFO b = {
        (CHAR)computePrintVal(pos, coord),
        (WORD)computeColor(frames[currFrame][pos.r][pos.c])
    };
    buffer[0] = b;
    SMALL_RECT s = {
        (SHORT)(cOffset + coord.c),
        (SHORT)(rOffset + coord.r),
        (SHORT)(cOffset + coord.c),
        (SHORT)(rOffset + coord.r)
    };
    WriteConsoleOutput(hConsole, buffer, {1, 1}, {(SHORT)(cOffset + coord.c), (SHORT)(rOffset + coord.r)}, &s);*/
    setCursor(rOffset + coord.r, cOffset + coord.c);
    setColorCell(frames[currFrame][pos.r][pos.c]);
    printf("%c", computePrintVal(pos, coord));
}

// Updates the screen after a player moves
// Goes through reprint list and removes old positions
// If the screen does not need to be shifted, the new positions are drawn
// Otherwise, the screen is reprinted cell by cell
// If the movement of the screen does not change a coordinate's appearance on the screen, it is not reprinted
void updateScreen(int dir) {
    for (int i = 0; i < (int)redrawList.size(); i += reprint.reprint ? 2 : 1) {
        Pos pos = redrawList[i];
        if (pos.r >= screenPos.r && pos.r < screenPos.r + screenSize && pos.c >= screenPos.c && pos.c < screenPos.c + screenSize * 2) {
            Pos screenCoord = mapToScreen(pos);
            for (int i = 0; i < 2; i++)
                printCell({pos.r, pos.c + i}, {screenCoord.r, screenCoord.c + i});
        }
    }
    redrawList.clear();
    if (reprint.reprint) {
        screenPos.r += reprint.rChange;
        screenPos.c += reprint.cChange;
        for (int r = 0; r < screenSize; r++) {
            for (int c = 0; c < screenSize * 2; c++) {
                Pos pos = screenToMap({r, c});
                if (dir == 0 && pos.r + 1 < rows && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r + 1][pos.c]) {
                    continue;
                } else if (dir == 1 && pos.r > 0 && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r - 1][pos.c]) {
                    continue;
                } else if (dir == 2 && pos.c + 2 < cols && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r][pos.c + 2]) {
                    continue;
                } else if (dir == 3 && pos.c >= 2 && frames[currFrame][pos.r][pos.c] == frames[currFrame][pos.r][pos.c - 2]) {
                    continue;
                } else {
                    printCell(pos, {r, c});
                }
            }
        }
        reprint.reprint = false;
    }

    for (Projectile projectile : projectileList) {
        setCursor(rOffset + projectile.coord.r, cOffset + projectile.coord.c);
        setColor(0);
        printf("%c", projectile.ch);
        //Sleep(10);
    }
    if (!projectileList.empty())
        Sleep(250);
    for (Projectile projectile : projectileList)
        printCell(screenToMap(projectile.coord), projectile.coord);
    projectileList.clear();
}

// Prints the box outline of the screen
void printBox() {
    std::string str(screenSize * 2, '_');
    for (int r = 0; r < screenSize; r++)
        str += '\n' + std::string(cOffset - 1, ' ') + '|' + std::string(screenSize * 2, r == screenSize - 1 ? '_' : ' ') + '|';
    setCursor(rOffset - 1, cOffset);
    printf(str.c_str());
    reset();
}

// Prints a basic version of the map, types instead of values
void printMapBasic() {
    setColor(0);
    for (int r = 0; r < screenSize; r++) {
        setCursor(rOffset + r, cOffset);
        for (int c = 0; c < screenSize * 2; c++) {
            Pos pos = screenToMap({r, c});
            //char val = frames[currFrame][pos.r][pos.c].value;
            printf("%c", frames[currFrame][pos.r][pos.c].type == ' ' ? (r == screenSize - 1 ? '_' : ' ') : frames[currFrame][pos.r][pos.c].type);
        }
    }
}

// Clears the screen by printing ' ' across the screen
void clearScreen() {
    std::string str = "";
    for (int r = 0; r < rOffset + screenSize; r++)
        str += std::string(cOffset + screenSize * 2 + 1, ' ') + '\n';
    reset();
    printf(str.c_str());
}

// Runs animation for "loading" a screen somewhere in the window
void screenLoad(int width, int height, int rowOffset, int colOffset) {
    int middle = std::round((float)height / 2);
    for (int h = 0; h < middle; h++) {
        setCursor(rowOffset + middle - 2 - h, colOffset);
        printf(std::string(width * 2, '_').c_str());
        for (int r = middle - h - 1; r < middle + h; r++) {
            setCursor(rowOffset + r, colOffset - 1);
            printf("|%s|", std::string(width * 2, r == middle + h - 1 ? '_' : ' ').c_str());
        }
        Sleep(50);
    }
}

// Runs animation for "closing" a screen somewhere in the window
void screenClose(int width, int height, int rowOffset, int colOffset) {
    int middle = std::round((float)height / 2);
    for (int h = middle - 2; h >= 0; h--) {
        setCursor(rowOffset + middle - 3 - h, colOffset - 1);
        printf(std::string(width * 2 + 2, ' ').c_str());
        setCursor(rowOffset + middle - 2 - h, colOffset - 1);
        printf(" %s ", std::string(width * 2, '_').c_str());
        setCursor(rowOffset + middle + h - 1, colOffset - 1);
        printf("|%s|", std::string(width * 2, '_').c_str());
        setCursor(rowOffset + middle + h, colOffset - 1);
        printf(std::string(width * 2 + 2, ' ').c_str());
        Sleep(50);
    }
    setCursor(rowOffset + middle - 2, colOffset - 1);
    printf(std::string(width * 2 + 2, ' ').c_str());
    setCursor(rowOffset + middle - 1, colOffset - 1);
    printf(std::string(width * 2 + 2, ' ').c_str());
}

// Prints the menu located under the screen
// Calling with save == 1 adds the coordinates of each menu item to menuPos
void printMenu(int save) {
    const int usableScreen = (cOffset * 2) + (screenSize * 2) - (menuCOffset * 2);
    int gaps[MENU_SIZE];
    gaps[1] = std::round((usableScreen - ((MENU_SIZE + 1) * ((float)(menu[0].length() + menu[1].length()) / 2))) / (MENU_SIZE + 1));
    gaps[0] = std::round(gaps[1] + ((float)menu[1].length() / 2));

    //CONSOLE_SCREEN_BUFFER_INFO* info = (CONSOLE_SCREEN_BUFFER_INFO*)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    CONSOLE_SCREEN_BUFFER_INFO* info = new CONSOLE_SCREEN_BUFFER_INFO;
    
    setCursor(rOffset + screenSize + menuROffset, menuCOffset);
    setColor(0);
    for (int i = 0; i < MENU_SIZE; i++) {
        if (i > 1)
            gaps[i] = std::round(gaps[i - 1] + ((float)(menu[i - 2].length() - menu[i].length()) / 2));
        if (save) {
            GetConsoleScreenBufferInfo(hConsole, info);
            menuPos.push_back({info->dwCursorPosition.Y, info->dwCursorPosition.X + gaps[i]});
        }
        printf("%s%s", std::string(gaps[i], ' ').c_str(), menu[i].c_str());
    }
    reset();
}

// Sets the window to the desired width in the given number of seconds
void changeWindow(int width, int time) {
    time *= 0.9;
    int sleepTime = 0, increment = INT_MAX;
    for (int i = 20; i < std::abs(width - windowWidth); i++) {
        for (int j = 1; j < increment && i * j <= std::abs(width - windowWidth); j++) {
            if (((float)std::abs(width - windowWidth) / j) * i > time * 0.98 && ((float)std::abs(width - windowWidth) / j) * i < time * 1.02) {
                sleepTime = i;
                increment = j;
            }
        }
    }
    if (sleepTime == 0 && increment == INT_MAX)
        return;
    DWORD style = GetWindowLong(console, GWL_STYLE);
    style |= WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
    if (width > windowWidth) {
        while (windowWidth <= width) {
            windowWidth += increment;
            SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
            removeScrollbar();
            Sleep(sleepTime);
        }
    } else {
        while (windowWidth >= width) {
            windowWidth -= increment;
            SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
            removeScrollbar();
            Sleep(sleepTime);
        }
    }
    windowWidth = width;
    SetWindowPos(console, NULL, r.left, r.top, windowWidth, windowHeight, SWP_NOMOVE|SWP_FRAMECHANGED|SWP_NOZORDER);
    removeScrollbar();
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
}

// Prints the box for a conversation
void printConvoBox() {
    setCursor(conversationROffset - 1, conversationCOffset);
    printf(std::string(convoSize, '_').c_str());
    for (int r = 0; r < screenSize; r++) {
        setCursor(conversationROffset + r, conversationCOffset - 1);
        printf("|%s|", std::string(convoSize, r == screenSize - 1 ? '_' : ' ').c_str());
    }
    reset();
}

/*void clearConvoBox() {
    for (int r = -1; r < screenSize; r++) {
        setCursor(r + conversationROffset, conversationCOffset - 1);
        printf(std::string(convoSize * 2 + 2, ' ').c_str());
    }
    reset();
}*/

// Loads given dialogue file, expands window, and displays conversation
void conversation(std::string dialogue) {
    changeWindow(890, 700);
    Sleep(500);
    std::ifstream d("Dialogues/" + dialogue);
	currentDialogue = std::string((std::istreambuf_iterator<char>(d)), std::istreambuf_iterator<char>());
    std::vector<std::string> lines;
    size_t pos = 0;
    while ((pos = currentDialogue.find('\n')) != std::string::npos) {
        std::string str = currentDialogue.substr(0, pos);
        lines.push_back(str);
        currentDialogue.erase(0, pos + 1);
    }
    lines.push_back(currentDialogue);

    screenLoad(convoSize, screenSize, conversationROffset, conversationCOffset);
    Sleep(500);

    for (int i = 0, r = 0; i < (int)lines.size(); i++) {
        pos = lines[i].find(":");
        //int speaker = std::stoi(lines[i].substr(0, pos));
        std::string speaker = lines[i].substr(0, pos);
        setCursor(conversationROffset + 2 * i + r + 1, conversationCOffset + 2);
        //setColor(speaker);
        //printf("  ");
        Pos npcPos = {-1, -1};
        if (speaker == "+++") {
            npcPos = *playerPos;
        } else {
            for (npcID npc : npcIDs) {
                if (npc.id == speaker)
                    npcPos = {npc.r, npc.c};
            }
        }
        setColorCell(frames[currFrame][npcPos.r][npcPos.c]);
        for (int i = 0; i < 2; i++)
            printf("%c", frames[currFrame][npcPos.r][npcPos.c].value);
        setColor(0);
        printf(" :");
        Sleep(500);
        std::string str = lines[i].substr(pos + 1);
        std::vector<std::string> strs;
        while (str.length() > maxCharsConvo) {
            strs.push_back(str.substr(0, maxCharsConvo));
            str.erase(0, maxCharsConvo);
        }
        strs.push_back(str);
        for (int j = 0; j < (int)strs.size(); j++) {
            r += (j ? 1 : 0);
            setCursor(conversationROffset + 2 * i + r + 1, conversationCOffset + 7);
            printConversationText(strs[j]);
        }
        Sleep(500);
    }
    Sleep(1000);
    screenClose(convoSize, screenSize, conversationROffset, conversationCOffset);
    changeWindow(ORIGINAL_WINDOW_WIDTH, 700);
}

// Prints a line of conversation dialogue character by character
void printConversationText(std::string line) {
    for (char c : line) {
        printf("%c", c);
        Sleep(20);
    }
}