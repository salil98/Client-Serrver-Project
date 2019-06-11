#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#define PORT 8080

int main()
{

	int sockfd, ret, opt = 1;
	struct sockaddr_in serverAddr;

	int newSocket;
	// sockaddr_in - references elements of the socket address. "in" for internet
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	pid_t childpid;
	// creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	// Prevents error such as: “address already in use
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;					 // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
	serverAddr.sin_port = htons(PORT);					 // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Accept connections from any IP address - listens from all interfaces.

	// Forcefully attaching socket to the port 8080
	ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 8080);

	if (listen(sockfd, 10) == 0)
	{
		printf("[+]Listening....\n");
	}
	else
	{
		printf("[-]Error in binding.\n");
	}

	while (1)
	{
		newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
		if (newSocket < 0)
		{
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if ((childpid = fork()) == 0)
		{
			close(sockfd);
			char client_reply[1024], temp[1024], message[1024];
			char ip[100] = {};
			inet_ntop(AF_INET, &(serverAddr.sin_addr), ip, INET_ADDRSTRLEN);
			while (1)
			{
				bzero(client_reply, sizeof(client_reply));
				bzero(message, sizeof(message));
				// read infromation received into the buffer.
				recv(newSocket, client_reply, 1024, 0);
				puts("client_reply:");
				puts(client_reply);
				if (strcmp(client_reply, "exit") == 0)
				{
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
				else if (strcmp(client_reply, "listall") == 0)
				{
					int length = 0;
					struct dirent *de;
					DIR *dr = opendir(".");
					if (dr == NULL)
					{
						printf("Could not open current directory");
						return 0;
					}
					while ((de = readdir(dr)) != NULL)
					{
						if (de->d_type != DT_DIR)
						{
							bzero(temp, 1024);
							sprintf(temp, "%s\n", de->d_name);
							length += snprintf(message + length, 1024 - length, temp);
						}
					}

					closedir(dr);
					send(newSocket, message, strlen(message), 0);
				}
				else
				{
					char f_path[1024] = {};
					sprintf(f_path, "./%s", client_reply);
					FILE *fp = fopen(f_path, "r");
					if (fp == NULL)
					{
						write(newSocket, "ERROR\0", 6);
						printf("File opern error");
					}
					else
					{
						/* Read data from file and send it */
						while (1)
						{
							/* First read file in chunks of 256 bytes */
							unsigned char buff[256] = {0};
							int nread = fread(buff, 1, 256, fp);
							/* If read was success, send data. */
							if (nread > 0)
							{
								printf("nread:%d\n", nread);
								send(newSocket, buff, nread, 0);
							}
							/*
							 * There is something tricky going on with read .. 
					 		 * Either there was error, or we reached end of file.
							 */
							if (nread < 256)
							{
								if (feof(fp))
									printf("End of file\n");
								if (ferror(fp))
									printf("Error reading\n");

								break;
							}
						}
						fclose(fp);
					}
				}
			}
		}
	}

	close(newSocket);

	return 0;
}
