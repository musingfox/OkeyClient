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

struct color{
	string red=(const char*)"red",black=(const char*)"black",yellow=(const char*)"yellow",blue=(const char*)"blue";
};
map<string,int > card;
char tmp[2048];

void init(char* S,string key1 , string value1 , string key2 , string key3 , string value2){
	const char* k1 = key1.c_str() , *v1 = value1.c_str() , *k2 = key2.c_str() , *v2 = value2.c_str() , *k3 = key3.c_str();
	sprintf(S,"{\"%s\":\"%s\",\"%s\":{\"%s\":\"%s\"}}",k1,v1,k2,k3,v2);
//	sprintf(S,"{\"%s\": {\"%s\": \"%s\"}, \"%s\": \"%s\"}",k2,k3,v2,k1,v1);
}
void get(char* S,string key1,string value1,string key2,string value2){
	const char *k1=key1.c_str() , *v1=value1.c_str(), *k2=key2.c_str(), *v2=value2.c_str();
	sprintf(S,"{\"%s\": \"%s\",\"%s\": \"%s\"}",k1,v1,k2,v2);
}
void takecard(char* S, string key1, string value1, string key2, string value2, string key3, string value3){
	const char *k1=key1.c_str(), *v1=value1.c_str(), *k2=key2.c_str(), *v2=value2.c_str(), *k3=key3.c_str(), *v3=value3.c_str();
	sprintf(S,"{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}",k1,v1,k2,v2,k3,v3);
}

string getcard(char* S){
	string tmp="";printf("%s\n",S);
	int st = 0;
	for( st = 0 ; st < strlen(S) ; st++ ){
		if( S[st] == '\"' && S[st+1] == 'c' ) break;
	}
	
	for( ; st < strlen(S)-1 ; st++ ){
		tmp += S[st];
	}
	return tmp;
}

void throwcard(char* S,string key1, string value1, string key2, string value2, string c){
	const char *k1=key1.c_str(), *v1=value1.c_str(), *k2=key2.c_str(), *v2=value2.c_str(), *card=c.c_str();
	sprintf(S,"{\"%s\":\"%s\", \"%s\":\"%s\",%s}",k1,v1,k2,v2,card);
}

int main(int argc, char* argv[]){
	int soc , s , rec , con , state = 0;
	char *IP = argv[1] , *port = argv[2];
	char S[2048] , buf[2048] ;
	string cards;
	
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
		memset(buf,0,sizeof(buf) );
		switch(state){
			case 0:
				init(S,"command","game","setting","log","verbose");
				state = 1;
				break;
			case 1:
				get(S,"player","1","action","hand");
				state = 2;
				break;
			case 2:
				cards = getcard(tmp);
				takecard(S,"player","1","action","take","from","deck");
				state = 3;
				break;
			case 3:
				throwcard(S,"player","1","action","throw",cards);
				state = 4;
				break;
			case 4:
				get(S,"player","2","action","hand");
				state = 5;
				break;
			case 5:
				cards = getcard(tmp);
				takecard(S,"player","2","action","take","from","deck");
				state = 6;
				break;
			case 6:
				throwcard(S,"player","2","action","throw",cards);
				state = 7;
				break;
				
		}
		scanf("%*s");
		printf("%s~~~~~~~~~~~~~~\n",S);
		s = send(soc , S , sizeof(char)*strlen(S) , 0 );
		rec = recv(soc , buf , sizeof(buf) , 0 );
		printf("%s!!!!!!!!!!!!!!\n",buf);sprintf(tmp,"%s",buf);
	//	scanf("%*s");
				
	}
	//	close(soc);
	return 0;
}
