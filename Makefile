CC=gcc
LN=ld
CFLAGS=-Wall -std=c99
FILES=SimpleServer.c SimpleHTTPLib.o
HEADERS=SimpleHTTPLib/SimpleHTTPLib.h SimpleHTTPLib/SysSocketFramework.h SimpleHTTPLib/hashtab.h SimpleHTTPLib/tree.h SimpleHTTPLib/type.h
SOURCES=SimpleHTTPLib/HTTPLib.c SimpleHTTPLib/SysSocketFramework.c SimpleHTTPLib/hashtab.c SimpleHTTPLib/tree.c SimpleHTTPLib/type.c
OBJECTS=HTTPLib.o SysSocketFramework.o hashtab.o tree.o type.o

default: buildLib linkingLib buildServer clean runServer

win: buildLib linkingLib forwindows clean runServer

buildLib: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

linkingLib: $(OBJECTS)
	$(LN) -r $(OBJECTS) -o SimpleHTTPLib.o

clean:
	rm -f *.o *.i *.s

buildServer: $(FILES)
	$(CC) $(FILES) -o SimpleServer

forwindows:
	gcc SimpleServer.c SimpleHTTPLib.o -o SimpleServer.exe -lws2_32

runServer:
	./SimpleServer