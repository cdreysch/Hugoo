# name of the exec
EXEC=Hugoo

# build directory
BDIR=./

# source directory
SDIR=./src/

# compiler
CC=g++

# compiler flags
CFLAGS=-c -Wall -std=c++11 

# sfml libraries
SFML=-lsfml-graphics -lsfml-window -lsfml-system

all: hugoo 

hugoo: main.o myHuman.o myItem.o mySequence.o mySpritesheet.o mySprite.o mySpriteManager.o myAnimatedSprite.o myTileMap.o
	$(CC) -o $(BDIR)$(EXEC) main.o myHuman.o myItem.o mySequence.o mySpritesheet.o mySprite.o mySpriteManager.o myAnimatedSprite.o myTileMap.o $(SFML) 

main.o: 
	$(CC) $(CFLAGS) $(SDIR)main.cpp 

myHuman.o:
	$(CC) $(CFLAGS) $(SDIR)myHuman.cpp

myItem.o:
	$(CC) $(CFLAGS) $(SDIR)myItem.cpp

myTileMap.o:
	$(CC) $(CFLAGS) $(SDIR)myTileMap.cpp

myAnimatedSprite.o:
	$(CC) $(CFLAGS) $(SDIR)myAnimatedSprite.cpp

mySpritesheet.o:
	$(CC) $(CFLAGS) $(SDIR)mySpritesheet.cpp

mySequence.o:
	$(CC) $(CFLAGS) $(SDIR)mySequence.cpp

mySprite.o:
	$(CC) $(CFLAGS) $(SDIR)mySprite.cpp

mySpriteManager.o:
	$(CC) $(CFLAGS) $(SDIR)mySpriteManager.cpp

clean:
	rm *.o $(EXEC)

run:
	$(BDIR)$(EXEC)
