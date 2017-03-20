#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<ctype.h>
#include<string.h>

int socket_desc;
char message[50], reply[1000];

void uploadFile(){
	if(recv(socket_desc, reply, 1000, 0) < 0){
		printf("Could not recieve.\n");
		return;
	}
	
	printf("%s\n", reply);

	scanf("%s", message);

	if(send(socket_desc, message, strlen(message)+1, 0) < 0){
		printf("Could not send.\n");
		return;
	}

	FILE *fp;
	fp=fopen(message, "r");
	
	while(fscanf(fp, "%c", &message[0])!=EOF){
		if(send(socket_desc, message, 1, 0) < 0){
			printf("Could not send.\n");
			return;
		}
	}

	message[0]='\0';
	if(send(socket_desc, message, 1, 0) < 0){
		printf("Could not send.\n");
		return;
	}
	printf("Done.\n");
	fclose(fp);	
}

void downloadFile(){
	if(recv(socket_desc, reply, 1000, 0) < 0){
		printf("Could not recieve.\n");
		return;
	}
	
	printf("%s\n", reply);

	scanf("%s", message);

	if(send(socket_desc, message, strlen(message)+1, 0) < 0){
		printf("Could not send.\n");
		return;
	}
	
	FILE *fp;
	fp=fopen(message, "w");
	
	if(recv(socket_desc, reply, 1, 0) < 0){
		printf("Could not recieve.\n");
		return;
	}
	
	while(reply[0]!='\0'){
		fprintf(fp, "%c", reply[0]);
		if(recv(socket_desc, reply, 1, 0) < 0){
			printf("Could not recieve.\n");
			return;
		}
	}	
	printf("Done.\n");
	fclose(fp);	
}
	
int main(){
	
	struct sockaddr_in server;
	
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(6969);

	if(connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
		printf("Cannot connect to server.");
		close(socket_desc);
		return 1;
	}
	
	printf("Connection established.\n");
	
	if(recv(socket_desc, reply, 1000, 0) < 0){
		printf("Message not recieved.\n");
		close(socket_desc);
		return 1;
	}
	printf("Message recieved.\nServer : ");
	
	printf("%s\n", reply);

	scanf("%c", &message[0]);

	message[1]='\0';

	if(send(socket_desc, message, strlen(message)+1, 0) < 0){
		printf("Message not sent.\n");
		close(socket_desc);
		return 1;
	}
	printf("Message sent.\n");

	if(message[0]=='1'){
		uploadFile();
	}
	else if(message[0]=='2'){
		downloadFile();
	}
	else{
		printf("Invalid choice.\n");
	}
	close(socket_desc);
	return 0;
}
