#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<WinSock2.h>

void mainMenu();

char loginStatus;

int main(int argc , char **argv)
{
	loginStatus = 0;
	mainMenu();
	return (0);
}

void mainMenu()
{
	char serverIpAddress[16];
	char command[128];
	char method[5];
	char argument[124];
	WSADATA wsadata;
	unsigned short serverControlPort = 21; // control connection port (L)
	unsigned short clientControlPort; // control connection port (U)
	unsigned short serverDataPort = 20; // data connection port = control port(L) - 1 
	unsigned short clientDataPort; //  data connection port = control port (U)
	int error = WSAStartup(0x0202, &wsadata);
	if (error){
		puts("can't initiate network requirements of windows");
		exit(EXIT_FAILURE);
	}
	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (clientSock == INVALID_SOCKET){
		puts("can't create socket ");
		exit(EXIT_FAILURE);
	}
	puts("please enter your host ip address for FTP-connection");
	scanf("%s", serverIpAddress);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverControlPort);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(serverIpAddress);
	int connectionStatus = connect(clientSock, (struct sockaddr *) & serverAddr, sizeof(serverAddr));
	if (connectionStatus == SOCKET_ERROR){
		puts("can't connect to server please check your network connection and then try again!");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in clientAddr;
	int clientAddrLen;
	getsockname(clientSock, (struct sockaddr*) &clientAddr,&clientAddrLen);
	printf("%hu", ntohs(clientAddr.sin_port));
	do
	{
		puts("please enter your command :");
		fflush(stdin);
		gets(command);
		sscanf("%s %s", method, argument);
		if (strcmp(method, "USER") == 0)
		{

		}
		else if (strcmp(method, "PASS") == 0)
		{
		}
		else if (strcmp(method, "PASS") == 0)
		{
		}
		else if (strcmp(method, "LIST") == 0)
		{
		}
		else if (strcmp(method, "RETR") == 0)
		{
		}
		else if (strcmp(method, "STOR") == 0)
		{
		}
		else if (strcmp(method, "DELE") == 0)
		{
		}
		else if (strcmp(method, "MKD") == 0)
		{
		}
		else if (strcmp(method, "RMD") == 0)
		{
		}
		else if (strcmp(method, "PWD") == 0)
		{
		}
		else if (strcmp(method, "CWD") == 0)
		{
		}
		else if (strcmp(method, "CDUP") == 0)
		{
		}
		else if (strcmp(method, "QUIT") == 0) // since here PROjECT NEED
		{
		}
		else if (strcmp(method, "HELP") == 0)
		{
		}
		else if (strcmp(method, "PORT") == 0) // sets client data port
		{
		}

	}while (true);
}