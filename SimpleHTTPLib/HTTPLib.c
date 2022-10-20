#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hashtab.h"
#include "SysSocketFramework.h"
#include "SimpleHTTPLib.h"

#define METHOD_SIZE 16
#define PROTO_SIZE  16
#define PATH_SIZE   2048

typedef struct HTTP{
    char *host;
    int32_t len;
    int32_t cap;
    void(**funcs)(int, HTTPrequest*);
    HashTab *tab;
}HTTP;

static HTTPrequest _new_request(void);
static void _page404_http(int conn);
static void _null_request(HTTPrequest *request);
static int8_t _switch_http(HTTP *http, int conn, HTTPrequest *request);
static void _parse_request(HTTPrequest *request, char *buffer, size_t size);

extern HTTP *NewHTTP(char *address){
    HTTP *http = (HTTP*)malloc(sizeof(HTTP));
    http->cap = 1000;
    http->len = 0;
    http->host = (char*)malloc(sizeof(char)*strlen(address)+1);
    strcpy(http->host, address);
    http->tab = new_hashtab(http->cap, STRING_TYPE, DECIMAL_TYPE);
    http->funcs = (void(**)(int, HTTPrequest*))malloc(http->cap * (sizeof (void(*)(int, HTTPrequest*))));
    return http;
}

extern void FreeHTTP(HTTP *http){
    free_hashtab(http->tab);
    free(http->host);
    free(http->funcs);
    free(http);
}

extern void HandleHTTP(HTTP *http, char *path, void(*handle)(int, HTTPrequest*) ){
    set_hashtab(http->tab, string(path), decimal(http->len));
    http->funcs[http->len] = handle;
    http->len += 1;
    if(http->len == http->cap){
        http->cap <<= 1;
        http->funcs = (void(**)(int, HTTPrequest*))realloc(http->funcs, 
			http->cap * (sizeof (void(*)(int, HTTPrequest*))));
    }
}

extern int8_t ListenHTTP(HTTP *http){
    int listener = ListenNet(http->host);
    if (listener < 0){
        return 1;
    }
    while(1){
        int conn = AcceptNet(listener);
        if(conn < 0){
            continue;
        }
        HTTPrequest req = _new_request();
        while(1){
            char buffer[BUFSIZ] = {0};
            int n = RecvNet(conn, buffer, BUFSIZ);
            if(n < 0){
                break;
            }
            _parse_request(&req, buffer, BUFSIZ);
            if(n != BUFSIZ){
                break;
            }
        }
        _switch_http(http, conn, &req);
        CloseNet(conn);
    }
    CloseNet(listener);
    return 0;
}

extern void ParseHTML(int conn, char *filename){
    char buffer[BUFSIZ] = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
    size_t readsize = strlen(buffer);
    SendNet(conn, buffer, readsize);
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        return;
    }
    while((readsize = fread(buffer,sizeof(char), BUFSIZ, file)) != 0){
        SendNet(conn, buffer, readsize);
    }
    fclose(file);
}

static HTTPrequest _new_request(void){
    return (HTTPrequest){
        .method = {0},
        .path   = {0},
        .proto  = {0},
        .state  =  0,
        .index  =  0,
    };
}

static void _parse_request(HTTPrequest *request, char *buffer, size_t size){
    printf("!!!-----REQUEST-----!!!\n");
    printf("%s\n", buffer);
    for(size_t i = 0; i < size; ++i){
        switch (request->state){
        case 0:
            if(buffer[i] == ' ' || request->index == METHOD_SIZE - 1){
                request->method[request->index] = '\0';
                _null_request(request);
                continue;
            }
            request->method[request->index] = buffer[i];
            break;
        case 1:
            if(buffer[i] == ' ' || request->index == PATH_SIZE - 1){
                request->path[request->index] = '\0';
                _null_request(request);
                continue;
            }
            request->path[request->index] = buffer[i];
            break;
        case 2:
            if(buffer[i] == '\n' || request->index == PROTO_SIZE - 1){
                request->proto[request->index] = '\0';
                _null_request(request);
                continue;
            }
            request->proto[request->index] = buffer[i];
            break;
        default: return;
        }
        request->index +=1;
    }
}

static void _null_request(HTTPrequest *request){
    request->state += 1;
    request->index = 0;
}

static int8_t _switch_http(HTTP *http, int conn, HTTPrequest *request){
    if(!in_hashtab(http->tab, string(request->path))){
        char buffer[PATH_SIZE];
        memcpy(buffer, request->path, PATH_SIZE);
        int32_t index = strlen(request->path);
        if (index == 0){
            _page404_http(conn);
            return 1;
        }
        index -= 1;
        buffer[index] = '\0';
        for(; index > 0 && buffer[index] != '/'; --index){
            buffer[index] = '\0';
        }
        if(!in_hashtab(http->tab, string(buffer))){
            _page404_http(conn);
            return 2;
        }
        index = get_hashtab(http->tab, string(buffer)).decimal;
        http->funcs[index](conn, request);
        return 0;
    }
    int32_t index = get_hashtab(http->tab, string(request->path)).decimal;
    http->funcs[index](conn, request);
    return 0;
}

static void _page404_http(int conn){
    char *header = "HTTP/1.1 404 Not Found\n\nnot found";
    size_t headsize = strlen(header);
    SendNet(conn, header, headsize);
}
