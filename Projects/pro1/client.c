#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int a_portNum;
	char *a_hostName;

	if(argc != 3){
		fprintf(stderr, "client <HOST A NAME> <HOST A PORT>\n");
		exit(1);
	}

	printf("The port number is %s\n", argv[3]);
	printf("The host name is %s\n", argv[2]);






	return 0;

}
