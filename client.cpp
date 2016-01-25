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
#include <utility>
#include <bits/stdc++.h>
using namespace std;

#define red 0
#define black 1
#define yellow 2
#define blue 3

const char* Color[] = {"red", "black", "yellow" , "blue" };

pair<string,int> top[5];
int hand[4][14];
char tmp[2048];



void next(char* &ptr){
	ptr = strtok(NULL," \",:[]{}");
}
void buildmap(string tmp,int num){
	const char* color = tmp.c_str();
	if( !strcmp(color,Color[red]) )
		hand[red][num]++;
	else if( !strcmp(color,Color[black]) )
		hand[black][num]++;
	else if( !strcmp(color,Color[yellow]) )
		hand[yellow][num]++;
	else if( !strcmp(color,Color[blue]) )
		hand[blue][num]++;
}
char* getcard(char* S){
	string color;
	int num;
	char *ptr=NULL;
	
	ptr = strtok(S," \",:[]{}");
	while( ptr != NULL ){
		printf("%s$$$$$$$\n",ptr);
		if( !strcmp(ptr,"hand") ){
			next(ptr);
			for( int i = 0 ; i < 24 ; i++,next(ptr) ){
				if( !strcmp(ptr,"color") ){
					next(ptr);
					color = ptr;
					next(ptr),next(ptr);
					num = atoi(ptr);
					buildmap(color,num);
				}
			}
		}
		else if ( !strcmp(ptr,"top") ){
			next(ptr);
			if( !strcmp(ptr,"player1") ){
				next(ptr),next(ptr);
				color = ptr;
				next(ptr),next(ptr);
				num = atoi(ptr);
				pair<string,int> tmp(color,num);
				top[1] = tmp;
			}
			else if( !strcmp(ptr,"player2") ){
				next(ptr),next(ptr);
				color = ptr;
				next(ptr),next(ptr);
				num = atoi(ptr);
				pair<string,int> tmp(color,num);
				top[2] = tmp;
			}
			else if( !strcmp(ptr,"player3") ){
				next(ptr),next(ptr);
				color = ptr;
				next(ptr),next(ptr);
				num = atoi(ptr);
				pair<string,int> tmp(color,num);
				top[3] = tmp;
			}
			else if( !strcmp(ptr,"player4") ){
				next(ptr),next(ptr);
				color = ptr;
				next(ptr),next(ptr);
				num = atoi(ptr);
				pair<string,int> tmp(color,num);
				top[4] = tmp;
			}
		}
		
		next(ptr);
	}
	
	return tmp;
}
void init(char* S,const char* k1 ,const char* v1 ,const char* k2 ,const char* k3 ,const char* v2){
	sprintf(S,"{\"%s\":\"%s\",\"%s\":{\"%s\":\"%s\"}}",k1,v1,k2,k3,v2);
}
void get(char* S,const char* k1,const char* v1,const char* k2,const char* v2){
	sprintf(S,"{\"%s\": \"%s\",\"%s\": \"%s\"}",k1,v1,k2,v2);
}
void takecard(char* S,const char* k1,const char* v1,const char*k2,const char*v2,const char* k3,const char* v3){
	sprintf(S,"{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}",k1,v1,k2,v2,k3,v3);
}
void throwcard(char* S,const char* k1,const char* v1,const char* k2,const char* v2,const char* card){
	sprintf(S,"{\"%s\":\"%s\", \"%s\":\"%s\",%s}",k1,v1,k2,v2,card);
}
int main(int argc, char* argv[]){
	int soc , s , rec , con , state = 0;
	char *IP = argv[1] , *port = argv[2];
	char S[2048] , buf[2048] ;
	char* cards;

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
	
	bool first = true;
	int player = 0;
	state = 0;
	while( 1 ){
		memset(S,0,sizeof(S) );
		memset(buf,0,sizeof(buf) );
		if( first ){
			init(S,"command","game","setting","log","separate");
			first = false;
		}
		else {
			const char* now = to_string(player+1).c_str();
			switch(state){
				case 0:
					get(S,"player",now,"action","hand");
					state = 1;
					break;
				case 1:
					memset(hand,0,sizeof(hand) );
					cards = getcard(tmp);
					takecard(S,"player",now,"action","take","from","deck");
					state = 2;
					break;
				case 2:
					throwcard(S,"player",now,"action","throw",cards);
					player = (player+1)%4;
					state = 0;
					break;
			}
		}
		
		printf("%s~~~~~~~~~~~~~~\n",S);
		s = send(soc , S , sizeof(char)*strlen(S) , 0 );
		rec = recv(soc , buf , sizeof(buf) , 0 );
		printf("%s!!!!!!!!!!!!!!\n",buf);sprintf(tmp,"%s",buf);
		scanf("%*s");
				
	}
	//	close(soc);
	return 0;
}
