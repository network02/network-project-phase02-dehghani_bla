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
	system("pause");
	return (0);
}

void mainMenu()
{
	char serverIpAddress[16];
	char command[256];
	char method[5];
	char argument[251];
	WSADATA wsadata;
	unsigned short serverControlPort = 21; // control connection port (L)
	//unsigned short clientControlPort; //  control connection port (U)
	unsigned short serverDataPort = 20; // data connection port = control port(L) - 1 
	unsigned short clientDataPort; //  data connection port = control port (U) - 1
	int error = WSAStartup(0x0202, &wsadata);
	if (error){
		puts("can't start windows socket 2 !");
		system("pause");
		exit(EXIT_FAILURE);
	}
	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (clientSock == INVALID_SOCKET){
		puts("can't create socket ");
		system("pause");
		exit(EXIT_FAILURE);
	}
	puts("please enter your host ip address for FTP-connection");
	scanf("%s", serverIpAddress);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverControlPort);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(serverIpAddress);
	//printf("%hu\n" , ntohl(serverAddr.sin_port)); // for debugging
	int connectionStatus = connect(clientSock, (struct sockaddr *) & serverAddr, sizeof(serverAddr));
	//printf("%hu\n" , ntohs(serverAddr.sin_port)); // for debugging (if connected a new socket in server is created and port number must be changed ! )
	if (connectionStatus == SOCKET_ERROR){
		puts("can't connect to server please check your network connection and then try again!");
		system("pause");
		exit(EXIT_FAILURE);
	}

	SOCKET clientDataSock;
	clientDataSock = clientSock;
	sockaddr_in clientDataConnectionAddr;
	//  code below was for assign port number of <clientSock> - 1 to clientDataSock (getsockname should be after connecting of clientSock !)
	/*
	struct sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	getsockname(clientSock, (struct sockaddr*) &clientAddr,&clientAddrLen);
	//printf("%hu", ntohs(clientAddr.sin_port));  // for debugging
	clientDataPort=ntohs(clientAddr.sin_port) - 1;
	clientDataSock=socket(AF_INET , SOCK_STREAM , IPPROTO_IP);
	clientDataConnectionAddr.sin_family=AF_INET;;
	clientDataConnectionAddr.sin_port=htons(clientDataPort);
	clientDataConnectionAddr.sin_addr.S_un.S_addr = serverAddr.sin_addr.S_un.S_addr;
	bind(clientDataSock,(struct sockaddr *) &clientDataConnectionAddr , sizeof(clientDataConnectionAddr));
	*/
	int i;
	int sendStatus;
	int receiveStatus;
	char reply[128];
	char helpMessage[2048];
	char str100k[102400];
	char str256[256];
	char str128[128];
	int sscanfRes;
	do
	{
		puts("please enter your command :");
		fflush(stdin);
		gets(command);
		sscanfRes = sscanf(command,"%s %[^\0]", method, argument);
		while(sscanfRes <1)
		{
			puts("command is unrecognized. try again :");
			gets(command);
			sscanfRes = sscanf(command , "%s %[^\0]", method , argument);
		}
		for(i=0;(command[i] != ' ') && (command[i] != '\0');i++){
			if(command[i]>='a' && command[i] <= 'z')
				command[i] = command[i] + 'A' - 'a' ;
		}
		i=0;
		while(command[i] == ' ')
			++i;
		if(strstr(command + i , "HELP") == command + i) {
			i+=5;
			while(command[i]){
				if (command[i] >='a' && command[i] <='z')
					command[i] = command[i] + 'A' - 'a';
				++i;
			}
		}
		sscanfRes = sscanf(command , "%s %[^\0]", method , argument); // because previous sscanf had not correct result because method might have small characters and will make strcmp wrong !!!!
		sendStatus=send(clientSock,command, 1 + strlen(command),0);
		if(sendStatus == SOCKET_ERROR){
			printf("can't send command !\tError-Code : <%d>\n" , WSAGetLastError());
			closesocket(clientSock);
			if(clientDataSock != clientSock)
				closesocket(clientDataSock);
			WSACleanup();
			system("pause");
			exit(EXIT_FAILURE);
		}
		recv(clientSock , reply , 128 , 0);
		puts(reply);
		if (strcmp(method, "USER") == 0)
		{
		}
		else if (strcmp(method, "PASS") == 0)
		{

		}
		else if (strcmp(method, "LIST") == 0)
		{
			recv(clientSock , str100k , 102400 , 0);
			puts(str100k);
		}
		else if (strcmp(method, "RETR") == 0)
		{
		}
		else if (strcmp(method, "STOR") == 0)
		{
		}
		else if (strcmp(method, "DELE") == 0)
		{
			recv(clientSock , str128 , 128 , 0);
			puts(str128);
		}
		else if (strcmp(method, "MKD") == 0)
		{
			recv(clientSock , str128 , 128 , 0) ;
			puts(str128);
		}	
		else if (strcmp(method, "RMD") == 0)
		{
			recv(clientSock , str128 , 128 , 0);
			puts(str128);
		}
		else if (strcmp(method, "PWD") == 0)
		{
			recv(clientSock , str256 , 256 , 0);
			puts(str256);
		}
		else if (strcmp(method, "CWD") == 0)
		{
			recv( clientSock , str128 , 128 , 0);
			puts(str128);
		}
		else if (strcmp(method, "CDUP") == 0)
		{
			recv(clientSock , str128 , 128 , 0);
			puts(str128);
		}
		else if (strcmp(method, "QUIT") == 0)
		{
		}
		else if (strcmp(method, "HELP") == 0)
		{
			 receiveStatus=recv(clientSock , helpMessage , 2048 , 0);
			 if (receiveStatus == SOCKET_ERROR) {
				printf("can't receive help from server !\tError-Code : <%d>\n" , WSAGetLastError());
				/*closesocket(clientSock);
				if(clientDataSock != clientSock)
				closesocket(clientDataSock);
				WSACleanup();
				system("pause");
				exit(EXIT_FAILURE);*/
			 }
			 else{
				puts(helpMessage);
			 }

		}
		else if (strcmp(method , "RPRT") == 0)
		{
			recv(clientSock , str100k  ,  102400/* 100KB MAX */ , 0);
			puts(str100k);
		}
		else if (strcmp(method, "NOOP") == 0)
		{
			// NOOP
		}
		// since here will implemented

		else if (strcmp(method, "PORT") == 0) // sets client data socket(welcomming socket) port (format of port command: PORT h1,h2,h3,h4,p1,p2)
		{
			//clientDataSock=socket(AF_INET , SOCK_STREAM , IPPROTO_IP);
			//clientDataConnectionAddr.sin_family=AF_INET;
			//sscanf(argument , "%hu" , &clientDataPort);
			//clientDataConnectionAddr.sin_port=htons(clientDataPort);
			//clientDataConnectionAddr.sin_addr.S_un.S_addr = serverAddr.sin_addr.S_un.S_addr;
			//bind(clientDataSock,(struct sockaddr *) &clientDataConnectionAddr , sizeof(clientDataConnectionAddr));

		}
		else if (strcmp(method, "PASV") == 0)
		{
		}
		else
		{
			// nothing to do... :|

		}

	}while (true);

}