#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <typeinfo>
#include <map>
#include <string>
using namespace std;

void init(char* S,string key1 , string value1 , string key2 , string key3 , string value2){
	const char* k1 = key1.c_str() , *v1 = value1.c_str() , *k2 = key2.c_str() , *v2 = value2.c_str() , *k3 = key3.c_str();
	sprintf(S,"{\"%s\":\"%s\",\"%s\":{\"%s\":\"%s\"}}",k1,v1,k2,k3,v2);
//	sprintf(S,"{\"%s\": {\"%s\": \"%s\"}, \"%s\": \"%s\"}",k2,k3,v2,k1,v1);
}
void getcard(char* S,string key1,string value1,string key2,string value2){
	const char *k1=key1.c_str() , *v1=value1.c_str(), *k2=key2.c_str(), *v2=value2.c_str();
	sprintf(S,"{\"%s\": \"%s\",\"%s\": \"%s\"}",k1,v1,k2,v2);
}


int main(int argc, char* argv[]){
	int soc , s , rec , con , state = 0;
	char *IP = argv[1] , *port = argv[2];
	char S[2048] , buf[2048] ;

	sockaddr_in server , client;
	socklen_t len = sizeof( sockaddr_in );
	memset( &server , 0 , sizeof (server) );

	server.sin_family = AF_INET;
	server.sin_port = htons( atoi((const char*)port) );
	server.sin_addr.s_addr = inet_addr( IP );
	
	soc = socket( AF_INET , SOCK_STREAM , 0 );
	if( soc < 0 )
		printf("error opening socket\n");
	con = connect(soc,(sockaddr*)&server, len );
	if( con < 0 )
		printf("error connecting\n");
	
	printf("#set up\n");
	printf("TCP Server listening on %s:%s\n\n",IP,port);
	
	state = 0;
	while( 1 ){
		memset(S,0,sizeof(S) );
		switch(state){
			case 0:
				init(S,"command","game","setting","log","verbose");
				state = 1;
				break;
			case 1:
				getcard(S,"player","1","action","hand");
				state = 2;
				break;
		}
		printf("%s~~~~~\n",S);
		s = send(soc , S , sizeof(char)*strlen(S) , 0 );
		rec = recv(soc , buf , sizeof(buf) , 0 );
		printf("%s~!!\n",buf);
	//	scanf("%*s");
				
	}
	//	close(soc);
	return 0;
}
