const int NUM_PIECES = 7;
const int SIZE_PIECES = 4;

struct PIECE {
    char color;
    char form[SIZE_PIECES][2];
};

struct PIECE PIECES[NUM_PIECES] {
    // La primera posición corresponde al centro de rotación
    // Mid corresponde al centro del tablero, para cumplir las mecánicas del tetris
    {CIAN,     {{0,1}, {0,0}, {0,2}, {0,3}}},                     // 0  XXXX
                                                                  //
    {AZUL,     {{0,1}, {0,0}, {0,2}, {1,2}}},                     // 1  XOX
                                                                  //      X
                                                                  //
    {AMARILLO, {{0,0}, {0,1}, {1,0}, {1,1}}},                     // 2  XX
                                                                  //    XX
                                                                  //
    {MAGENTA,  {{0,1}, {0,0}, {0,2}, {1,0}}},                      // 3  XOX
                                                                  //    X 
    {BLANCO,   {{1,1}, {0,1}, {1,0}, {1,2}}},                     // 4   X
                                                                  //    XOX
                                                                  //
    {ROJO,     {{1,1}, {0,0}, {0,1}, {1,2}}},                     // 5  XX
                                                                  //     OX
                                                                  //
    {VERDE,    {{1,1}, {0,1}, {0,2}, {1,0}}}                      // 6   XX
                                                                  //    XO 
};

