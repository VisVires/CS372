#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>


#define MAXDATASIZE 500 	//max data size is 500 bytes

int main(int argc, char *argv[])
{
	int sockfd, numbytes, status, read_result, write_result;
	struct addrinfo addr, *res;
	char *hostname;
	char *port;
	size_t handleSize = 11;
	char *handle;
	char *a_handle;
	char rbuffer[MAXDATASIZE+1];
	char wbuffer[MAXDATASIZE+1];
	char quit[] = "\\quit";

	//make sure user input is correct
	if(argc != 3){
		fprintf(stderr, "usage: client <HOST A NAME> <HOST A PORT>\n");
		exit(1);
	}
	
	//get commandline args
	hostname = argv[1];
	port = argv[2];

	//reset hints to 0
	memset(&addr, 0, sizeof(addr));
	addr.ai_family = AF_UNSPEC;		//don't care if IPv4 or IPv6
	addr.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(hostname, port, &addr, &res);	
	
	//make socket
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0)
	{
		printf("error creating socket\n");
		exit(1);
	}
	//printf("Socket Created\n");

	//connect to server
	status = connect(sockfd, res->ai_addr, res->ai_addrlen);
	if (status < 0){
		printf("Error connecting to server\n");
		exit(1);
	}
	printf("Connected\n");


	//choose and send client handle
	printf("Please Enter A Handle ");
	handle = malloc(sizeof(char) * 11);
	int hlength = 0;
	//get appropriate length handle
	while(hlength == 0 || hlength > 11){
		printf("10 Characters Long: ");
		hlength = getline(&handle, &handleSize, stdin);
	}
	//fflush(stdout);
	handle[strcspn(handle, "\n")] = 0;
	//printf("handle: %s\n", handle);
	//send to user
	write_result = send(sockfd, handle, 11, 0);
	if (write_result == -1){
		printf("Write Error");
		exit(1);
	}
	
	
	//get handle from server
	a_handle = malloc(sizeof(char) * 11);
	read_result = recv(sockfd, a_handle, handleSize, 0);
	if (read_result == -1){
		printf("Read Error");
		exit(1);
	}
	//}
	//add null character to read input
	a_handle[read_result] = '\0';
	printf("You are now speaking to: %s\n", a_handle);
	printf("You Start!>");
	//flush stdout
	fflush(stdout);
	
	//continue until connection is closed
	while(1)
	{
			
		//reset write buffer	
		memset(&wbuffer, 0, sizeof(wbuffer));	
		
		//send response
		fflush(stdout);
		printf(">");
		fgets(wbuffer, MAXDATASIZE+1, stdin);		
		//check for quit signal
		if (strncmp(wbuffer, "\\quit", 5) == 0){
			printf("Goodbye\n");
			close(sockfd);
			exit(0);
		}
		//write message to server
		write_result = send(sockfd, wbuffer, strlen(wbuffer), 0);
		if (write_result == -1){
			printf("Write Error");
			close(sockfd);
			exit(1);
		}
		
				
		//reset read buffer
		memset(&rbuffer, 0, sizeof(rbuffer));
		if(read_result = recv(sockfd,rbuffer, MAXDATASIZE, 0)){
			if (read_result == -1){
				printf("Read Error");
				close(sockfd);
				exit(1);
			}
			else{
				rbuffer[read_result] = '\0';
				//printf("%d", read_result);
				printf("%s> %s\n", a_handle, rbuffer);
			}
		}
		//flush standard input
		fflush(stdout);
	} 
	//close client socket
	close(sockfd);	
	return 0;
}
