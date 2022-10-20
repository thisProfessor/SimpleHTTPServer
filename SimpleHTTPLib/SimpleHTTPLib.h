#ifndef EXTCLIB_SIMPLEHTTPLIB_H_
#define EXTCLIB_SIMPLEHTTPLIB_H_

#include <stdint.h>

typedef struct HTTPrequest{
    char method[16]; 
    char path[2048];
    char proto[16];
    uint8_t state;
    size_t index;
}HTTPrequest;

typedef struct HTTP HTTP;

extern HTTP *NewHTTP(char *address);
extern void FreeHTTP(HTTP *http);
extern void HandleHTTP(HTTP *http, char *path, void(*)(int, HTTPrequest*));
extern int8_t ListenHTTP(HTTP *http);
extern void ParseHTML(int conn, char *filename);

#endif /* EXTCLIB_SIMPLEHTTPLIB_H_*/
