#include "log.h"

#include <time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
void ERROR_LOG(struct sockaddr_in cli_addr, int fd, const char* msg){
    time_t now;
    time(&now);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';
    printf("\33[1;35m" "[%s] [Error] [Client_ip:%12s:%-7d] [client fd %d] %s\n" "\33[0m\n", time_str, 
            inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), fd, msg);
}

void ACCESS_LOG(struct sockaddr_in cli_addr, int fd, const char* msg){
    time_t now;
    time(&now);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';
    printf("\33[1;36m" "[%s] [Access] [Client_ip:%12s:%-7d] [client fd %d] %s\n" "\33[0m\n", time_str, 
            inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), fd, msg);
}