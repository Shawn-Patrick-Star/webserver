#include "respond.h"

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>


void respond(Request *request, char* buf, const char * request_str){
    char path[128];

    // 400
    if(request == NULL){
        create_packet("", "HTTP/1.1 400 Bad Request", 0, "html", buf);
        return;
    }

    // 505
    if(!strIsEqual(request->http_version, "HTTP/1.1")){
        create_packet("", "HTTP/1.1 505 HTTP Version Not Supported", 0, "html", buf);
        return;
    }

    // 可以将FILE* 设置为static静态全局变量 以减少文件打开次数 也不用传参了

    HTTP_METHOD method = method_str2enum(request->http_method);
    switch (method)
    {
    case GET:
        handle_get_request(request, buf);
        break;
    case POST:
        create_packet(request_str, "HTTP/1.1 200 OK", strlen(request_str), "html", buf);
        break;
    case HEAD:
        create_packet("", "HTTP/1.1 200 OK", 0, "html", buf);
        break;
    default:
        create_packet("", "HTTP/1.1 501 Not Implemented", 0, "html", buf);
        break;
    }

}



void handle_get_request(Request *request, char *buf){
    char path[128];
    // 确定文件路径
    if(strIsEqual(request->http_uri, "/"))
        sprintf(path, "%s", default_file_path);
    else
        sprintf(path, "%s%s", "./static_site", request->http_uri);
    
    // 确定文件类型
    const char * dot = strrchr(path, '.');
    if(dot == NULL){
        fprintf(stderr, "Error: file type not found\n");
        return;
    }
    char file_type[16];
    memset(file_type, 0, 16);
    int i = 0;
    while(dot[i+1] != '\0'){
        file_type[i] = dot[i+1];
        i++;
    }

    // 读取文件内容
    FILE *fp = fopen(path, "r");
    if(fp == NULL){
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        create_packet("", "HTTP/1.1 404 Not Found", 0, "html", buf);
        return;
    }

    char temp[1024];
    int body_len = fread(temp, 1, 1024, fp);
    create_packet(temp, "HTTP/1.1 200 OK", body_len, file_type, buf);
    
    fclose(fp);
}


void create_packet(const char* body, const char* statusLine, int body_len, char* file_type, char* packet){
    // 生成响应头
    char time_buffer[80];
    get_time(time_buffer, sizeof(time_buffer));
    sprintf(packet, "%s\r\n", statusLine);
    sprintf(packet, "%sServer: liso/1.0\r\n", packet);
    sprintf(packet, "%sDate: %s\r\n", packet, time_buffer);
    sprintf(packet, "%sContent-Length: %d\r\n", packet, body_len);
    sprintf(packet, "%sContent-Type: text/%s\r\n\r\n", packet, file_type);
    // 生成响应体
    strcat(packet, body);
}


void get_time(char *buffer, size_t len){
    memset(buffer, 0, len);
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    // 格式：Sun, 02 Jun 2024 02:42:16 GMT
    strftime(buffer, len, "%a, %d %b %Y %H:%M:%S GMT", local_time);
}

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

// void handle_head_request(Request *request, char *buf){

// }