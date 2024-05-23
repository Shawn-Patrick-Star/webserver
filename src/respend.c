#include "respend.h"
#include <stdbool.h>


bool strIsEqual(char *str1, const char *str2){
	int i = 0;
	while(str1[i] != '\0' && str2[i] != '\0'){
		if(str1[i] != str2[i]){
			return false;
		}
		i++;
	}
	if(str1[i] == '\0' && str2[i] == '\0'){
		return true;
	}
	return false;
}

HTTP_METHOD method_str2enum(char * method){
    if(strIsEqual(method, "GET")) return GET;
    if(strIsEqual(method, "POST")) return POST;
    if(strIsEqual(method, "HEAD")) return HEAD;
    return UNKNOWN;
}

void copyString(char *dest, const char *src, int len){
    memset(dest, 0, strlen(dest));
    len = len > strlen(src) ? strlen(src) : len;
    for(int i = 0; i < len; i++){
        dest[i] = src[i];
    }
}

void respend(Request *request, char* buf){
    if(request == NULL){
        copyString(buf, "HTTP/1.1 400 Bad Request\r\n\r\n", 29);
        return;
    }
    // 404
    if(request->http_uri[0] != '/'){
        copyString(buf, "HTTP/1.1 404 Not Found\r\n\r\n", 27);
        return;
    }
    // 505
    if(!strIsEqual(request->http_version, "HTTP/1.1")){
        copyString(buf, "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n", 43);
        return;
    }


    HTTP_METHOD method = method_str2enum(request->http_method);
    switch (method)
    {
    case GET:
        
        break;
    case POST:
        
        break;
    case HEAD:
            
        break;
    default:
        copyString(buf, "HTTP/1.1 501 Not Implemented\r\n\r\n", 33);
        return;
    }
    return;
}



void handle_get_request(Request *request, char *buf, int *status_code){
    if(request->http_uri[0] == '/'){
        char *path = request->http_uri + 1;
        FILE *file = fopen(path, "r");
        if(file == NULL){
            *status_code = HTTP_404;
            return;
        }
        copyString(buf, "HTTP/1.1 200 OK\r\n\r\n", 19);
        char ch;
        int i = 19;
        while((ch = fgetc(file)) != EOF){
            buf[i++] = ch;
        }
        fclose(file);
    }
    else{
        *status_code = HTTP_400;
    }
}