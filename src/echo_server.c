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



int main(int argc, char* argv[])
{
    int sock, client_sock;
    ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];

    fprintf(stdout, "----- Echo Server -----\n");
    
    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }


    if (listen(sock, 5))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

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

        readret = 0;

        while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
        {
            printf("----------recv---------\n %s\n", buf);
            // int count = 0;
            // int start = 0;
            // for(int i = 0; i < readret; i++){
            //     if(strncmp(buf + i, "\r\n\r\n", 4) == 0){
            //         char temp[512];
            //         memset(temp, 0, 512);
            //         strncpy(temp, buf + start, i + 4 - start);
            //         // printf("-------------------\n %s\n", temp);
            //         Request *request = parse(temp, readret, client_sock);
            //         respond(request, temp);
            //         printf("----------send---------\n %s\n", temp);
            //         start = i + 4;
            //         count++;
            //     }
            // }

            Request *request = parse(buf, readret, client_sock);
            respond(request, buf);
            // ERROR_LOG(cli_addr, client_sock, "Error reading from client socket.");
            // ACCESS_LOG(cli_addr, client_sock, "Access reading from client socket.");
            printf("----------send---------\n %s\n", buf);
            if (send(client_sock, buf, strlen(buf), 0) != strlen(buf))
            {
                close_socket(client_sock);
                close_socket(sock);
                fprintf(stderr, "Error sending to client.\n");
                return EXIT_FAILURE;
            }

            // 非常重要的一步，释放 yylex 内存
            yylex_destroy();
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
