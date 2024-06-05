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

#include "respond.h"
#include "parse.h"
#include "log.h"
#define ECHO_PORT 9999
#define BUF_SIZE 4096


int close_socket(int sock)
{
    if (close(sock) == -1)
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 0;
    }
    return 1;
}

int initServer(int port){
    fprintf(stdout, "----- Liso Server -----\n");

    int sock;

    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr))){
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }

    if (listen(sock, 5)){
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

    return sock;
}

int main(int argc, char* argv[])
{
    int sock = initServer(ECHO_PORT);
    
    int client_sock;
    socklen_t cli_size;
    struct sockaddr_in cli_addr;
    

    /* finally, loop waiting for input and then write it back */
    while (1)
    {
        cli_size = sizeof(cli_addr);
        if ((client_sock = accept(sock, (struct sockaddr *)&cli_addr, &cli_size)) == -1)
        {
            close(sock);
            fprintf(stderr, "Error accepting connection.\n");
            return EXIT_FAILURE;
        }

        ssize_t readret = 0;
        char buf[BUF_SIZE];// 读入全部请求
        while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
        {
            printf("----------recv---------\n %s\n", buf);
            // pipeline
            char* start = buf; 
            char* end;
            while((end = strstr(start, "\r\n\r\n")) != NULL){
                char one_request[512];// 读入一次请求
                memset(one_request, 0, 512);
                int len = end - start + 4;
                strncpy(one_request, start, len);

                Request *request = parse(one_request, len, client_sock);
                
                char packet[1024];// 生成响应报文
                memset(packet, 0, 1024);
                respond(request, packet, one_request);

                printf("----------send---------\n %s\n", packet);

                if (send(client_sock, packet, strlen(packet), 0) != strlen(packet))
                {
                    close_socket(client_sock);
                    close_socket(sock);
                    fprintf(stderr, "Error sending to client.\n");
                    return EXIT_FAILURE;
                }
                yylex_destroy();
                start = end + 4;
            }

            // ERROR_LOG(cli_addr, client_sock, "Error reading from client socket.");
            // ACCESS_LOG(cli_addr, client_sock, "Access reading from client socket.");

            memset(buf, 0, BUF_SIZE);
        } 

        if (readret == -1) // 接受错误 连接断开
        {
            close_socket(client_sock);
            close_socket(sock);
            fprintf(stderr, "Error reading from client socket.\n");
            return EXIT_FAILURE;
        }

        if (!close_socket(client_sock))
        {
            close_socket(sock);
            fprintf(stderr, "Error closing client socket.\n");
            return EXIT_FAILURE;
        }
    }

    close_socket(sock);

    return EXIT_SUCCESS;
}
