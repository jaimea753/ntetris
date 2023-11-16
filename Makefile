CC = g++
CPPFLAGS = -std=c++11
LDFLAGS = -lncurses                     
RM = rm -f                    

all: 
	${CC} betris.cpp -o betris ${LDFLAGS}

clean:
	${RM} betris

