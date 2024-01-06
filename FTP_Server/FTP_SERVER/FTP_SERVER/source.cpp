#include<winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>
#include <string.h>
#include <windows.h> // for file and directory operations

void service(void);

#define METHOD_SIZE 5 //  // max 4 char method ! + '\0'
#define COMMAND_SIZE 256
#define ARGUMENT_SIZE ( COMMAND_SIZE - 1 - (METHOD_SIZE - 1 ) - 1 + 1 )  // an space is removed in argument from command ! (-1 dar akhare ebarat)
#define FILE_BUF_SIZE 1024


struct node{
	struct node *next;
	char command[COMMAND_SIZE];
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
char *linkedList_print(struct linkedList *ll){
	char *res= (char *) malloc(1 + ll->size * 256 * sizeof(char));
	res[0]=0;
	struct node *tmp = ll->head;
	while(tmp){
		strcat(res,tmp->command);
		strcat(res,"\n");
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

#define NUMBER_OF_REPLIES 40

char reply[NUMBER_OF_REPLIES][128]=
{ 
 "110 Restart marker reply."
,"120 Service ready in nnn minutes."
,"125 Data connection already open; transfer starting."
,"150 File status okay; openning data connection ..."
,"200 Command okay."
,"202 Command not implemented, superfluous at this site."
,"211 System status, or system help reply."
,"212 Directory status:"
,"213 File status:"
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
,"400 username\\password is Incorrect ."
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


short replyMap[NUMBER_OF_REPLIES]=
{
	110   ,  120   , 125   ,  150  ,  200   ,  202   ,  211   ,  212   ,
	213   ,  214   ,  215  ,  220  ,  221   ,  225   ,  226   ,  227   ,
	230   ,  250   ,  257  ,  331  ,  332   ,  350   ,  400   ,  421   ,
	425   ,  426   ,  450   ,  451  ,  452  ,  500   ,  501   ,  502   ,
	503   ,  504   ,  530   ,  532  ,  550  ,  551   ,  552   ,  553 
};
short reply_code_index_find(short code)
{
	for( short i=0 ; i<NUMBER_OF_REPLIES ; i++ )
		if (replyMap[i] == code )
			return (i);
	return -1;
}

#define SIZE_OF_HELP_MESSAGE 2048
char helpMessage[SIZE_OF_HELP_MESSAGE]=
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

#define NUMBER_OF_REGISTERED_USERS 8
struct userSpecifications
{
	char userName[ARGUMENT_SIZE];
	char password[ARGUMENT_SIZE];
	char reportAccess;
	char readAccess;
	char writeAccess;
	;//Others
} registeredUsers[NUMBER_OF_REGISTERED_USERS] = { 
						 {"ali" , "123" , 1 , 1 , 1} , { "mahdi" , "123" , 1  , 0 , 0} , {"ahmad" , "123" , 1  , 0 , 1} , {"mahmood" , "123" , 1  , 1 , 0} 
						 ,{"ali2" , "123" , 0 , 1 , 1} , { "mahdi2" , "123" , 0  , 0 , 0} , {"ahmad2" , "123" , 0  , 0 , 1} , {"mahmood2" , "123" , 0  , 1 , 0}
					   };
void removeUserAccesses(struct userSpecifications *user)
{
	user->reportAccess = user->writeAccess = user->readAccess = 0;
}
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
	SOCKET serverSock = socket(AF_INET , SOCK_STREAM, IPPROTO_IP);
	SOCKET serverDataSock;
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
	struct sockaddr_in serverAddr;
	serverAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverControlPort);
	int status1,status2;
	status1=bind(serverSock , (struct sockaddr *) &serverAddr , sizeof(serverAddr));
	if( (status1 == SOCKET_ERROR) ){
		puts("can't bind server socket !");
		closesocket(serverSock);
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
	char command[COMMAND_SIZE];
	char method[METHOD_SIZE];
	char argument[ARGUMENT_SIZE];
	linkedList_init(&historyList);
	int sscanfRes;
	char *strstrRes;
	char helpLine[256];
	int i;
	char *historyStr;
	char str256[256];
	WIN32_FIND_DATAA ffd; // A is important...
	DWORD dwError;
	char str100k[102400];
	DWORD bufferLength;
	char *workingDirectory;
	char str128[128];
	FILE *fp;
	char fileBuf[FILE_BUF_SIZE];
	struct sockaddr_in clientDataAddr;
	clientDataAddr.sin_family=AF_INET;
	unsigned long clientDataIp;
	struct sockaddr_in serverDataAddr;
	do
	{
		struct userSpecifications currentUser;
		currentUser.reportAccess = 0;
		currentUser.readAccess = 0;
		currentUser.writeAccess = 0;
		currentUser.userName[0]=0;
		currentUser.password[0]=0;
		connectedControlSock=accept(serverSock , (struct sockaddr *) &clientAddr , &clientAddrLen);
		clientDataPort = clientControlPort = ntohs(clientAddr.sin_port);
		clientDataIp=ntohl(clientAddr.sin_addr.S_un.S_addr);
		puts("a client connected to server .");
		if(connectedControlSock == INVALID_SOCKET)
		{
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(serverSock);
			WSACleanup();
			linkedList_delete(&historyList);
			system("pause");
			exit(EXIT_FAILURE);
		}
		puts("receiving command ...");
		status1=recv(connectedControlSock , command , 256 , 0);
		while(status1>0)
		{
			linkedList_pushback(&historyList , command);
			sscanfRes=sscanf(command , "%s %[^\0]", method , argument);
			if(sscanfRes < 1 ){
				send(connectedControlSock , reply[reply_code_index_find(500)] , 1 + strlen(reply[reply_code_index_find(500)]) , 0);
				puts("receiving command ...");
				status1=recv(connectedControlSock , command , 256 , 0);
				continue;
			}
			if (strcmp(method, "USER") == 0)
			{
				send(connectedControlSock , reply[reply_code_index_find(331)] , 1 + strlen(reply[reply_code_index_find(331)]) , 0);
				if(sscanfRes == 1) // argument = NULL
					argument[0] = '\0';
				strcpy(currentUser.userName , argument);
				removeUserAccesses(&currentUser);

			}
			else if (strcmp(method, "PASS") == 0)
			{
				if(sscanfRes == 1 )
					argument[0]='\0';
				strcpy(currentUser.password , argument );
				if( (currentUser.password[0] == 0)
					|| (currentUser.userName[0] ==0) ){
					send(connectedControlSock , reply[reply_code_index_find(332)] , 1 + strlen(reply[reply_code_index_find(332)]) , 0);
					removeUserAccesses(&currentUser);
				}
				else
				{
					for(i=0 ; i<NUMBER_OF_REGISTERED_USERS ; i++)
					{
						if( (strcmp(currentUser.userName , registeredUsers[i].userName ) == 0)
							&& (strcmp(currentUser.password , registeredUsers[i].password ) == 0) ) {
							currentUser=registeredUsers[i];
							send(connectedControlSock , reply[reply_code_index_find(230)] , 1 + strlen(reply[reply_code_index_find(230)]) , 0);
							break;
						}
					}
					if( i == NUMBER_OF_REGISTERED_USERS ){
						send(connectedControlSock , reply[reply_code_index_find(400)] , 1 + strlen(reply[reply_code_index_find(400)]) , 0 );
						removeUserAccesses(&currentUser);
					}
				}
			}
			else if (strcmp(method, "LIST") == 0)
			{
				send(connectedControlSock , reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]) , 0);
				if(sscanfRes > 1)
					strcpy(str256 , argument);
				else 
					strcpy(str256 , ".");
				if(str256[strlen(str256)-1] != '\\')
					strcat(str256 , "\\*");
				else strcat(str256 , "*");
				HANDLE hFind = FindFirstFileA(str256 ,&ffd); // <A> At end of FindFirstFile is important ...
				//(about 1 hour it took my time to understand that  for Ascii , FindFirstFile doesn't work and it is defined as FindFirstFileW (Unicode or wide character set) )
				if (INVALID_HANDLE_VALUE == hFind)
				{	
					dwError = GetLastError();
					sprintf(str128 , "ERROR : An Error Occured with code <%lu>" , dwError);
					send(connectedControlSock , str128 , 1 + strlen(str128) , 0); 
				}
				else
				{
					str100k[0] = '\0';
					if( (sscanfRes > 1) && (argument[strlen(argument) - 1] == '\\') )
						argument[strlen(argument) -1] = '\0';
					do
					{
						if(sscanfRes >1)
							sprintf(str256 , "%s\\%s\n" , argument , ffd.cFileName);
						else
							sprintf(str256 , "%s\n" , ffd.cFileName);
						strcat(str100k , str256);
					}while(FindNextFileA(hFind , &ffd) != 0); // A is important.
					dwError = GetLastError();
					if(dwError != ERROR_NO_MORE_FILES){
						sprintf(str128 , "ERROR : An Error Occured with code <%lu>" , dwError);
						send(connectedControlSock , str128 , 1 + strlen(str128) , 0); 
					}
					else
					{
						send(connectedControlSock , str100k , 1 + strlen(str100k) , 0);
					}
					FindClose(hFind);
				}

			}
			else if (strcmp(method, "RETR") == 0)
			{
				
				if(sscanfRes == 1  ){
					send(connectedControlSock , reply[reply_code_index_find(450)] , 1 + strlen( reply[reply_code_index_find(450)] ) , 0);
				}
				else if(currentUser.readAccess == 0 )
				{
					send(connectedControlSock , reply[reply_code_index_find(530)] , 1 + strlen( reply[reply_code_index_find(530)] ) , 0);
				}
				else
				{
					fp=fopen(argument , "rb");
					if(fp == NULL)
					{
						send(connectedControlSock , reply[reply_code_index_find(450)] , 1 + strlen( reply[reply_code_index_find(450)] ) , 0);
					}
					else
					{
						serverDataSock = socket(AF_INET , SOCK_STREAM , IPPROTO_IP);
						serverDataAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
						serverDataAddr.sin_family = AF_INET;
						serverDataAddr.sin_port = htons(serverDataPort);
						status2=bind(serverDataSock , (struct sockaddr *) &serverDataAddr , sizeof(serverDataAddr));
						if(status2 == SOCKET_ERROR)
						{
							serverDataAddr.sin_port =htons(0); // test unassigned ports 
							//(11_ How does ftp assigns sockets to a client server connection in phase 1 project resources)
													//+ read MSDN for bind
							status2=bind(serverDataSock , (struct sockaddr *) &serverDataAddr , sizeof(serverDataAddr));
						}
						if(status2 == SOCKET_ERROR)
						{
							send(connectedControlSock , reply[reply_code_index_find(425)] , 1 + strlen(reply[reply_code_index_find(425)]) , 0 );
							closesocket(serverDataSock);
						}
						else
						{
							clientDataAddr.sin_addr.S_un.S_addr = htonl(clientDataIp);
							clientDataAddr.sin_port = htons(clientDataPort);
							int status2 = connect(serverDataSock , (struct sockaddr *)&clientDataAddr , sizeof(clientDataAddr));
							if(status2 == SOCKET_ERROR)
							{
								send(connectedControlSock , reply[reply_code_index_find(425)] , 1 + strlen(reply[reply_code_index_find(425)]) , 0 );
								closesocket(serverDataSock);
							}
							else
							{
								send(connectedControlSock , reply[reply_code_index_find(150)] , 1 + strlen( reply[reply_code_index_find(150)] ) , 0);
								i = fread(fileBuf , sizeof(char) , FILE_BUF_SIZE , fp );
								while( i == FILE_BUF_SIZE )
								{
									send(serverDataSock , fileBuf , FILE_BUF_SIZE , 0);
									i = fread(fileBuf , sizeof(char) , FILE_BUF_SIZE , fp );
								}
								fileBuf[i]=EOF;
								send(serverDataSock , fileBuf , i+1 , 0);
								send(connectedControlSock , reply[reply_code_index_find(226)] , 1 + strlen(reply[reply_code_index_find(226)]) , 0);
								closesocket(serverDataSock);
							}
						}
					}
				}
			}
			else if (strcmp(method, "STOR") == 0)
			{
				
			}
			else if (strcmp(method, "DELE") == 0)
			{
				send(connectedControlSock , reply[reply_code_index_find(213)] , 1 + strlen(reply[reply_code_index_find(213)]) , 0 );
				if(currentUser.writeAccess == 0){
					sprintf(str128 , "%s" , "you don't have permission to write(delete) files.");
					send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
				}
				else
				{
					status2=DeleteFileA(argument);
					if(status2){
						sprintf(str128 , "%s" , "OK .file successfully Deleted .");
						send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
					}
					else
					{
						if(GetLastError() == ERROR_FILE_NOT_FOUND){
							sprintf(str128 , "%s" , "ERROR : file not found .");
							send(connectedControlSock , str128  , 1 + strlen(str128) , 0 );
						}
						else if( GetLastError() == ERROR_ACCESS_DENIED)
						{
							sprintf(str128 , "%s" , "ERROR : file is Read-Only or you don't have permission to delete it.");
							send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
						}
						else{
							sprintf(str128 , "ERROR : an Error Occured with error code <%lu>",GetLastError());
							send(connectedControlSock , str128 , 1 + strlen(str128) , 0 );
						}
					}
				}
				
			}
			else if (strcmp(method, "MKD") == 0)
			{
				send (connectedControlSock , reply[reply_code_index_find(212)] , 1 + strlen(reply[reply_code_index_find(212)]) , 0);
				if(currentUser.writeAccess == 0){
					sprintf(str128 , "%s" , "you don't have permission to write(make) directories.");
					send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
				}
				else
				{
					status2 = CreateDirectoryA(argument , NULL) ;
					if(status2)
					{
						sprintf(str128 , "%s" , "OK .Directory successfully created.");
						send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
					}
					else 
					{
						if(GetLastError() == ERROR_ALREADY_EXISTS){
							sprintf(str128 , "%s" , "ERROR :  Directory already exists .");
							send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
						}
						else if (GetLastError() == ERROR_PATH_NOT_FOUND){
							sprintf(str128 , "%s" , "ERROR :  Directory PATH NOT Found .");
							send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
						}
						else if( GetLastError() == 123 ){
							sprintf(str128 , "%s" , "ERROR :  The filename, directory name, or volume label syntax is incorrect.");
							send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
							
						}
						else{
							sprintf(str128 , "ERROR : an Error Occured with code <%lu>" , GetLastError());
							send(connectedControlSock , str128 , strlen(str128) , 0 );
						}
					}
				}
			}
			else if (strcmp(method, "RMD") == 0)
			{
				send (connectedControlSock , reply[reply_code_index_find(212)] , 1 + strlen(reply[reply_code_index_find(212)]) , 0);
				if(currentUser.writeAccess == 0){
					sprintf(str128 , "%s" , "you don't have permission to write(remove) directories.");
					send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
				}
				else
				{
					status2 = RemoveDirectoryA(argument);
					if (status2)
					{
						sprintf(str128 , "%s" , "OK .Directory successfully removed.");
						send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
					}
					else
					{
						if(GetLastError() == ERROR_DIR_NOT_EMPTY){
							sprintf(str128 , "%s" , "ERROR : Directory is not empty...operation failed.");
							send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
						}
						else if(GetLastError() == ERROR_PATH_NOT_FOUND)
						{
							sprintf(str128 , "%s" , "ERROR :  Directory PATH NOT Found .");
							send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
						}
						else{
							sprintf(str128 , "ERROR : an Error Occured with code <%lu>" , GetLastError());
							send(connectedControlSock , str128 , 1 + strlen(str128) , 0 );
						}
					}
				}
			}
			else if (strcmp(method, "PWD") == 0)
			{
				send(connectedControlSock , reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]) , 0);
				i = GetCurrentDirectoryA(0 , NULL); // storing length of current(working) directory in i
				workingDirectory = (char *) malloc((i+1) *sizeof(char));
				bufferLength = i;
				i = GetCurrentDirectoryA(bufferLength , workingDirectory);
				if(i)
				{
					send(connectedControlSock , workingDirectory , i + 1 , 0); // maximum path size in client for this response in recv command is 256
				}
				else
				{
					sprintf(str128 , "Error : An Error Occured with code <%lu>." , GetLastError());
					send(connectedControlSock , str128 , 1 + strlen(str128) , 0 ); // because of path response above maximum path in client for this response at recv is 256 too.
				}
				free(workingDirectory);
			}
			else if (strcmp(method, "CWD") == 0)
			{
				send(connectedControlSock , reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]) , 0);
				status2 = SetCurrentDirectoryA(argument);
				if(status2){
					strcpy(str128 , "OK . working directory successfully changed .");
					send(connectedControlSock , str128 ,  1 + strlen(str128) , 0);
				}
				else
				{
					sprintf(str128 , "Error : An Errror Occured with code <%lu>." , GetLastError());
					send(connectedControlSock , str128 , 1 + strlen(str128) , 0);
				}
			}
			else if (strcmp(method, "CDUP") == 0)
			{
				send(connectedControlSock , reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]) , 0);
				status2 = SetCurrentDirectoryA("..");
				if(status2){
					strcpy(str128 , "OK . CDUP succeeded .");
					send(connectedControlSock , str128 , 1 + strlen(str128) , 0);
				}
				else
				{
					sprintf(str128 , "Error : An Errror Occured with code <%lu>." , GetLastError() );
					send(connectedControlSock , str128 , 1 + strlen(str128) , 0);
				}

			}
			else if (strcmp(method, "QUIT") == 0)
			{
				
			}
			else if (strcmp(method, "RPRT") == 0) 
			{
				send(connectedControlSock , reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]) , 0);
				if(currentUser.reportAccess){
					historyStr=linkedList_print(&historyList);
					send(connectedControlSock , historyStr,strlen(historyStr) + 1 , 0);
					free((void *) historyStr);
				}
				else
				{
					sprintf(str128 , "%s" , "you don't have needed permissions !");
					send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
				}
			}
			else if (strcmp(method, "HELP") == 0)
			{
				send (connectedControlSock , reply[reply_code_index_find(214)] , 1 + strlen(reply[reply_code_index_find(214)]) , 0);
				if(sscanfRes == 1){
					send(connectedControlSock , helpMessage , 1 + strlen(helpMessage) , 0 );
				}
				else // argument contains a command name
				{
					strstrRes=strstr(helpMessage,argument);
					if(strstrRes == NULL){
						sprintf(str128 , "%s" , "help for this command not found !");
						send( connectedControlSock , str128 , 1 + strlen(str128) , 0 );
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
			else if (strcmp(method, "NOOP") == 0)
			{
				send(connectedControlSock, reply[reply_code_index_find(200)] , 1 + strlen(reply[reply_code_index_find(200)]),0);
			}

			// since here will implement

			else if (strcmp(method, "PORT") == 0) // sets client data socket(welcomming socket) port (format of port command: PORT h1,h2,h3,h4,p1,p2)
			{
			}
			else if (strcmp(method, "PASV") == 0) // 
			{	
			}
			else
			{
				send(connectedControlSock , reply[reply_code_index_find(502)] , 1 + strlen(reply[reply_code_index_find(502)]) , 0);
			}
			puts("receiving command ...");
			status1=recv(connectedControlSock , command , 256 , 0);
		}
		if(status1 == 0){
			printf("%s" , "connection closed by client...\n");
		}else if(status1<0){
			printf("receiving command failed with Error-Code: <%d>\n->-> connection to this client will close soon...\n", WSAGetLastError());
			closesocket(connectedControlSock);
		}
	}while(true);
}