#ifndef 		__HEAD_H__
#define			__HEAD_H__
#define 		MAXLEVEL  15
typedef struct CARD{
	int number;
	int level;
	char *flower;
	char point;
}card;//卡

typedef struct DECK{
	int top;
	int arr[55];
}deck;//牌堆

typedef struct PLAYERS{
	int id;
	int status;
	card handcard[21];
	int size;
}players;//玩家

typedef struct GAMES{
	int type;
	int level;
	int sum;
	int who;
	int count;
	int arr[16];
}games;//桌面

typedef struct BUFFERS{
	int arr[16];
	int brr[20];
	int sum;
}buffers;//出牌缓冲区
/*--------------------------------*/
void game_init();
void turning(); 
void handcard_sort();
void print();
int win();
void turn_switch();
#endif
