#include<winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>

void service(void);


struct node{
	struct node *next;
	char command[128];
};
struct linkedList{
	struct node *head;
	struct node *tail;
	int size;
} historyList;
void linkedList_init(struct linkedList *ll){
	ll->head=NULL;
	ll->tail=NULL;
	ll->size=0;
}
void linkedList_pushback(struct linkedList *ll , char const *command){
	++(ll->size);
	struct node *tmp = ll->tail;
	struct node *newNode = (struct node *) malloc (sizeof(struct node));
	strcpy(newNode->command , command);
	newNode->next = NULL;
	if(tmp == NULL){
		ll->head=ll->tail= newNode;
		return;
	}
	tmp->next = newNode;
	ll->tail = newNode;
}
void linkedList_print(struct linkedList *ll){
	struct node *tmp = ll->head;
	while(tmp){
		puts(tmp->command);
		tmp = tmp->next;
	}
}
void linkedList_delete( struct linkedList *ll){
	struct node *tmp = ll->head , *tmp2;
	while(tmp){
		tmp2=tmp->next;
		free((void *) tmp);
		tmp=tmp2;
	}
	ll->head= ll->tail = NULL;
	ll->size=0;
}

char reply[39][128]={ 
 "110 Restart marker reply."
,"120 Service ready in nnn minutes."
,"125 Data connection already open; transfer starting."
,"150 File status okay; about to open data connection. };"
,"200 Command okay."
,"202 Command not implemented, superfluous at this site."
,"211 System status, or system help reply."
,"212 Directory status."
,"213 File status."
,"214 Help message."
,"215 NAME system type."
,"220 Service ready for new user."
,"221 Service closing control connection."
,"225 Data connection open; no transfer in progress."
,"226 Closing data connection."
,"227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)."
,"230 User logged in, proceed."
,"250 Requested file action okay, completed."
,"257 \"PATHNAME\" created."
,"331 User name okay, need password."
,"332 Need account for login."
,"350 Requested file action pending further information."
,"421 Service not available, closing control connection."
,"425 Can�t open data connection."
,"426 Connection closed; transfer aborted."
,"450 Requested file action not taken."
,"451 Requested action aborted: local error in processing."
,"452 Requested action not taken."
,"500 Syntax error, command unrecognized."
,"501 Syntax error in parameters or arguments."
,"502 Command not implemented."
,"503 Bad sequence of commands."
,"504 Command not implemented for that parameter."
,"530 Not logged in."
,"532 Need account for storing files."
,"550 Requested action not taken."
,"551 Requested action aborted: page type unknown."
,"552 Requested file action aborted."
,"553 Requested action not taken."
};

short replyMap[39]=
{
	110   ,  120   , 125   ,  150  ,  200   ,  202   ,  211   ,  212   ,
	213   ,  214   ,  215  ,  220  ,  221   ,  225   ,  226   ,  227   ,
	230   ,  250   ,  257  ,  331  ,  332   ,  350   ,  421   ,  425   , 
	426   ,  450   ,  451  ,  452  ,  500   ,  501   ,  502   ,  503   , 
	504   ,  530   ,  532  ,  550  ,  551   ,  552   ,  553 
};
short reply_code_index_find(short code)
{
	for( short i=0 ; i<39 ; i++ )
		if (replyMap[i] == code )
			return (i);
	return -1;
}

struct userSpecifications
{
	char userName[124];
	char password[124];
	char isAdmin;
	char hasAccessPrivateFiles;
	;//etc
} users[4] = { {"ali" , "123" , 1 , 1} , { "mahdi" , "123" , 0 , 0} , {"ahmad" , "123" , 0 , 1} , {"mahmood" , "123" , 1 , 0} };

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
	int error=WSAStartup( MAKEWORD(2,2) , &wsadata);
	if (error) {
		puts("can't start windows socket 2 !");
		system("pause");
		exit(EXIT_FAILURE);
	}
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
	int clientAddrLen;
	clientAddrLen= sizeof(clientAddr); // this line is NEEDED (else error cde 10014 will occure in accept ! )
	char command[128];
	char method[5];
	char argument[124];
	linkedList_init(&historyList);
	do
	{
		connectedControlSock=accept(serverSock , (struct sockaddr *) &clientAddr , &clientAddrLen);
		// send liste farman ha + code
		if(connectedControlSock == INVALID_SOCKET)
		{
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(serverSock);
			closesocket(serverDataSock);
			WSACleanup();
			linkedList_delete(&historyList);
			system("pause");
			exit(EXIT_FAILURE);
		}
		status1=recv(connectedControlSock , command , 128 , 0);
		while(status1>0)
		{
			linkedList_pushback(&historyList , command);
			sscanf(command , "%s %s", method , argument);
			if (strcmp(method, "USER") == 0)
			{
				send(connectedControlSock , reply[reply_code_index_find(331)] , 1 + strlen(reply[reply_code_index_find(331)]) , 0);
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
			else if (strcmp(method, "QUIT") == 0)
			{
				
			}
			else if (strcmp(method, "RPRT") == 0) // since here PROjECT NEED
			{
				linkedList_print(&historyList);
			}
			else if (strcmp(method, "HELP") == 0)
			{
				send (connectedControlSock , reply[reply_code_index_find(214)] , 1 + strlen(reply[reply_code_index_find(214)]) , 0);
				// ......
			}
			else if (strcmp(method, "PORT") == 0) // sets client data socket(welcomming socket) port (format of port command: PORT h1,h2,h3,h4,p1,p2)
			{

			}
			else
			{
				send(connectedControlSock , reply[reply_code_index_find(502)] , 1 + strlen(reply[reply_code_index_find(502)]) , 0);
			}
			status1=recv(connectedControlSock , command , 128 , 0);
		}
		if(status1 == 0){
			printf("%s" , "connection closed by client !\n");
		}else if(status1<0){
			printf("recv failed !\tError-Code:%d -> connection to this client will close soon...\n", WSAGetLastError());
			closesocket(connectedControlSock);
		}
	}while(true);
}