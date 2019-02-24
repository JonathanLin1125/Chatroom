#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 1358
#define MYPORT PORT
#define MAXDATASIZE 300

#define BACKLOG 10
char buffer[500];
char enter[500];
char name[500];
char full_msg[500];
char hostname[1023];
char all_names[500][500];
int main()
{
    int sockfd, new_fd[10];
    int fd_size = 0;
    struct sockaddr_in my_addr; //my address information
    struct sockaddr_in their_addr; //remote address information
    int sin_size;
    fd_set readfds;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == - 1)
    {
        perror("socket() error");
        exit(1);
    }
    else
    {
        printf("socket() is OK...\n");
        fflush(stdout);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), 0, 8);

    gethostname(hostname, 1023);
    printf("%s\n", hostname);


    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind() error");
        exit(1);
    }
    else
    {
        printf("bind() is OK...\n");
        fflush(stdout);
    }

    if(listen(sockfd, BACKLOG) == -1)
    {
        perror("listen() error");
        exit(1);
    }
    else
    {
        printf("listen() is OK...\n");
        fflush(stdout);
    }

    

    while(1)
    {
        memset(&(name), '\0', 500);
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        for(int i = 0; i < fd_size; ++i)
        {
            if(new_fd[i] != - 1)
                FD_SET(new_fd[i], &readfds);
        }

        select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        if(FD_ISSET(sockfd, &readfds))
        {
            sin_size = sizeof(struct sockaddr_in);
            new_fd[fd_size] = accept(sockfd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size);
        
            read(new_fd[fd_size], name, 499);
            printf("ENTER %s\n", name);
            strcpy(all_names[fd_size], name); 
            fd_size++;
        
            memset(&(enter), '\0', 500);
            strcat(enter, "ENTER ");
            strcat(enter, name);
            for(int i = 0; i < fd_size; ++i)
            {
                write(new_fd[i], enter, 499);
            }
        
        }
        else
        {
        int n;
        for(int i = 0; i < fd_size; ++i)
        {
            int sd = new_fd[i];
            if(FD_ISSET(sd, &readfds))
            {
                if(n != -1 && (n = read(sd, buffer, 499)) == 0)
                {
                    printf("LEAVE %s\n", all_names[i]);
                    strcat(full_msg, "LEAVE ");
                    strcat(full_msg, all_names[i]);
                    for(int k = 0; k < fd_size; ++k)
                    {
                        write(new_fd[k], full_msg, 499);
                    }
                    new_fd[i] = -1;
                    close(new_fd[i]);
                }
                else
                {
                    printf("%s: %s", all_names[i], buffer); 
                    strcat(full_msg, all_names[i]);
                    strcat(full_msg, ": ");
                    strcat(full_msg, buffer);
                    for(int k = 0; k < fd_size; ++k)
                        write(new_fd[k], full_msg, 499);
                }
            }
            memset(&(buffer), '\0', 500);
            memset(&(full_msg), '\0', 500);
        }
        }
    }

    printf("\n");
    memset(&(buffer), '\0', 500);

    //const char* msg = "I got your message";
/*    int n;
    
    while((n = read(new_fd[fd_size - 1], buffer, 499)) > 0)
    {
        printf("%s: %s", name, buffer);
        fflush(stdout);
        strcat(full_msg, name);
        strcat(full_msg, ": ");
        strcat(full_msg, buffer);
        write(new_fd[fd_size - 1], full_msg, 499);
        memset(&(buffer), '\0', 500);
        memset(&(full_msg), '\0', 500);
    }

    printf("LEAVE %s\n", name);
    close(new_fd[fd_size - 1]);
    close(sockfd);*/
    return 0;
}
