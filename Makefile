CC = g++
CPPFLAGS = -std=c++11
LDFLAGS = -lncurses                     
RM = rm -f                    

all: 
	${CC} ntetris.cpp -o ntetris ${LDFLAGS}

clean:
	${RM} ntetris

