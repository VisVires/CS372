#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	long a_portNum;
	char *a_hostName;
	char *eptr;
	char handle[11];

	if(argc != 3){
		fprintf(stderr, "client <HOST A NAME> <HOST A PORT>\n");
		exit(1);
	}

	printf("The port number is %s\n", argv[2]);
	printf("The host name is %s\n", argv[1]);

	a_portNum = strtol(argv[2], &eptr, 10);
	printf("Port Number %ld\n", a_portNum); 

	printf("Please Enter A Handle: ");
	scanf("%s", handle);
	printf("You chose %s\n", handle);


	return 0;

}
