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

pair<int,int> top[5];
int hand[4][14]; // color , num
char tmp[2048];
vector< pair<int,int> > used[10]; // color , num
vector< pair<int,int> > useful[10];
vector< pair<int,int> > trash;
int Sum;


void show(int A[4][14]){
	printf("1 2 3 4 5 6 7 8 910111213\n");
	for( int i = 0 ; i < 4 ; i++ ){
		for( int j = 1 ; j < 14 ; j++ )
			printf("%d ",A[i][j]);
		puts("");
	}
	puts("!!!!!!!!!!!!!!!!");
}
void next(char* &ptr);
pair<int,int> buildtop(string tmp,int num);

string makecards(const char* color , int num){
	string n = to_string(num).c_str();
	string tmp = (string)"{" + "\"color\"" + ": " + "\"" + color + "\"" + " ,"+ "\"number\"" + ": " + n + "}";
	return tmp;
}

bool Iswin(char *S){
	char *ptr = strtok(S," \",:[]{}");
	bool win = false;
	while( ptr != NULL ){
		if( !strtok(ptr,"win") ){
			next(ptr);
			win = atoi(ptr);
		}
		next(ptr);
	}
	return win;
}
pair<int,int> getnew(char* S);

const char* Istake(int hand[4][14],int player,bool& istake);

const char* AI(int hand[4][14],pair<int,int>& newcard);

void getcard(char* S);
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
	const char* cards;

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
	
	bool first = true , istake = false, iswin = false;;
	int player = 0;
	pair<int,int> newcard;
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
					getcard(tmp);
					istake = false;
					cards = Istake(hand,(player+3)%4,istake);
					if( istake )
						takecard(S,"player",now,"action","take","from","discard");
					else 
						takecard(S,"player",now,"action","take","from","deck");
					state = 2;
					break;
				case 2:
					if( istake ){
						cards = Istake(hand,(player+3)%4,istake);
					}
					else{
						newcard = getnew(tmp);
					//	cout << newcard.first << "! !" << newcard.second << endl;
						cards = AI(hand,newcard);puts("AIAIAIAI");
					}
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
		if( state == 2 && Iswin(buf) ){
			break;
		}
	//	scanf("%*s");
				
	}
		close(soc);
	return 0;
}
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
pair<int,int> buildtop(string tmp,int num){
	const char* color = tmp.c_str();
	if( !strcmp(color,Color[red]) )
		return pair<int,int>(red,num);
	else if( !strcmp(color,Color[black]) )
		return pair<int,int>(black,num);
	else if( !strcmp(color,Color[yellow]) )
		return pair<int,int>(yellow,num);
	else if( !strcmp(color,Color[blue]) )
		return pair<int,int>(blue,num);
	return pair<int,int>(0,0);
}

void getcard(char* S){
	string color;
	int num;
	char *ptr=NULL;
	
	ptr = strtok(S," \",:[]{}");
	while( ptr != NULL ){
//		printf("%s$$$$$$$\n",ptr);
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
			for( int i = 0 ; i < 4 ; i++ ){
				if( !strcmp(ptr,"player1") ){
					next(ptr),next(ptr);
					color = ptr;
					next(ptr),next(ptr);
					num = atoi(ptr);
					top[0] = buildtop(color,num);
				}
				else if( !strcmp(ptr,"player2") ){
					next(ptr),next(ptr);
					color = ptr;
					next(ptr),next(ptr);
					num = atoi(ptr);
					top[1] = buildtop(color,num);
				}
				else if( !strcmp(ptr,"player3") ){
					next(ptr),next(ptr);
					color = ptr;
					next(ptr),next(ptr);
					num = atoi(ptr);
					top[2] = buildtop(color,num);
				}
				else if( !strcmp(ptr,"player4") ){
					next(ptr),next(ptr);
					color = ptr;
					next(ptr),next(ptr);
					num = atoi(ptr);
					top[3] = buildtop(color,num);
				}
				if( i < 3 ) next(ptr); 
			}
		}
		next(ptr);
	}
	
}
pair<int,int> getnew(char* S){
	cout << S << "!!!!"<<endl;
	char *ptr = strtok(S," \",:[]{}");
	while( ptr != NULL ){
		if( !strcmp(ptr,"color") ){
			next(ptr);
			string color = ptr;
			next(ptr),next(ptr);
			int num = atoi(ptr);
			return buildtop(color,num);
		}
		next(ptr);
	}
	
}

const char* Istake(int hand[4][14],int player,bool& istake){
	int flg = 0 , usedsz = 0 , usefulsz = 0 , st , A[4][14] , total = 0 , cardn = 0 , tmpcolor , tmpnum , before;
	for( int i = 0 ; i < 4 ; i++ )
		for( int j = 0 ; j < 14 ; j++ )
			A[i][j] = hand[i][j];
	tmpcolor = top[player].first , tmpnum = top[player].second;
	before = A[tmpcolor][tmpnum]; A[tmpcolor][tmpnum]++;
	bool wait = false;
	cout << tmpcolor << " " << tmpnum << endl;
	for( int test = 0 ; test < 2 ; test++ ){
		
		//init
		
		for( int i = 0 ; i < 10 ; i++ )
			used[i].clear() , useful[i].clear();
		trash.clear();
		Sum = usedsz = usefulsz = 0;
//		show(A);
		//get used card
		for( int i = 1 ; i < 14 ; i++ ){
			if( usedsz == 4 ){
				wait = true;break;
			}
			flg = 0;
			for( int j = 0 ; j < 4 ; j++ ){
				if( A[j][i] ){
					flg++;
				}
			}
			if( flg >= 3 ){
				for( int j = 0 ; j < 4 ; j++ ){
					if( A[j][i] ){
						A[j][i]--;Sum++;
						used[usedsz].push_back(pair<int,int>(j,i) );
					}
				}
				usedsz++;
			}
		}
		flg = 0;
		for( int i = 0 ; i < 4 ; i++ ){
			for( int j = 1 ; j < 14 ; j++ ){
				if( wait || usedsz == 4 ){
					wait = true; break;
				}
				st = j;flg = 0;
				if( A[i][j] ){
					for(  ; j < 14 ; j++ ){
						if( A[i][j] ){
							flg++;
						}
						else break;
					}
					if( flg >= 3 ){
						for( int k = st ; k < j ; k++ ){
							A[i][k]--;Sum++;
							used[usedsz].push_back(pair<int,int>(i,k) );
						}
						usedsz++;
					}
				}
			}
		}
		if( usedsz < 4 ){
			//get useful card
			usefulsz = 0;
			for( int i = 1 ; i < 14 ; i++ ){
				
				flg = 0;
				for( int j = 0 ; j < 4 ; j++ ){
					if( A[j][i] ){
						flg++;
					}
				}
				if( flg >= 2 ){
					for( int j = 0 ; j < 4 ; j++ ){
						if( A[j][i] ){
							A[j][i]--;Sum++;
							useful[usefulsz].push_back(pair<int,int>(j,i) );
						}
					}
					usefulsz++;
				}
			}
			st = 0;
			for( int i = 0 ; i < 4 ; i++ ){
				for( int j = 1 ; j < 14 ; j++ ){
					st = j;flg = 0;
					if( A[i][j] ){
						for(  ; j < 14 ; j++ ){
							if( A[i][j] ){
								flg++;
							}
							else break;
						}
						if( flg >= 2 ){
							for( int k = st ; k < j ; k++ ){
								A[i][k]--;Sum++;
								useful[usefulsz].push_back(pair<int,int>(i,k) );
							}
							usefulsz++;
						}
					}
				}
			}
		}
		
		if( istake ) break;
		if( !istake && !test && A[tmpcolor][tmpnum] - before < 1 ){
			istake = true;
		}
		if( !test )
			for( int i = 0 ; i < 4 ; i++ )
				for( int j = 1 ; j < 14 ; j++ )
					A[i][j] = hand[i][j];
		
	}
	
	//get trash
	for( int i = 0 ; i < 4 ; i++ )
		for( int j = 1 ; j < 14 ; j++ )
			while( A[i][j]-- )
				trash.push_back(pair<int,int>(i,j) );
	
	//build new hand
	string cards;
	total = cardn = 0;
	for( int i = 0 ; i < usedsz ; i++,total++, cards+= ", " ){
		for( int j = 0 ; j < used[i].size() ; j++, total++,cardn++, cards+=", " )
			cards += makecards(Color[used[i][j].first],used[i][j].second);
		cards += makecards("empty",-1);
	}
	for( int i = 0 ; i < usefulsz ; i++,total++, cards += ", " ){
		for( int j = 0 ; j < useful[i].size() ; j++, total++ , cardn++ , cards += ", " )
			cards += makecards(Color[useful[i][j].first],useful[i][j].second);
		cards += makecards("empty",-1);
	}
	for( int i = 0 ; cardn < 14 && i < trash.size() ; i++,total++, cardn++, cards += ", " ){
		cards += makecards(Color[trash[i].first],trash[i].second);
		
	}
	for( int i = total ; i < 23 ; i++, cards += ", " )
		cards += makecards("empty",-1);
	cards += makecards("empty",-1);
	cards = (string)"\"cards\"" + ": " + "[" + cards + "]";
	return cards.c_str();
}
const char* AI(int hand[4][14],pair<int,int>& newcard){
	
	int flg = 0 , usedsz = 0 , usefulsz = 0 , st , A[4][14] , total = 0 , cardn = 0 ;
	for( int i = 0 ; i < 4 ; i++ )
		for( int j = 0 ; j < 14 ; j++ )
			A[i][j] = hand[i][j];
	A[newcard.first][newcard.second]++;//put new card
	
	bool wait = false;
	//init
	
	for( int i = 0 ; i < 10 ; i++ )
		used[i].clear() , useful[i].clear();
	trash.clear();
	Sum = usedsz = usefulsz = 0;
//		show(A);
	//get used card
	for( int i = 1 ; i < 14 ; i++ ){
		if( usedsz == 4 ){
			wait = true; break;
		}
		flg = 0;
		for( int j = 0 ; j < 4 ; j++ ){
			if( A[j][i] ){
				flg++;
			}
		}
		if( flg >= 3 ){
			for( int j = 0 ; j < 4 ; j++ ){
				if( A[j][i] ){
					A[j][i]--;Sum++;
					used[usedsz].push_back(pair<int,int>(j,i) );
				}
			}
			usedsz++;
		}
	}
	
	flg = 0;
	for( int i = 0 ; i < 4 ; i++ ){
		for( int j = 1 ; j < 14 ; j++ ){
			if( wait || usedsz == 4 ){
				wait = true; break;
			}
			st = j;flg = 0;
			if( A[i][j] ){
				for(  ; j < 14 ; j++ ){
					if( A[i][j] ){
						flg++;
					}
					else break;
				}
				if( flg >= 3 ){
					for( int k = st ; k < j ; k++ ){
						A[i][k]--;Sum++;
						used[usedsz].push_back(pair<int,int>(i,k) );
					}
					usedsz++;
				}
			}
		}
	}
	if( usedsz < 4 ){
		//get useful card
		usefulsz = 0;
		for( int i = 1 ; i < 14 ; i++ ){
			flg = 0;
			for( int j = 0 ; j < 4 ; j++ ){
				if( A[j][i] ){
					flg++;
				}
			}
			if( flg >= 2 ){
				for( int j = 0 ; j < 4 ; j++ ){
					if( A[j][i] ){
						A[j][i]--;Sum++;
						useful[usefulsz].push_back(pair<int,int>(j,i) );
					}
				}
				usefulsz++;
			}
		}
		st = 0;
		for( int i = 0 ; i < 4 ; i++ ){
			for( int j = 1 ; j < 14 ; j++ ){
				st = j;flg = 0;
				if( A[i][j] ){
					for(  ; j < 14 ; j++ ){
						if( A[i][j] ){
							flg++;
						}
						else break;
					}
					if( flg >= 2 ){
						for( int k = st ; k < j ; k++ ){
							A[i][k]--;Sum++;
							useful[usefulsz].push_back(pair<int,int>(i,k) );
						}
						usefulsz++;
					}
				}
			}
		}
	}
	
	//get trash
	for( int i = 0 ; i < 4 ; i++ )
		for( int j = 1 ; j < 14 ; j++ )
			while( A[i][j]-- )
				trash.push_back(pair<int,int>(i,j) );
	
	
	//build new hand
	string cards;
	total = cardn = 0;
	for( int i = 0 ; i < usedsz ; i++,total++, cards+= ", " ){
		for( int j = 0 ; j < used[i].size() ; j++, total++,cardn++, cards+=", " )
			cards += makecards(Color[used[i][j].first],used[i][j].second);
		cards += makecards("empty",-1);
	}
	for( int i = 0 ; i < usefulsz ; i++,total++, cards += ", " ){
		for( int j = 0 ; j < useful[i].size() ; j++, total++ , cardn++ , cards += ", " )
			cards += makecards(Color[useful[i][j].first],useful[i][j].second);
		cards += makecards("empty",-1);
	}
	for( int i = 0 ; cardn < 14 && i < trash.size() ; i++,total++, cardn++, cards += ", " ){
		cards += makecards(Color[trash[i].first],trash[i].second);
		
	}
	for( int i = total ; i < 23 ; i++, cards += ", " )
		cards += makecards("empty",-1);
	cards += makecards("empty",-1);
	cards = (string)"\"cards\"" + ": " + "[" + cards + "]";
	return cards.c_str();
}
