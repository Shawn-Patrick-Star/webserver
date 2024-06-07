/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>


#include "respond.h"
#include "parse.h"
#include "log.h"

#define ECHO_PORT 9999
#define BUF_SIZE 4096
// #define TIMEOUT
#define FAIL_INITSOCK -1


int close_socket(int sock);

int initServer(int port);

int main(int argc, char* argv[])
{
    int listen_sock = initServer(ECHO_PORT);
    if (listen_sock < 0) { printf("initserver() failed.\n"); return -1; }
    printf("listensock=%d\n",listen_sock);

    fd_set readfds; // 需要监视读事件的socket的集合，大小为16字节（1024位）的bitmap
    FD_ZERO(&readfds); // 初始化readfds，把bitmap的每一位都置为0
    FD_SET(listen_sock, &readfds); // 把服务端用于监听的socket加入readfds
    int maxfd = listen_sock; // readfds中socket的最大值

    while(1)
    {
        fd_set tmpfds = readfds; // 在select()中 会修改bitmap 所以 要把readfds复制一份给tmpfds 再把tmpfds传给select()
        #ifdef TIMEOUT        
        struct timeval timeout;     
        timeout.tv_sec=10;        // 秒
        timeout.tv_usec=0; 
        int infds = select(maxfd + 1, &tmpfds, NULL, NULL, &timeout);
        #else
        int infds = select(maxfd + 1, &tmpfds, NULL, NULL, 0);
        #endif  
        if (infds < 0){
            perror("select() failed"); 
            break;
        }else if(infds == 0){
            printf("select() timeout.\n"); 
            continue;
        }

        for (int eventfd = 0; eventfd <= maxfd; eventfd++)
        {
            if (FD_ISSET(eventfd, &tmpfds) == 0) continue;

            if (eventfd == listen_sock){ 
                struct sockaddr_in cli_addr;
                socklen_t cli_size = sizeof(cli_addr);
                int client_sock = accept(listen_sock, (struct sockaddr *)&cli_addr, &cli_size);
                if(client_sock < 0){ perror("accept() failed"); continue; };
                printf ("accept client(socket=%d) ok.\n", client_sock);

                FD_SET(client_sock, &readfds);

                maxfd = client_sock > maxfd ? client_sock : maxfd;
            }
            else{
                ssize_t readret = 0;
                char all_request[BUF_SIZE];// 读入全部请求
                if((readret = recv(eventfd, all_request, BUF_SIZE, 0)) >= 1)
                {

                    printf("----------recv---------\n %s\n", all_request);
                    // pipeline
                    char* start = all_request; 
                    char* end;
                    while((end = strstr(start, "\r\n\r\n")) != NULL){
                        char one_request[512];// 读入一次请求
                        memset(one_request, 0, 512);
                        int len = end - start + 4;
                        strncpy(one_request, start, len);
                        // 解析
                        Request *request = parse(one_request, len, eventfd);
                        
                        char packet[1024];// 生成响应报文
                        memset(packet, 0, 1024);
                        respond(request, packet, one_request);

                        printf("----------send---------\n %s\n", packet);

                        if (send(eventfd, packet, strlen(packet), 0) != strlen(packet))
                        {
                            close_socket(eventfd);
                            close_socket(listen_sock);
                            fprintf(stderr, "Error sending to client.\n");
                            return EXIT_FAILURE;
                        }
                        yylex_destroy();
                        start = end + 4;
                    }

                    // ERROR_LOG(cli_addr, client_sock, "Error reading from client socket.");
                    // ACCESS_LOG(cli_addr, client
                }
                else{
                    printf("client(eventfd=%d) disconnected.\n",eventfd);
                    
                    close(eventfd);                         // 关闭客户端的socket
                    FD_CLR(eventfd,&readfds);     // 把bitmap中已关闭客户端的标志位清空。

                    if (eventfd == maxfd){              // 重新计算maxfd的值，注意，只有当eventfd==maxfd时才需要计算。
                        for (int ii = maxfd; ii > 0; ii--){    // 从后面往前找。
                            if (FD_ISSET(ii,&readfds)){
                                maxfd = ii; 
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

int initServer(int port){
    fprintf(stdout, "----- Liso Server -----\n");

    int sock;

    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return FAIL_INITSOCK;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr))){
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return FAIL_INITSOCK;
    }

    if (listen(sock, 5)){
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return FAIL_INITSOCK;
    }

    return sock;
}

int close_socket(int sock)
{
    if (close(sock) == -1)
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 0;
    }
    return 1;
}