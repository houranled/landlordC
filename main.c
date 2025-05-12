#include<stdio.h>
#include"head.h"
int main(){
	while(1){
		int which;
		game_init();//初始化游戏
		while(1){
			turning();//回合进行中
			printf("\n\n\n");
			if(which=win())//是否产生胜者
				break;
			turn_switch();//切换出牌方
	}
		printf("胜负已出！胜利者是玩家%d\n",which+1);
		printf("是否重新游戏?(y为继续，其它退出程序):");
		char choice;
		scanf("%c",&choice);
		scanf("%*[^\n]");
		scanf("%*c");
		if(choice == 'y'||choice =='Y')
			continue;
		break;
	}
	printf("谢谢试玩\n");
}
