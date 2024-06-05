/******************************************************************************
* echo_client.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo client.  The  *
*              client connects to an arbitrary <host,port> and sends input    *
*              from stdin.                                                    *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <errno.h>

#define ECHO_PORT 9999
#define BUF_SIZE 4096
#define KEEP_ALIVE

static struct addrinfo *servinfo;


int initClient(const char* server_ip, const char* port){
    fprintf(stdout, "----- Liso Client -----\n");

    struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;  //IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
    hints.ai_flags = AI_PASSIVE; //fill in my IP for me

    int status, clientSock;
    // struct addrinfo *servinfo; //will point to the results
    if ((status = getaddrinfo(server_ip, port, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo error: %s \n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    if((clientSock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
    {
        fprintf(stderr, "Socket failed");
        return EXIT_FAILURE;
    }

    return clientSock;
}


int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "usage: %s <server-ip> <port> <txt_file>", argv[0]);
        return EXIT_FAILURE;
    }


    int sock = 0;
    sock = initClient(argv[1], argv[2]);
    
    if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        fprintf(stderr, "Connect failed\n");
        return EXIT_FAILURE;
    }
        
    char msg[BUF_SIZE]; 
    FILE *fp = fopen(argv[3], "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return 1;
    }
    int readRet = fread(msg, 1, BUF_SIZE, fp);
    int bytes_received;
    fprintf(stdout, "-------------Sending-----------\n%s", msg);
    send(sock, msg , strlen(msg), 0);

    char buf[BUF_SIZE];

#ifdef KEEP_ALIVE
    while((bytes_received = recv(sock, buf, BUF_SIZE, 0)) > 1)
    {
        buf[bytes_received] = '\0';
        fprintf(stdout, "-------------Received-----------\n%s", buf);
    }
#else
    if((bytes_received = recv(sock, buf, BUF_SIZE, 0)) > 1)
    {
        buf[bytes_received] = '\0';
        fprintf(stdout, "-------------Received-----------\n%s", buf);
    }
#endif
    // printf("bytes_received: %d\n", bytes_received);

    freeaddrinfo(servinfo);
    close(sock);    
    return EXIT_SUCCESS;
}
