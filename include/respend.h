#ifndef RESPEND_H
#define RESPEND_H

#include "parse.h"
#include <stdbool.h>

#define default_file_path "./static_site/index.html"

typedef enum {
    HTTP_200 = 0, 
    HTTP_400, 
    HTTP_404, 
    HTTP_501,
    HTTP_505
} HTTP_STATUS;

typedef enum {
    GET = 0, 
    POST, 
    HEAD,
    UNKNOWN
} HTTP_METHOD;



bool strIsEqual(char *str1, const char *str2);

HTTP_METHOD method_str2enum(char * method);

void copyString(char *dest, const char *src, int len);

void respend(Request *request, char* buf);

#endif //RESPEND_H