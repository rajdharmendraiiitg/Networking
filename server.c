#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>


char message[100], *reply, ch[2];
pthread_mutex_t lock;

int socket_desc, connection_desc, count=5;
struct sockaddr_in server, client;
socklen_t client_len;

void uploadFile(){
	reply="Please give the name of file to be uploaded : ";
	if(send(connection_desc, reply, strlen(reply)+1, 0) < 0){
		printf("Could not send.\n");
		return;
	}
	printf("Message sent.\n");

	if(recv(connection_desc, message, 100, 0) < 0){
		printf("Could not recieve.\n");
		return;
	}
	printf("Message recieved.\n");

	FILE *fp;
	fp=fopen(message, "w");
	if(fp==NULL){
		printf("Could not open file.\n");
		return;
	}
	
	if(recv(connection_desc, message, 1, 0) < 0){
		printf("Could not recieve.\n");
		return;
	}
	
	while(message[0]!='\0'){
		fprintf(fp, "%c", message[0]);
		if(recv(connection_desc, message, 1, 0) < 0){
			printf("Could not recieve.\n");
			return;
		}
	}	
	printf("Done.\n");
	fclose(fp);
}

void downloadFile(){
	reply="Please give the name of file to be downloaded : ";
	if(send(connection_desc, reply, strlen(reply)+1, 0) < 0){
		printf("Could not send.\n");
		return;
	}
	printf("Message sent.\n");
	if(recv(connection_desc, message, 100, 0) < 0){
		printf("Could not recieve.\n");
		return;
	}
	printf("Message recieved.\n");

	FILE *fp;
	fp=fopen(message, "r");
	if(fp==NULL){
		printf("Could not open file.\n");
		return;
	}
	
	while(fscanf(fp, "%c", &ch[0])!=EOF){
		if(send(connection_desc, ch, 1, 0) < 0){
			printf("Could not send.\n");
			return;
		}
		
	}

	ch[0]='\0';
	if(send(connection_desc, ch, 1, 0) < 0){
		printf("Could not send.\n");
		return;
	}
	printf("Done.\n");
	fclose(fp);	
}

void * Server(void * a){
	//char ch='y';
	//while(ch=='y'){
	pthread_mutex_lock(&lock);

	connection_desc = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&client_len);
	
	if(connection_desc==-1){
		printf("Could not connect to client.\n");
		return 1;
	}

	printf("Connection established.\n");
	
	reply = "Hello client! What would u like to do? Press the given numbers to choose the desired option :\n1. Upload\n2. Download\n";
	
	if(send(connection_desc, reply, strlen(reply)+1, 0) < 0){
		printf("Could not send.\n");
		return;
	}
	printf("Message sent.\n");
	
	if(recv(connection_desc, message, 100, 0) < 0){
		printf("Could not recieve.\n");
		return;
	}
	printf("Message recieved.\n");
	
	if(message[0] == '1'){
		uploadFile();
		printf("Uploading\n");
	}
	else if(message[0] == '2'){
		downloadFile();
		printf("Downloading\n");
	}
	else{
		printf("Invalid choice.\n");
	}
	printf("\n");
	//scanf("%c", &ch);
	pthread_mutex_unlock(&lock);
	//}
}

int main(){

	pthread_t tid[4];

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc==-1){
		printf("Could not create socket.\n");
		return 1;
	}
	printf("Socket created successfully.\n");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(6969);
	
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
		printf("Could not bind socket with server.\n");
		return 1;
	}
	printf("Server created successfully.\n");
	
	listen(socket_desc, 5);
	
	printf("Waiting for clients. . .\n");
	
	client_len = sizeof(client);

	

	for(int i=0; i<4; i++){
		
		pthread_create(&tid[i], NULL, Server, NULL);
	}

	for(int i=0; i<4; i++){
		pthread_join(tid[i], NULL);
	}
	
	close(connection_desc);
	close(socket_desc);
	return 0;
}
