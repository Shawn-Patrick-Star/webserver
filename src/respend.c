#include "respend.h"

#include <stdbool.h>
#include <string.h>
#include <errno.h>


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

char* method_enum2str(HTTP_METHOD method){
    switch (method)
    {
    case GET:
        return "GET";
    case POST:
        return "POST";
    case HEAD:
        return "HEAD";
    default:
        return "UNKNOWN";
    }
}


void copyString(char *dest, const char *src, int len){
    memset(dest, 0, strlen(dest));
    len = len > strlen(src) ? strlen(src) : len;
    for(int i = 0; i < len; i++){
        dest[i] = src[i];
    }
}

void respend(Request *request, char* buf){
    char *path;


    if(request == NULL){
        copyString(buf, "HTTP/1.1 400 Bad Request\r\n\r\n", 29);
        return;
    }
    // 404
    if(request->http_uri[0] != '/'){
        path = default_file_path;
        FILE *file = fopen(path, "r");
        if(file == NULL){
            fprintf(stderr, "Error opening file: %s\n", strerror(errno));
            copyString(buf, "HTTP/1.1 404 Not Found\r\n\r\n", 27);
            return;
        }
        fclose(file);
    }
    else{
        // 等待补充
        path = request->http_uri;
        // Host 地址 + path
        // 判断 文件（网站）地址 是否存在
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



void handle_get_request(Request *request, char *buf){
    char *path;
    
    if(request->http_uri[0] == '/'){
        path = default_file_path;
        FILE *fp = fopen(path, "r");
        // 改成追加字符串
        copyString(buf, "HTTP/1.1 200 OK\r\n\r\n", 19);
        // 以下内容都是随便写的
        int BUF_SIZE = 1024;
        int len = fread(buf + 19, 1, BUF_SIZE - 19, fp);
        fclose(fp);
    }
    else{
        // 等待补充
        path = request->http_uri;
        // Host 地址 + path
        // 判断 文件（网站）地址 是否存在
    }
}