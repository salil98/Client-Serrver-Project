#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1024], server_reply[1024];

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    memset(&server, '0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");
    bzero(server_reply, sizeof(server_reply));

    //keep communicating with server
    while (1)
    {
        printf(">>");
        gets(message);
        if (strcmp(message, "exit") == 0)
            break;
        if (strcmp(message, "listall") == 0)
        {
            if (send(sock, message, strlen(message), 0) < 0)
            {
                puts("Send failed");
                return 1;
            }
            if (recv(sock, server_reply, 1024, 0) < 0)
            {
                puts("recv failed");
                break;
            }

            printf("%s", server_reply);
        }
        else
        {

            if (send(sock, message, strlen(message), 0) < 0)
            {
                puts("Send failed");
                return 1;
            }
            FILE *fp = fopen(message, "w");
            if (fp == NULL)
            {
                printf("Error opening file");
                return 1;
            }
            int bytesReceived = 0;
            char recvBuff[256];
            memset(recvBuff, '0', sizeof(recvBuff));
            int flag = 0, cnt = 0;
            while ((bytesReceived = recv(sock, recvBuff, 256, 0)) > 0)
            {

                // printf("bytes-read%d %d\n", bytesReceived, cnt);
                if (strcmp(recvBuff, "ERROR\0") == 0)
                {
                    flag = 1;
                    break;
                }

                fwrite(recvBuff, 1, bytesReceived, fp);
                if (bytesReceived < 256)
                    break;

                //cnt++;
                //if (cnt >= 226)
                //  break;
            }

            fclose(fp);
            if (bytesReceived < 0)
                printf("\n Read Error \n");
            if (flag == 0)
                printf("File transmission completed for %s\n", message);
            else
            {
                printf("Such a file doesn't exsist at server side\n");
                remove(message);
            }
        }

        bzero(server_reply, sizeof(server_reply));
        bzero(message, sizeof(message));
    }

    close(sock);
    return 0;
}