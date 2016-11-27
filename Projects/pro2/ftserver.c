#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAXSIZE 1024 

void dataConnection(char *, int, int);
void sendDirContents(int, char *, char *);
void sendFileToClient(int, char*, char *, char *);
void readClient(int, char *, int);
void writeClient(int, char *, int);
int fileExist(char *);



int main(int argc, char *argv[])
{
	char *port, *dPort; //*command;
	char buffer[MAXSIZE], command[MAXSIZE];
	char *list = "-l";
	char *get = "-g";
	char *good = "OK";
	char *badName = "FAIL";	
	char ipstr[INET6_ADDRSTRLEN];
	int sockfd, new_socket, client_port;;
	struct sockaddr_storage cli_addr;
	socklen_t addr_size;
	struct addrinfo serv_addr, *res;
	int read_result, write_result;	

	if (argc != 2) {
		fprintf(stderr, "Usage: ftserver <SERVER_PORT>\n");
		exit(1);
	}
	//set port number
	port = argv[1];
	
	//clear memory for serv_addr and set up serv_addr	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.ai_family = AF_UNSPEC;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, port, &serv_addr, &res);	

	//set up TCP server
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0){
		error("error opening server socket\n");
		exit(1);
	}
	
	//beej.us.setcokoptman
	int optval = 1;	
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1)
		error("setsockopt");
	
	//set up bind
	if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
		error("Bind 1 Failed\n");
	};	

	//listen for new connections
	listen(sockfd, 10);
	printf("Server open on: %s\n", port);	
	
	//get client size 
	addr_size = sizeof cli_addr;
	
	//clear client
	memset(&cli_addr, 0, sizeof(cli_addr));
	while (1){
		//accept new connection
		new_socket = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_size);
		if(new_socket < 0)
		{
			printf("Connection Failed!!\n");
			break;
		}
		
		int com;
		int validCommand = 1;
		//printf("Connected\n");
			
		//output client details
		//beej.us/guide/output/html/multipage/getpeernameman.html
		getpeername(new_socket, (struct sockaddr *) &cli_addr, &addr_size);
		struct sockaddr_in *o_sock = (struct sockaddr_in *) &cli_addr;
		client_port = ntohs(o_sock->sin_port);
		inet_ntop(AF_INET, &o_sock->sin_addr, ipstr, sizeof ipstr);
		printf("Connection from: %s at Client Port: %d\n", ipstr, client_port);
		
		//get port from client
		memset(&buffer, 0, MAXSIZE);
		read_result = recv(new_socket, buffer, sizeof(buffer), 0);
		if (read_result == -1){
			error("Error getting new port");
		}
		else{
			buffer[read_result] = '\0';
		}

		//set new port to dPort	
		dPort = buffer;
		//printf("Data Port: %s\n", dPort);	
		//send confirmation
		writeClient(new_socket, good, strlen(good));
			
		fflush(stdout);	
		
		//clear buffer and get command
		memset(&command, 0, MAXSIZE);
		read_result = recv(new_socket, command, sizeof(command), 0);	
		if (read_result == -1){
			error("Error getting new port");
		}
		else{
			command[read_result] = '\0';
		}
			
		//printf("Command: %s\n", command);
		//check if command valid and send confirmation or invalid statement
		if(strcmp(command,list) != 0 && strcmp(command, get) != 0){
			printf("Invalid Command\n");
			validCommand = 0;
			//send invalid name to client
			writeClient(new_socket, badName, strlen(badName));
			close(new_socket);
		}
		else{
			if(strcmp(command, list) == 0)
			{
				com = 1;
			}
			else if(strcmp(command, get) == 0)
			{
				com = 2;
			}
			//write confirmation	
			writeClient(new_socket, good, strlen(good));
		}
		
		//fork to create data connection
		if(validCommand){
			pid_t cpid = fork();
			if (cpid < 0){
				error("Error on fork\n");
			}
		
			//in child
			if(cpid == 0){
				//printf("In Child\n");
				//send to data connection function
				dataConnection(dPort, com, sockfd);	
				close(sockfd);
				close(new_socket);
				exit(0);
			}
		}
		close(new_socket);
	}
	close(sockfd);	
	return 0;
}

/*
 *
 *
 *
 *
 *
 *
 *
*/

void dataConnection(char *port, int command, int osock){

	int sock, nsock;
	char buf[MAXSIZE];
	char *filename;
	char *good = "OK";
	char *badName = "FAIL";
	int nport, validfile;
	char ipstr[INET6_ADDRSTRLEN];
	struct sockaddr_storage cli_addr;
	socklen_t addr_size;
	struct addrinfo serv_addr, *res;
	int read_result, write_result;	

	//printf("Command2: %s\n", command);
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.ai_family = AF_UNSPEC;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, port, &serv_addr, &res);	

	//set up TCP server
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock < 0){
		error("error opening data socket\n");
		exit(1);
	}

	//beej.us.setcokoptman
	int optval = 1;	
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1)
		error("setsockopt");
	

	//set up bind
	if(bind(sock, res->ai_addr, res->ai_addrlen) == -1){
		error("Bind 2 Failed\n");
	};	

	//listen for new connections
	listen(sock, 10);
	
	//get client size 
	addr_size = sizeof cli_addr;
	
	//clear client
	memset(&cli_addr, 0, sizeof(cli_addr));
	
	//printf("Awaiting Second Connection\n");
	nsock = accept(sock, (struct sockaddr *) &cli_addr, &addr_size);
	if(nsock < 0)
	{
		printf("Connection Failed!!\n");
		return;
	}
		
	//output client details
	getpeername(nsock, (struct sockaddr *) &cli_addr, &addr_size);
	struct sockaddr_in *o_sock = (struct sockaddr_in *) &cli_addr;
	nport = ntohs(o_sock->sin_port);
	inet_ntop(AF_INET, &o_sock->sin_addr, ipstr, sizeof ipstr);
	//printf("Connection from: %s at Client Port: %d\n", ipstr, nport);
	
	//if command is a list
	if (command == 1){
		printf("List directory requested on port %s\n", port);
		sendDirContents(nsock, port, ipstr);
	}
	//if command is a get
	else if (command == 2){
	
		memset(&buf, 0, MAXSIZE);
		//get filename from client	
		read_result = recv(nsock, buf, sizeof(buf), 0);
		if (read_result == -1){
			error("Error getting filename");
		}
		else{
			buf[read_result] = '\0';
		}		
		printf("File \"%s\" requested on port %s\n", buf, port);
		validfile = fileExist(buf);
		if (validfile){
			//send confirmation
			writeClient(nsock, good, strlen(good));
			//send file to client
			sendFileToClient(nsock, buf, port, ipstr);
		}
		else{
			printf("File not found. Sending error message to %s: %s\n",ipstr ,port);
			writeClient(nsock, badName, strlen(badName));
		}
	}	
		
	close(nsock);
	close(sock);
}

/*
 *
 *
 *
 *
 */
//source: man7.org/linux/man-pages/man2/stat2.html
int fileExist(char *filename){
	struct stat s;
	int result = stat(filename, &s);
	if (result == 0){
		return 1;
	} 
	else
		return 0;
}

/*
 *
 *
 *
 */
void sendFileToClient(int sock, char *filename, char *port, char *ip){
	
	char *fileContents = NULL;
	char buf[MAXSIZE] = "";
	char good[MAXSIZE];
	size_t fileSize = 0;
	//set file pointer
	FILE *fp = fopen(filename, "r");
	//go to end of file
	fseek(fp, 0, SEEK_END);
	//locate file pointer and return size
	fileSize = ftell(fp);
	//place point back at beginning
	rewind(fp);
	//allocate space for file
	fileContents = malloc((fileSize + 1) * sizeof(*fileContents));
	//read file into buffer
	fread(fileContents, fileSize, 1, fp);
	//print file contents
	sprintf(buf, "%d", fileSize);	
	//printf("%s\n", buf);
	//printf("%s\n", fileContents);
	//send size
	//writeClient(sock, buf, sizeof(int));
	//get confirm
	//memset(&good, 0, MAXSIZE);
	//readClient(sock, good, sizeof(good)); 
	//send file contents
	printf("Sending \"%s\" to %s: %s\n", filename, ip, port);
	writeClient(sock, fileContents, fileSize);
	//free memory
	free(fileContents);
}



void sendDirContents(int sock, char *port, char *ip){
	DIR *d;
	struct dirent *dir;
	int files;
	char buffer[MAXSIZE] = ""; 
	d = opendir(".");
	if (d){
		//memset(&dir, 0, MAXSIZE);
		while ((dir = readdir(d)) != NULL){
			if (dir->d_type == DT_REG){
				//printf("%s\n", dir->d_name);
				files++;
				strcat(buffer, dir->d_name);
				strcat(buffer, "\n");		
			}
		}
		
		printf("Sending directory contents to %s: %s\n", ip, port);
		writeClient(sock, buffer, strlen(buffer));	
		
	}
	closedir(d);
}



/*
 *
 *
 *
 *
 *
 */

void readClient(int sock, char* buf, int size){
	int read_result;
	int totalSize = size;
	//get port from client
	memset(&buf, 0, MAXSIZE);
	read_result = recv(sock, buf, sizeof(buf), 0);
	if (read_result == -1){
		error("Error getting new port");
	}
	else{
		buf[read_result] = '\0';
	}
}

/*
 *
 *
 *
 *
 *
 */

void writeClient(int sock, char *ptr, int size){
	int write_result, written, leftover;
	//set to remaining file
	leftover = size;
	//while there is still information left to send
	while(leftover > 0){
		//send info
		write_result = send(sock, ptr, size, 0);
		if (write_result == -1){
			error("Error sending confirmation");
		}
		else if(write_result == 0){
			printf("Nothing is written");
		}
		else{
			//reduce left over info by amount written
			leftover -= write_result;
			//move ptr to position of string needing to be written still
			ptr += write_result;
		}
	}			
}
