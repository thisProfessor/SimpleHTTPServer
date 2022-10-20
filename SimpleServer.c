#include <stdio.h>
#include <string.h>
#include "SimpleHTTPLib/SimpleHTTPLib.h"

void index_page(int conn, HTTPrequest *req) {
	if(strcmp(req->path, "/") != 0) {
		ParseHTML(conn, "html/page404.html");
		return;
	}
	ParseHTML(conn, "html/index.html");
}

void about_page(int conn, HTTPrequest *req) {
	if(strcmp(req->path, "/about/") != 0) {
		ParseHTML(conn, "html/page404.html");
		return;
	}
	ParseHTML(conn, "html/about.html");
}


int main(void) {
	printf("\t\t!!!-----SERVER STARTED-----!!!\n\n");

	char *socket = "127.0.0.1:7545";
	HTTP *route = NewHTTP(socket);

	HandleHTTP(route, "/", index_page);
	HandleHTTP(route, "/about/", about_page);

	ListenHTTP(route);
	FreeHTTP(route);
	return 0;
}
