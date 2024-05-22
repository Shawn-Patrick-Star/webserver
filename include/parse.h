#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0

enum {
	NORMAL = 0, HTTP_501, HTTP_400
};


//Header field
typedef struct
{
	char header_name[4096];
	char header_value[4096];

} Request_header;


//HTTP Request Header
typedef struct
{
	char http_version[50];
	char http_method[50];
	char http_uri[4096];
	Request_header *headers;
	int header_count;
	int MAX_HEADER_COUNT;

} Request;

Request* parse(const char *buffer, int size,int socketFd, int *status_code);

// functions decalred in parser.y
int yyparse();
void set_parsing_options(char *buf, size_t i, Request *request);
