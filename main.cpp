#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <string>
using namespace std;

#define H 20
#define W 15

// ===================== Global =====================
char board[H][W];

int x = 5, y = 0;
int speed = 200;
int score = 0;
int totalLines = 0;
int level = 1;
bool running = true;

// ===================== Console helpers =====================
void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setTextColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void hideCursor(bool hide = true) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(h, &info);
    info.bVisible = !hide;
    SetConsoleCursorInfo(h, &info);
}

void setConsoleTitle() {
    SetConsoleTitleA("TETRIS - Console Edition");
}

void clearScreen() {
    system("cls");
}

void drawBox(int left, int top, int width, int height) {
    for (int i = 0; i < width; i++) {
        gotoxy(left + i, top);              cout << char(205);
        gotoxy(left + i, top + height - 1); cout << char(205);
    }
    for (int i = 0; i < height; i++) {
        gotoxy(left, top + i);              cout << char(186);
        gotoxy(left + width - 1, top + i);  cout << char(186);
    }
    gotoxy(left, top);                     cout << char(201);
    gotoxy(left + width - 1, top);         cout << char(187);
    gotoxy(left, top + height - 1);        cout << char(200);
    gotoxy(left + width - 1, top + height - 1); cout << char(188);
}

// ===================== Board =====================
void initBoard() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if (i == H - 1 || j == 0 || j == W - 1) board[i][j] = '#';
            else board[i][j] = ' ';
}

int originX = 0, originY = 0;
const int HUD_MARGIN = 6;     
const int HUD_WIDTH  = 26;    

void updateLayout()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    int consoleW = csbi.srWindow.Right  - csbi.srWindow.Left + 1;
    int consoleH = csbi.srWindow.Bottom - csbi.srWindow.Top  + 1;

    int boardWChars = W * 2;                  
    int totalW = boardWChars + HUD_MARGIN + HUD_WIDTH;
    int totalH = H;                           

    originX = (consoleW - totalW) / 2;
    originY = (consoleH - totalH) / 2;

    if (originX < 0) originX = 0;
    if (originY < 0) originY = 0;
}

void draw() {
    updateLayout();

    for (int i = 0; i < H; i++) {
        gotoxy(originX, originY + i);
        for (int j = 0; j < W; j++) {
            if (board[i][j] == '#') cout << "##";
            else if (board[i][j] == ' ') cout << "  ";
            else cout << "[]";
        }
    }
}


void drawHUD() {
    int hudX = originX + (W * 2) + HUD_MARGIN;
    int hudY = originY;

    gotoxy(hudX, hudY + 1);  cout << "SCORE : " << score << "    ";
    gotoxy(hudX, hudY + 2);  cout << "LINES : " << totalLines << "    ";
    gotoxy(hudX, hudY + 3);  cout << "LEVEL : " << level << "    ";

    gotoxy(hudX, hudY + 5);  cout << "A/D : Move       ";
    gotoxy(hudX, hudY + 6);  cout << "W   : Rotate     ";
    gotoxy(hudX, hudY + 7);  cout << "X   : Soft drop  ";
    gotoxy(hudX, hudY + 8);  cout << "Q   : Quit       ";
}


int removeLine() {
    int cleared = 0;

    for (int i = H - 2; i > 0; i--) {
        int j;
        for (j = 1; j < W - 1; j++)
            if (board[i][j] == ' ') break;

        if (j == W - 1) {
            cleared++;

            for (int ii = i; ii > 1; ii--)
                for (int jj = 1; jj < W - 1; jj++)
                    board[ii][jj] = board[ii - 1][jj];

            for (int jj = 1; jj < W - 1; jj++)
                board[1][jj] = ' ';

            i++;

            if (speed > 60) speed -= 10;
            draw();
            Sleep(200);
        }
    }
    return cleared;
}

// ===================== OOP Pieces =====================
class Piece {
protected:
    char shape[4][4];

public:
    virtual ~Piece() {}

    const char (*getShape() const)[4] { return shape; }

    virtual void rotateCW() {
        char temp[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                temp[i][j] = shape[3 - j][i];

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = temp[i][j];
    }

    virtual void rotateCCW() {
        rotateCW(); rotateCW(); rotateCW();
    }

    void copyShapeTo(char out[4][4]) const {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                out[i][j] = shape[i][j];
    }

    void loadFrom(const char in[4][4]) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = in[i][j];
    }

    virtual char tileChar() const = 0;
};

class IPiece : public Piece {
public:
    IPiece() {
        char s[4][4] = {
            {' ','I',' ',' '},
            {' ','I',' ',' '},
            {' ','I',' ',' '},
            {' ','I',' ',' '}
        };
        loadFrom(s);
    }
    char tileChar() const override { return 'I'; }
};

class OPiece : public Piece {
public:
    OPiece() {
        char s[4][4] = {
            {' ',' ',' ',' '},
            {' ','O','O',' '},
            {' ','O','O',' '},
            {' ',' ',' ',' '}
        };
        loadFrom(s);
    }
    void rotateCW() override {}
    void rotateCCW() override {}
    char tileChar() const override { return 'O'; }
};

class TPiece : public Piece {
public:
    TPiece() {
        char s[4][4] = {
            {' ',' ',' ',' '},
            {' ','T',' ',' '},
            {'T','T','T',' '},
            {' ',' ',' ',' '}
        };
        loadFrom(s);
    }
    char tileChar() const override { return 'T'; }
};

class SPiece : public Piece {
public:
    SPiece() {
        char s[4][4] = {
            {' ',' ',' ',' '},
            {' ','S','S',' '},
            {'S','S',' ',' '},
            {' ',' ',' ',' '}
        };
        loadFrom(s);
    }
    char tileChar() const override { return 'S'; }
};

class ZPiece : public Piece {
public:
    ZPiece() {
        char s[4][4] = {
            {' ',' ',' ',' '},
            {'Z','Z',' ',' '},
            {' ','Z','Z',' '},
            {' ',' ',' ',' '}
        };
        loadFrom(s);
    }
    char tileChar() const override { return 'Z'; }
};

class JPiece : public Piece {
public:
    JPiece() {
        char s[4][4] = {
            {' ',' ',' ',' '},
            {'J',' ',' ',' '},
            {'J','J','J',' '},
            {' ',' ',' ',' '}
        };
        loadFrom(s);
    }
    char tileChar() const override { return 'J'; }
};

class LPiece : public Piece {
public:
    LPiece() {
        char s[4][4] = {
            {' ',' ',' ',' '},
            {' ',' ','L',' '},
            {'L','L','L',' '},
            {' ',' ',' ',' '}
        };
        loadFrom(s);
    }
    char tileChar() const override { return 'L'; }
};

// ===================== Game helpers =====================
Piece* cur = nullptr;

bool canPlaceAt(const Piece* p, int nx, int ny) {
    const char (*s)[4] = p->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ') {
                int tx = nx + j;
                int ty = ny + i;
                if (tx < 1 || tx >= W - 1 || ty < 0 || ty >= H - 1) return false;
                if (board[ty][tx] != ' ') return false;
            }
    return true;
}

void delPieceFromBoard() {
    const char (*s)[4] = cur->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ') {
                int ty = y + i;
                int tx = x + j;
                if (ty >= 0 && ty < H && tx >= 0 && tx < W)
                    board[ty][tx] = ' ';
            }
}

void putPieceToBoard() {
    const char (*s)[4] = cur->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ')
                board[y + i][x + j] = s[i][j];
}

bool canMove(int dx, int dy) {
    return canPlaceAt(cur, x + dx, y + dy);
}

void rotateWithKickCW() {
    char backup[4][4];
    cur->copyShapeTo(backup);

    cur->rotateCW();

    int kicks[] = {0, -1, 1, -2, 2};
    for (int k = 0; k < 5; k++) {
        int nx = x + kicks[k];
        int ny = y;
        if (canPlaceAt(cur, nx, ny)) {
            x = nx;
            return;
        }
    }
    cur->loadFrom(backup);
}

Piece* randomPiece() {
    int r = rand() % 7;
    switch (r) {
        case 0: return new IPiece();
        case 1: return new OPiece();
        case 2: return new TPiece();
        case 3: return new SPiece();
        case 4: return new ZPiece();
        case 5: return new JPiece();
        default:return new LPiece();
    }
}

// ===================== Menu UI =====================
void printLogo(int left, int top) {
    setTextColor(11);
    gotoxy(left, top + 0); cout << " _______ _______ _______ _______ _______ ";
    gotoxy(left, top + 1); cout << "|__   __|__   __|__   __|__   __|__   __|";
    gotoxy(left, top + 2); cout << "   | |     | |     | |     | |     | |   ";
    gotoxy(left, top + 3); cout << "   | |     | |     | |     | |     | |   ";
    gotoxy(left, top + 4); cout << "   |_|     |_|     |_|     |_|     |_|   ";
    gotoxy(left, top + 5); cout << "           T E T R I S   (Console)       ";
    setTextColor(7);
}


void printCentered(int y, const string& text, int width) {
    int x = (width - (int)text.size()) / 2;
    if (x < 0) x = 0;
    gotoxy(x, y);
    cout << text;
}

void drawMenu(int selected) {
    clearScreen();
    hideCursor(true);

    int consoleWidth = 90;
    int boxLeft = 8, boxTop = 2, boxW = 74, boxH = 22;

    drawBox(boxLeft, boxTop, boxW, boxH);

    printLogo(boxLeft + 8, boxTop + 2);

    gotoxy(boxLeft + 3, boxTop + 10);
    cout << "Console Edition  |  Controls: A/D/W/X, Q to quit";

    string items[] = {"Start Game", "How to Play", "Exit"};
    int n = 3;

    int menuX = boxLeft + 26;
    int menuY = boxTop + 13;

    for (int i = 0; i < n; i++) {
        gotoxy(menuX, menuY + i * 2);
        if (i == selected) {
            setTextColor(240); 
            cout << "  " << items[i] << "  ";
            setTextColor(7);
        } else {
            cout << "  " << items[i] << "  ";
        }
    }

    gotoxy(boxLeft + 3, boxTop + boxH - 2);
    cout << "Use UP/DOWN or W/S to navigate, ENTER to select, ESC to quit.";
}

void showHowToPlay() {
    clearScreen();
    hideCursor(true);

    int boxLeft = 8, boxTop = 2, boxW = 74, boxH = 22;
    drawBox(boxLeft, boxTop, boxW, boxH);

    setTextColor(11);
    gotoxy(boxLeft + 3, boxTop + 2);
    cout << "HOW TO PLAY";
    setTextColor(7);

    gotoxy(boxLeft + 3, boxTop + 5);
    cout << "- A / D : Move left / right";
    gotoxy(boxLeft + 3, boxTop + 7);
    cout << "- W     : Rotate clockwise (with simple wall-kick)";
    gotoxy(boxLeft + 3, boxTop + 9);
    cout << "- X     : Soft drop (move down faster)";
    gotoxy(boxLeft + 3, boxTop + 11);
    cout << "- Q     : Quit game";

    gotoxy(boxLeft + 3, boxTop + 14);
    cout << "Scoring:";
    gotoxy(boxLeft + 5, boxTop + 16);
    cout << "1 line  = 100 * level";
    gotoxy(boxLeft + 5, boxTop + 17);
    cout << "2 lines = 300 * level";
    gotoxy(boxLeft + 5, boxTop + 18);
    cout << "3 lines = 500 * level";
    gotoxy(boxLeft + 5, boxTop + 19);
    cout << "4 lines = 800 * level";

    gotoxy(boxLeft + 3, boxTop + boxH - 2);
    cout << "Press any key to go back...";
    getch();
}

int runStartMenu() {
    int selected = 0;

    while (true) {
        drawMenu(selected);

        int key = getch();
        if (key == 27) return 2; // ESC -> Exit

        if (key == 224) {
            int k2 = getch();
            if (k2 == 72) { 
                selected = (selected - 1 + 3) % 3;
            } else if (k2 == 80) { // DOWN
                selected = (selected + 1) % 3;
            }
        } else {
            if (key == 'w' || key == 'W') selected = (selected - 1 + 3) % 3;
            else if (key == 's' || key == 'S') selected = (selected + 1) % 3;
            else if (key == 13) { // ENTER
                return selected;
            }
        }
    }
}

// ===================== Game loop (wrapped) =====================
void resetGameState() {
    x = 5; y = 0;
    speed = 200;
    score = 0;
    totalLines = 0;
    level = 1;
    initBoard();

    if (cur) { delete cur; cur = nullptr; }
    cur = randomPiece();
}

void gameOverScreen() {
    int hudX = W * 2 + 3;
    gotoxy(hudX, 11); cout << "====================";
    gotoxy(hudX, 12); cout << "      GAME OVER     ";
    gotoxy(hudX, 13); cout << "====================";
    gotoxy(hudX, 15); cout << "Press any key...";
    getch();
}

void runGame() {
    clearScreen();
    resetGameState();

    while (true) {
        delPieceFromBoard();

        if (kbhit()) {
            char c = getch();
            if ((c == 'a' || c == 'A') && canMove(-1, 0)) x--;
            else if ((c == 'd' || c == 'D') && canMove(1, 0)) x++;
            else if ((c == 'x' || c == 'X') && canMove(0, 1)) y++;
            else if (c == 'w' || c == 'W') rotateWithKickCW();
            else if (c == 'q' || c == 'Q') {
                gotoxy(0, H + 1);
                break;
            }
        }

        if (canMove(0, 1)) {
            y++;
        } else {
            putPieceToBoard();

            int lines = removeLine();
            if (lines > 0) {
                totalLines += lines;

                if (lines == 1) score += 100 * level;
                else if (lines == 2) score += 300 * level;
                else if (lines == 3) score += 500 * level;
                else score += 800 * level;

                level = totalLines / 10 + 1;
            }

            delete cur;
            cur = randomPiece();
            x = 5; y = 0;

            if (!canPlaceAt(cur, x, y)) {
                draw();
                drawHUD();
                gameOverScreen();
                break;
            }
        }

        putPieceToBoard();
        draw();
        drawHUD();
        Sleep(speed);
    }
}

// ===================== main =====================
int main() {
    srand((unsigned)time(nullptr));
    setConsoleTitle();
    hideCursor(true);

    while (true) {
        int choice = runStartMenu();

        if (choice == 0) {
            runGame();
        } else if (choice == 1) {
            showHowToPlay();
        } else {
            clearScreen();
            gotoxy(0, 0);
            break;
        }
    }

    if (cur) delete cur;
    return 0;
}

