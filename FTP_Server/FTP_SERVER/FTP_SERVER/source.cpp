#include<winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>
#include <string.h>

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
std::string linkedList_print(struct linkedList *ll){
	std::string res("");
	struct node *tmp = ll->head;
	while(tmp){
		res.append(tmp->command);
		res.append("\n");
		puts(tmp->command);
		tmp = tmp->next;
	}
	return res;
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

char reply[39][128]=
{ 
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
,"425 Can’t open data connection."
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

char helpMessage[2048]=
"\t\t\t in the name of GOD\n"
"server commands and discussions are listed below :\n"
"1_ \"USER\" : format:(USER <username>) discussion:(this command will send username to server for authentication of user)\n"
"2_ \"PASS\" : format:(PASS <password>) discussion:(this command usage is after USER command for sending password to the server\n"
"2_ \"LIST\" : format:(LIST <pathname>) discussion:(this command will list files and directories on server .pathname argument is not mandatory)\n"
"3_ \"RETR\" : format:(RETR <file pathname>) discussion:(this command will return file stored at pathname on server.)\n"
"4_ \"STOR\" : format:(STOR <client file pathname> <server file pathname>) discussion:(this command will store client file on server. if in server alredy exist will replace)\n"
"5_ \"DELE\" : format:(DELE <pathname>) discussion:(this command will delete file stored at pathname on server .)\n"
"6_ \"MKD\" : format:(MKD <pathname>) discussion:(this command will create a directory on server with address pathname .)\n"
"7_ \"RMD\" : format:(RMD <pathname>) discussion:(this command will remove a directory on server with address pathname .)\n"
"8_ \"PWD\" : format:(PWD) discussion:(this command will return working directory address on server(print working directory) .)\n"
"9_ \"CWD\" : format:(CWD <pathname>) discussion:(this command will change working directory on server to pathname (change working directory) .)\n"
"10_ \"CDUP\" : format:(CDUP) discussion:(this command will change current direcy to parent of that .)\n"
"11_ \"QUIT\" : format:(QUIT) discussion:(this command closes client connection between server and client.if a file is transferring the operation will complete and then connection closes .)\n"
"12_ \"RPRT\" : format:(RPRT) discussion:(this command will return history of all commands has send to server for clients wich have permission .\n"
"13_ \"HELP\" : format:(HELP <command name>) discussion:(if after HELP without any command name , all server commands help will send ; else command's help will send .\n"
;

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
} registeredUsers[4] = { {"ali" , "123" , 1 , 1} , { "mahdi" , "123" , 0 , 0} , {"ahmad" , "123" , 0 , 1} , {"mahmood" , "123" , 1 , 0} };

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
	status1=listen(serverSock, SOMAXCONN);
	puts("server is listenening on port 21 :");
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
	int sscanfRes;
	char *strstrRes;
	char helpLine[256];
	int i;
	std::string historyStr;
	do
	{
		struct userSpecifications currentUser;
		currentUser.isAdmin = 1;
		currentUser.hasAccessPrivateFiles = 0;
		currentUser.userName[0]=0;
		currentUser.password[0]=0;
		connectedControlSock=accept(serverSock , (struct sockaddr *) &clientAddr , &clientAddrLen);
		puts("a client connected to server .");
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
		puts("receiving command ...");
		status1=recv(connectedControlSock , command , 128 , 0);
		while(status1>0)
		{
			linkedList_pushback(&historyList , command);
			sscanfRes=sscanf(command , "%s %s", method , argument);
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
			else if (strcmp(method, "RPRT") == 0) 
			{
				send(connectedControlSock , reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]) , 0);
				if(currentUser.isAdmin){
					historyStr=linkedList_print(&historyList);
					send(connectedControlSock , historyStr.c_str(),historyStr.size() + 1 , 0);
				}
				else
				{
					send(connectedControlSock , "you don't have needed permissions !" , 1 + 35 , 0 );
				}
			}
			else if (strcmp(method, "HELP") == 0) // since here PROjECT NEED
			{
				send (connectedControlSock , reply[reply_code_index_find(214)] , 1 + strlen(reply[reply_code_index_find(214)]) , 0);
				if(sscanfRes == 1){
					send(connectedControlSock , helpMessage , 1 + strlen(helpMessage) , 0 );
				}
				else // argument contains a command name
				{
					strstrRes=strstr(helpMessage,argument);
					if(strstrRes == NULL){
						send(connectedControlSock , "help for this command not found !" , 1+26 , 0);
					}
					else
					{
						strstrRes-=1;
						i=0;
						while((*strstrRes) != '\n'){
							helpLine[i]=(*strstrRes);
							++strstrRes;
							++i;
						}
						helpLine[i]='\0';
						send(connectedControlSock , helpLine , i+1 , 0);
					}
				}
				
			}
			else if (strcmp(method, "PORT") == 0) // sets client data socket(welcomming socket) port (format of port command: PORT h1,h2,h3,h4,p1,p2)
			{
			}
			else if (strcmp(method, "PASV") == 0) // 
			{	
			}
			else if (strcmp(method, "NOOP") == 0)
			{
				send(connectedControlSock, reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]),0);
			}
			else
			{
				send(connectedControlSock , reply[reply_code_index_find(502)] , 1 + strlen(reply[reply_code_index_find(502)]) , 0);
			}
			puts("receiving command ...");
			status1=recv(connectedControlSock , command , 128 , 0);
		}
		if(status1 == 0){
			printf("%s" , "connection closed by client !\n");
		}else if(status1<0){
			printf("recv failed !\tError-Code : %d -> connection to this client will close soon...\n", WSAGetLastError());
			closesocket(connectedControlSock);
		}
	}while(true);
}