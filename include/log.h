#ifndef LOG_H 
#define LOG_H

#include <netinet/ip.h>
#include <netinet/in.h>

// #define ERRORLOG(format, ...) printf("\33[1;35m" format "\33[0m", ## __VA_ARGS__)
// #define ACCESSLOG(format, ...) printf("\33[1;36m" format "\33[0m", ## __VA_ARGS__)

void ERROR_LOG(struct sockaddr_in cli_addr, int fd, const char* msg);
    
void ACCESS_LOG(struct sockaddr_in cli_addr, int fd, const char* msg);

#endif //LOG_H