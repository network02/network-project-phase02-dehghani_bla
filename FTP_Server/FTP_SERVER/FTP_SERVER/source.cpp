#include<winsock2.h>
#include <stdlib.h>
#include <stdio.h>

void service(void);

int main(int argc , char **argv , char **env)
{
	service();
	system("pause");
	return (0);
}

void service(void)
{
	unsigned  short clientDataPort;
	unsigned short clientControlPort;
	unsigned short serverControlPort = 21;
	unsigned short serverDataPort = 20;
	WSAData wsadata;
	WSAStartup( MAKEWORD(2,2) , &wsadata);
	int serverSock = socket(AF_INET , SOCK_STREAM, IPPROTO_IP);
	int serverDataSock = socket(AF_INET , SOCK_STREAM , IPPROTO_IP);
	if( (serverSock == INVALID_SOCKET) || (serverDataSock == INVALID_SOCKET) ){
		puts("can't create server needed socket !");
		if(serverSock != INVALID_SOCKET)
			closesocket(serverSock);
		else if(serverDataSock != INVALID_SOCKET)
			closesocket(serverDataSock);
		WSACleanup();
		system("pause");
		exit(EXIT_FAILURE);
	}
	sockaddr_in serverAddr;
	sockaddr_in serverDataAddr;
	serverAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	serverDataAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverDataAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverControlPort);
	serverDataAddr.sin_port = htons(serverDataPort);
	int status1,status2;
	status1=bind(serverSock , (struct sockaddr *) &serverAddr , sizeof(serverAddr));
	status2=bind(serverDataSock , (struct sockaddr *) &serverDataAddr , sizeof(serverDataAddr));
	if( (status1 == SOCKET_ERROR) || (status2 == SOCKET_ERROR) ){
		puts("can't bind server socket !");
		closesocket(serverSock);
		closesocket(serverDataSock);
		WSACleanup();
		system("pause");
		exit(EXIT_FAILURE);
	}
	status1=listen(serverSock, 1/*SOMAXCONN*/);
	if(status1==SOCKET_ERROR){
		puts("Error listening on socket!");

	}
	int connectedControlSock;
	struct sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr); // this code is NEEDED (else error cde 10014 will occure in accept ! )
	do
	{
		connectedControlSock=accept(serverSock , (struct sockaddr *) &clientAddr , &clientAddrLen);
		if(connectedControlSock == INVALID_SOCKET)
		{
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(serverSock);
			closesocket(serverDataSock);
			WSACleanup();
			system("pause");
			exit(EXIT_FAILURE);
		}
		
	}while(true);
	WSACleanup();

	
}