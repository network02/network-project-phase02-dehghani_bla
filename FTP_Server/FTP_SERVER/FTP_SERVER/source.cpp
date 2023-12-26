#include<winsock2.h>
#include <stdlib.h>

void service(void);

int main(int argc , char **argv , char **env)
{
	service();
	system("pause");
	return (0);
}

void service(void)
{
	unsigend short clientDataPort;
	unsigned short clientControlPort;
	unsigned short serverControlPort = 21;
	unsigned short serverDataPort = 20;
	WSAData wsadata;
	WSAStartup( MAKEWORD(2,2) , &wsadata);
	int serverSock = socket(AF_INET , SOCK_STREAM, IPPROTO_IP);
	int serverDataSock = socket(AF_INET , SOCK_STREAM , IPPROTO_IP);
	sockaddr_in serverAddr;
	sockaddr_in serverDataAddr;

	
}