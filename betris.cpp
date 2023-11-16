#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>

#include "config.hpp"
#include "pieces.hpp"

using namespace std;

struct GAME {
   	char table[HEIGHT][WIDTH];	
	int score, lines, nextPiece;
};

void draw_borders(WINDOW *screen) { 
    int x, y, i;
    getmaxyx(screen, y, x); // 4 corners 
    mvwprintw(screen, 0, 0, "+");
    mvwprintw(screen, y - 1, 0, "+");
    mvwprintw(screen, 0, x - 1, "+");
    mvwprintw(screen, y - 1, x - 1, "+"); // sides
    for (i = 1; i < (y - 1); i++) {
        mvwprintw(screen, i, 0, "|");
        mvwprintw(screen, i, x - 1, "|"); 
    } // top and bottom 
    for (i = 1; i < (x - 1); i++) {
        mvwprintw(screen, 0, i, "-");
        mvwprintw(screen, y - 1, i, "-");
    } 
}

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *mainGameWindow;              
                                
    mainGameWindow = newwin(height, width, starty, startx);
              
    wrefresh(mainGameWindow);
                                                           
    return mainGameWindow;
}                             
                                      
void putPiece(struct GAME &game, struct PIECE &movingPiece, char color) {
    for (int i = 0; i < SIZE_PIECES; i++) {
        game.table[movingPiece.form[i][0]][movingPiece.form[i][1]] = color;
    }
}

void printGame(WINDOW *mainGameWindow, struct GAME &game, struct PIECE &movingPiece) {
    draw_borders(mainGameWindow);
    for (int i = 0; i < HEIGHT; i++) {
        wmove(mainGameWindow, i + 1, 1);
        for (int j = 0; j < WIDTH; j++) {
            wattron(mainGameWindow, COLOR_PAIR(game.table[i][j]));
            waddch(mainGameWindow, ' ');
            waddch(mainGameWindow, ' ');
            wattroff(mainGameWindow, COLOR_PAIR(game.table[i][j]));
        }
    }
    wrefresh(mainGameWindow);
}

void printNextPiece(WINDOW *nextPieceWindow, struct GAME &game) {
    wclear(nextPieceWindow);
    draw_borders(nextPieceWindow);
    wattron(nextPieceWindow, COLOR_PAIR(PIECES[game.nextPiece].color));

    for (int i = 0; i < SIZE_PIECES; i++) {
        wmove(nextPieceWindow, PIECES[game.nextPiece].form[i][0] + 2,
              (PIECES[game.nextPiece].form[i][1]  * 2) + 2);
        waddch(nextPieceWindow, ' ');
        waddch(nextPieceWindow, ' ');
    }

    wattroff(nextPieceWindow, COLOR_PAIR(PIECES[game.nextPiece].color));
    wrefresh(nextPieceWindow);
}

void printScore(WINDOW *scoreWindow, struct GAME &game) {
    char scoreLabel[] = "POINTS: ";
    char score[6];
    sprintf(score, "%d", game.score);
    char linesLabel[] = "LINES: ";
    char lines[3]; 
    sprintf(lines, "%d", game.lines);
    wclear(scoreWindow);
    draw_borders(scoreWindow);
    // Puntos
    wmove(scoreWindow, 1, 1); 
    wprintw(scoreWindow, scoreLabel);
    wmove(scoreWindow, 2, 1); 
    wprintw(scoreWindow, score);

    // Líneas
    wmove(scoreWindow, 4, 1); 
    wprintw(scoreWindow, linesLabel);
    wmove(scoreWindow, 5, 1); 
    wprintw(scoreWindow, lines);

    wrefresh(scoreWindow);
}

void initTable(struct GAME &game) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            game.table[i][j] = NEGRO;
        }
    }
}

void clearLine(struct GAME &game, int line) {
    // Copia de abajo a arriba
    for (int i = line; i > 0; i--) {
        for (int j = 0; j < WIDTH; j++) {
            game.table[i][j] = game.table[i-1][j];
        }
    }
    // Borra la primera línea
    for (int i = 0; i < WIDTH; i++) {
        game.table[0][i] = NEGRO;
    }
}

void checkLine(struct GAME &game, const int lowestLine) {
    int end = lowestLine - 3;
    int scoreAdded = 0;
    if (end < 0) {
        end = 0;
    }
    for (int i = lowestLine; i >= end;) {
        bool isFull = true;
        for (int j = 0; j < WIDTH && isFull; j++) {
            if (game.table[i][j] == NEGRO) {
                isFull = false;
            }
        }
        if (isFull) {
            // No tengo ni idea de cómo funciona el sistema de puntuación del tetris
            if (scoreAdded == 0) {
                scoreAdded = 10;
            }
            else {
                scoreAdded = scoreAdded * 10;
            }
            clearLine(game, i);
            game.lines++;
        }
        else {
            i--;
        }
    }

    game.score += scoreAdded;
    if (game.score > 999999) {
        game.score = 999999;
    }
}

void gravity(struct PIECE &movingPiece) {
    for (int i = 0; i < SIZE_PIECES; i++) {
        movingPiece.form[i][0]++;
    }
}


bool pieceFits(struct GAME &game, struct PIECE &movingPiece) {
    bool fits = true;
    for (int i = 0; i < SIZE_PIECES && fits; i++) {
        if (movingPiece.form[i][0] < 0 || movingPiece.form[i][0] >= HEIGHT ||
            movingPiece.form[i][1] < 0 || movingPiece.form[i][1] >= WIDTH) {
            fits = false;
        }
        else if (game.table[movingPiece.form[i][0]][movingPiece.form[i][1]] != NEGRO) {
            fits = false;
        }
    }
    return fits;
}

void rotatePiece(struct GAME &game, struct PIECE &movingPiece, int dir) {
    // La pieza amarilla no rota
    if (movingPiece.color != AMARILLO) {
        struct PIECE tempPiece = movingPiece;
        // La pieza azul rota de forma distinta
        if (movingPiece.color == CIAN) {
            // Esto es muy feo, detecto si la pieza está tumbada
            if (movingPiece.form[0][0] == movingPiece.form[1][0]) {
                for (int i = 1; i < SIZE_PIECES; i++) {
                    tempPiece.form[i][1] = movingPiece.form[0][1];
                }
                tempPiece.form[1][0] = movingPiece.form[1][0] - 1;
                tempPiece.form[2][0] = movingPiece.form[1][0] + 1;
                tempPiece.form[3][0] = movingPiece.form[1][0] + 2;
            }
            else {
                for (int i = 1; i < SIZE_PIECES; i++) {
                    tempPiece.form[i][0] = movingPiece.form[0][0];
                }
                tempPiece.form[1][1] = movingPiece.form[1][1] - 1;
                tempPiece.form[2][1] = movingPiece.form[1][1] + 1;
                tempPiece.form[3][1] = movingPiece.form[1][1] + 2;
            }
        }
        else {
            // Rotate left
            //       (0  1)
            // (x y) (-1 0)
            // or
            // Rotate right
            //       (0 -1)
            // (x y) (1  0)
            for (int i = 1; i < SIZE_PIECES; i++) {
                tempPiece.form[i][0] = movingPiece.form[0][0]
                                       + (movingPiece.form[i][1] - movingPiece.form[0][1]) * dir;
                tempPiece.form[i][1] = movingPiece.form[0][1]
                                       + (movingPiece.form[i][0] - movingPiece.form[0][0]) * -dir;
                                      
            }
        }
        if (pieceFits(game, tempPiece)) {
            movingPiece = tempPiece;
        }
    }
}

void movePiece(struct GAME &game, struct PIECE &movingPiece, int dir) {
    struct PIECE tempPiece = movingPiece;
    for (int i = 0; i < SIZE_PIECES; i++) {
        tempPiece.form[i][1] = movingPiece.form[i][1] + dir;
    }
    if (pieceFits(game, tempPiece)) {
        movingPiece = tempPiece;
    }
}

int detectLowestLine(struct PIECE &movingPiece) {
    int max = 0;
    for (int i = 1; i < SIZE_PIECES; i++) {
        if (movingPiece.form[i][0] > movingPiece.form[max][0]) {
            max = i;
        }
    }
    return movingPiece.form[max][0];
}

bool checkIfLanded(struct GAME &game, struct PIECE &movingPiece) {
    struct PIECE tempPiece = movingPiece;
    for (int i = 0; i < SIZE_PIECES; i++) {
        tempPiece.form[i][0]++;
    }
    return !pieceFits(game, tempPiece);
}

void generateNextPiece(struct GAME &game, struct PIECE &movingPiece) {
    movingPiece = PIECES[game.nextPiece];
    // Muevo la pieza al medio
    for (int i = 0; i < SIZE_PIECES; i++) {
        movingPiece.form[i][1] += (MID - 1);
    }
    for (int i = 0; i < SIZE_PIECES; i++) {
        movingPiece.form[i][0]++;
    }
    game.nextPiece = rand() % NUM_PIECES;
}

int main() {
    srand(time(NULL));
    char c;
    
    initscr();

    // COLORS !!!
    start_color();
    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_BLUE);
    init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(6, COLOR_BLACK, COLOR_CYAN);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);

    // GAME
    struct GAME gameTable;
    gameTable.score = 0;
    gameTable.lines = 0;

    timeout(INPUT_DELAY);
    noecho();
    curs_set(0);

    // Windows
    WINDOW *mainGameWindow, *nextPieceWindow, *scoreWindow;
    //
    // Main Window
    int mainWindowHeight = HEIGHT + 2;
    int mainWindowWidth = WIDTH*2 + 2;

    // Lo declaro aquí porque me sirve para centrar la ventana

    int nextPieceWindowWidth = SIZE_PIECES*2 + 4;
    
    int mainWindowStartY = (LINES - mainWindowHeight) / 2;
    int mainWindowStartX = (COLS - mainWindowWidth - nextPieceWindowWidth) / 2;

    mainGameWindow = create_newwin(mainWindowHeight, mainWindowWidth,
                                   mainWindowStartY, mainWindowStartX);
    //
    // Next piece window
    int nextPieceWindowHeight = 6;
    
    int nextPieceWindowStartY = mainWindowStartY;
    int nextPieceWindowStartX = mainWindowStartX + mainWindowWidth;

    nextPieceWindow = create_newwin(nextPieceWindowHeight, nextPieceWindowWidth,
                                   nextPieceWindowStartY, nextPieceWindowStartX);
    // Score and lines window
    int scoreWindowHeight = mainWindowHeight - nextPieceWindowHeight;
    int scoreWindowWidth = SIZE_PIECES*2 + 4;
    int scoreWindowStartY = mainWindowStartY + nextPieceWindowHeight;
    int scoreWindowStartX = mainWindowStartX + mainWindowWidth;
    scoreWindow = create_newwin(scoreWindowHeight, scoreWindowWidth,
                                   scoreWindowStartY, scoreWindowStartX);
                   
    initTable(gameTable);
    bool endGame = false;
    bool goDown = false;
    bool isPaused = false;

    c = getch();

    struct PIECE movingPiece;
    gameTable.nextPiece = rand() % NUM_PIECES;
    generateNextPiece(gameTable, movingPiece);
    printScore(scoreWindow, gameTable);
    printNextPiece(nextPieceWindow, gameTable);
    time_t gravityTimer;
    time(&gravityTimer);

    do {
        c = getch();
        // Capital
        c &= ~(1UL << 5);
        // Borro la posición actual de la pieza
        putPiece(gameTable, movingPiece, NEGRO);

        // Gravity
        if ((difftime(time(NULL), gravityTimer) >= SPEED) || goDown) {
            goDown = false;
            time(&gravityTimer);
            if (checkIfLanded(gameTable, movingPiece)) {
                putPiece(gameTable, movingPiece, movingPiece.color);
                checkLine(gameTable, detectLowestLine(movingPiece));
                printScore(scoreWindow, gameTable);
                generateNextPiece(gameTable, movingPiece);
                printNextPiece(nextPieceWindow, gameTable);
                if (!pieceFits(gameTable, movingPiece)) {
                    endGame = true;
                }
            }
            else {
                gravity(movingPiece);
            }
        }

        // Input
        switch(c) {
            case MOVER_ABAJO:
                goDown = true;
                break;
            case MOVER_IZQUIERDA:
                movePiece(gameTable, movingPiece, -1);
                break;
            case MOVER_DERECHA:
                movePiece(gameTable, movingPiece, 1);
                break;
            case ROTAR_DERECHA:
                rotatePiece(gameTable, movingPiece, -1);
                break;
            case ROTAR_IZQUIERDA:
                rotatePiece(gameTable, movingPiece, 1);
                break;
            case EMPEZAR:
                isPaused = true;
                // Para que no se use la pausa como ayuda, la pieza en movimiento no se muestra
                printGame(mainGameWindow, gameTable, movingPiece);
                break;
        }

        while (isPaused) {
            c = getch();
            // Capital
            c &= ~(1UL << 5);
            if (c == EMPEZAR) {
                isPaused = false;
            }
        }

        // Pongo de nuevo la pieza e imprimo el tablero
        putPiece(gameTable, movingPiece, movingPiece.color);
        //
        printGame(mainGameWindow, gameTable, movingPiece);
	} while (c != SALIR && endGame != true);

    endwin();

    return 0;
}

