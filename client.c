#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// the port client will be connecting to
#define PORT 1358
// max number of bytes we can get at once
#define MAXDATASIZE 300
char message[300];
char chat[500];

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char username[300];
    struct hostent *he;
    // connector’s address information
    struct sockaddr_in their_addr;
     
    // if no command line argument supplied
    if(argc != 4)
    {
        fprintf(stderr, "Client-Usage: %s the_client_hostname\n", argv[0]);
        // just exit
        exit(1);
    }
    
    strcpy(username, argv[1]);

    // get the host info
    if((he=gethostbyname(argv[2])) == NULL)
    {    perror("gethostbyname()");
        exit(1);
    }
    else
        printf("Client-The remote host is: %s\n", argv[2]);
     
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket()");
        exit(1);
    }
    else
        printf("Client-The socket() sockfd is OK...\n");
     
    // host byte order
    their_addr.sin_family = AF_INET;
    // short, network byte order
    printf("Server-Using %s and port %d...\n", argv[2], atoi(argv[3]));
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    // zero the rest of the struct
    memset(&(their_addr.sin_zero), '\0', 8);
     
    if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect()");
        exit(1);
    }
    else
        printf("Client-The connect() is OK...\n\n");
  
    write(sockfd, username, sizeof(username));
    read(sockfd, chat, 500);
    printf("%s\n", chat);
    //printf("%s has joined the chatroom\n", username);
    
    memset(&(message), '\0', 300);
    memset(&(chat), '\0', 500);

    //int r = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    FD_SET(0, &readfds);

    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(0, &readfds);


        select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        if(FD_ISSET(sockfd, &readfds))  
        {
            read(sockfd, chat, 499);
            chat[499] = '\0';
            printf("%s\n", chat);
        }
        if(FD_ISSET(0, &readfds))
        {
            fgets(message, 500, stdin);
            write(sockfd, message, sizeof(message));
            //char msg[500];
            //if(read(sockfd, msg, 500) > 0)
            //{
            //    msg[499] = '\0';
            //    printf("%s\n", msg);
            // }
        }
        memset(&(message), '\0', 300);
        memset(&(chat), '\0', 500);
    }
    printf("%s has left the chatroom\n", username);

    if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
        perror("recv()");
        exit(1);
    }
    else
        printf("Client-The recv() is OK...\n");
     
    buf[numbytes] = '\0';
    printf("Client-Received: %s", buf);
     
    printf("Client-Closing sockfd\n");
    close(sockfd);
    return 0;
}
