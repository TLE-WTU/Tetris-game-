#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>
using namespace std;

#define H 20
#define W 15

char board[H][W];

int x = 5, y = 0;
int speed = 200;
int score = 0;
int totalLines = 0;
int level = 1;


void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void initBoard() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if (i == H - 1 || j == 0 || j == W - 1) board[i][j] = '#';
            else board[i][j] = ' ';
}

void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (board[i][j] == '#') cout << "##";
            else if (board[i][j] == ' ') cout << "  ";
            else cout << "[]";
        }
        cout << "\n";
    }
}

void drawHUD() {
    int hudX = W * 2 + 3; 
    gotoxy(hudX, 1);  cout << "SCORE : " << score << "    ";
    gotoxy(hudX, 2);  cout << "LINES : " << totalLines << "    ";
    gotoxy(hudX, 3);  cout << "LEVEL : " << level << "    ";
    gotoxy(hudX, 5);  cout << "A/D : Move";
    gotoxy(hudX, 6);  cout << "W   : Rotate";
    gotoxy(hudX, 7);  cout << "X   : Soft drop";
    gotoxy(hudX, 8);  cout << "Q   : Quit";
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

// ===================== OOP Piece + Polymorphism =====================

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

// ===================== Game helpers working with Piece* =====================

Piece* cur = nullptr;

bool canPlaceAt(const Piece* p, int nx, int ny) 
{
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

void delPieceFromBoard() 
{
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

void putPieceToBoard() 
{
    const char (*s)[4] = cur->getShape();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (s[i][j] != ' ')
                board[y + i][x + j] = s[i][j];
}

bool canMove(int dx, int dy) {
    return canPlaceAt(cur, x + dx, y + dy);
}


void rotateWithKickCW() 
{
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

Piece* randomPiece() 
{
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

// ===================== main =====================

int main() {
    srand((unsigned)time(nullptr));
    system("cls");
    initBoard();

    cur = randomPiece();

    while (1) {
        delPieceFromBoard();

        if (kbhit()) {
            char c = getch();
            if (c == 'a' && canMove(-1, 0)) x--;
            else if (c == 'd' && canMove(1, 0)) x++;
            else if (c == 'x' && canMove(0, 1)) y++;
            else if (c == 'w') rotateWithKickCW();
            else if (c == 'q')
            {
                gotoxy(0, H + 1);
                break;
            }
        }

        if (canMove(0, 1)) {
            y++;
        } else {
            putPieceToBoard();
            int lines = removeLine();    
            if (lines > 0){
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
                gotoxy(0, H + 1);
                cout << "GAME OVER!\n";
                break;
            }
        }

        putPieceToBoard();
        draw();
        drawHUD();
        Sleep(speed);

    }
    delete cur;
    return 0;
}
