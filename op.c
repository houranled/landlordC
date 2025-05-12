#include<stdio.h>
#include<stdlib.h>
#include"head.h"
#include<string.h>
#include <stdbool.h>


static int type_buffer();
static char point[]={'0','3','4','5','6','7','8','9','X','J','Q','K','A','2','w','W'};
static char *farr[]={"方片","梅花","红桃","黑桃"};
static char* type_arr[]={"弃权","单张","对子","王炸","骷髅","骷髅单","炸弹","骷髅对","炸带单","顺子","炸带一对","飞机不带","连对","飞机单","飞机带对"};
static char* sta_arr[2]={"农民","地主"};
static players player[3];//玩家
static games game;

/* 洗牌堆 */
static deck* deck_init(){
    int i,j;
    srand(time(0));
    deck *p_deck=(deck*)malloc(sizeof(deck));
    if(!p_deck){
            printf("分配内存失败\n");
            return NULL;
    }
    for(i=1;i<=54;i++){
            p_deck->arr[i]=rand()%54;
            for(j=1;j<i;j++){
                    if(p_deck->arr[i]==p_deck->arr[j]){
                            i--;
                            break;
                    }
            }
    }
    p_deck->top=54;
    return p_deck;
}

/* 初始化玩家(洗牌，id，身份 手牌,总数) */
static void player_init(){
    int i,j;
    for(j=0;j<3;j++){
            for(i=1;i<=20;i++){
                    player[j].handcard[i].number=100;
                    player[j].handcard[i].level =0;
            }
    }
    deck *p=deck_init();
    if(!p){
            printf("没有牌堆\n");
            return ;
    }
    int which=0;
    which=rand()%3;
    game.who=which;
    for(i=0;i<3;i++){
            player[i].id=i;
            if(i==which){//地主
                    player[i].status=1;
                    for(j=1;j<=20;j++){
                            player[i].handcard[j].number=p->arr[(p->top)--];
                    }
                    player[i].size=20;
            }
            else{//农民
                    player[i].status=0;
                    for(j=1;j<=17;j++){
                            player[i].handcard[j].number=p->arr[(p->top)--];
                    }
                    player[i].size=17;
            }
    }
    free(p);
    p=NULL;
}

/* 手牌信息补完 */
static void handcard_init(){
    int i,j;
    for(i=0;i<3;i++){
            for(j=1;j<=20;j++){
                            int number=player[i].handcard[j].number;
                            int *p_level=&(player[i].handcard[j].level);
                            char **pp_flower=&(player[i].handcard[j].flower);
                            char *p_point=&(player[i].handcard[j].point);
                    if(number>=0&&number<=51){
                            *p_level=number/4+1;
                            *p_point=point[number/4+1];
                            *pp_flower=farr[number%4];
                    }
                    else if(number==52){
                            *p_level=14;
                            *p_point='w';
                            *pp_flower="小王";
                    }
                    else if(number==53){
                            *p_level=15;
                            *p_point='W';
                            *pp_flower="大王";
                    }
                    else {
                            *p_level=0;
                            *p_point=' ';
                            *pp_flower="  ";
                    }
            }
    }
}

/* 打印当前玩家手牌 */
void print(){
    int i,j;
    for(i=0;i<3;i++){
            if (i!=game.who) continue;
            for(j=1;j<=player[i].size;j++){
                    //printf("======");
                    if(player[i].handcard[j].number == 100){
                            printf("    ");
                    }
                    else {
                            char *p_tmp=player[i].handcard[j].flower;
                            printf("%s ",p_tmp);
                    }
            }
            printf("\n");
            for(j=1;j<=player[i].size;j++){
                    if(player[i].handcard[j].number == 100){
                            printf(" ");
                    }
                    else {
                            printf(" %c   ",player[i].handcard[j].point);
                    }
            }
    }
    printf("\n");
    for(j=1;j<=player[game.who].size;j++){
            if(! (j>9))
                    printf("(%d)  ",j);
            else 
            printf("(%d) ",j);
    }
    printf("\n");
}

/* 游戏初始化 */
void game_init(){
    game.count=2;//弃权次数
    player_init();//洗牌 发牌
    handcard_init();//手牌信息补完
}

int fcmp(const void *pa,const void *pb){//升序
    return *(int*)pa-*(int*)pb;
}

static void rehandcard_sort(players *p_player,int* p_number){//真正的排序函数
    int *base=p_number;
    size_t nmemb=p_player->size;
    size_t size= sizeof(card);
    qsort(base,20,size,fcmp);
}

void handcard_sort(){//外壳排序函数
    rehandcard_sort(&player[0],&(player->handcard[1].number));
    rehandcard_sort(&player[1],&((player+1)->handcard[1].number));
    rehandcard_sort(&player[2],&((player+2)->handcard[1].number));
}

/* 询问是否出牌 */
static int require(){ //1表示出牌 0表示弃权
    if(game.type == 3 ){
        if(game.count != 2){
            printf("要不起!\n");
            return 0;
        }
        else 
            return 1;
    }
    if(game.count !=2){
        printf("出牌吗?(y表示出牌，n弃权):");
        char choice;
        scanf("%c",&choice);
        scanf("%*[^\n]");
        scanf("%*c");
        if(choice == 'n' || choice == 'N'){
            return 0;
        }
        else return 1;
    } else {
        printf("继续出牌\n");
        return 1;
    }
}

buffers buffer={0};//出牌缓冲区

/* 清空出牌缓冲区 */
static void reset(){
    int a;
    for(a=0;a<16;a++) buffer.arr[a]=0;

    for(a=0;a<20;a++) buffer.brr[a]=0;

    buffer.sum=0;
}

/* 放牌进入缓冲区 */
static void buffer_put(){
    char intput[40]={0};//把字符串转换为数字
    int brr[20]={0};
    int i=0;
    int j;
    int sum;
    int flag=0;
    while(1){
        reset();
    sig:	
        printf("请输入要放的牌...:");
        fgets(intput,40,stdin);
        if(strlen(intput)==39&&intput[38]!='\n'){
            scanf("%*[^\n]");
            scanf("%*c");
        }
        for(j=0,i=0,sum=0;i<strlen(intput);i++){//记录出牌下标
            if(intput[i]>='0'&&intput[i]<='9'){
                sum=sum*10+(intput[i]-'0');	
                flag=1;
            }
            else {
                if(flag)
                brr[j] = sum;
                sum=0;
                j++;
                flag=0;
            }
        }
        int k;
        printf("要出: ");
        for(k=0;brr[k];k++) printf("%d ",brr[k]);

        printf("号牌\n");
        
        int who = game.who;
        players* p_player=&(player[who]);

        int index;
        for(i=0;brr[i];i++){//记录出的牌是什么
            index=brr[i];
            if(index>(p_player->size) || index<=0 ){//输入的字符串范围有误
                printf("输入内容超出范围，重新输入\n");
                goto sig;
            }
            else{
                int level=p_player->handcard[index].level;
                 ++(buffer.arr[level]);
                buffer.brr[i] =brr[i];
             }
        }
        for(i=1;i<=15;i++) buffer.sum+=buffer.arr[i];//记录出了多少张牌

        char aff= 'N';
        int type = type_buffer();
        if(type != -1)
            printf("要出的类型是:%s\n\n",type_arr[type]);
        else {
            printf("不存在此类型的牌\n\n");
            reset();
            return;
        }
        printf("确定要这样出吗?(确定输入y,否则按其它)");
        scanf("%c",&aff);
        scanf("%*[^\n]");
        scanf("%*c");
        if(aff == 'y' || aff =='Y') break;                    
    }
}

static void turnstart(){
    char u;
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n==============================================================小浩斗地主======================================================\n\n\n\n\n");
    printf("轮到下一家");
    scanf("%c",&u);
    int i;
    printf("\n\n\n\n\n\n\n牌面张数为%d张\n",game.sum);
    printf("牌面类型为:   %s%c\n",type_arr[game.type],point[game.level]);
    printf("=============================================================%s的回合==========================================================\n\n",sta_arr[player[game.who].status]);
    printf("现在轮到玩家%d",game.who+1);
    printf(" 	       				 玩家1(%s)手牌%d  ",sta_arr[player[0].status],player[0].size);if(game.who==0) printf("<=====\n");else printf("\n");
    printf("                                                 玩家2(%s)手牌%d  ",sta_arr[player[1].status],player[1].size);if(game.who == 1) printf("<=====\n"); else printf("\n");
    printf("                                                 玩家3(%s)手牌%d  ",sta_arr[player[2].status],player[2].size);if(game.who == 2) printf("<=====\n"); else printf("\n");
}

/* 判断是否连续 */
static int continuum(int num,int total){
    int i,count=0;
    int flag=0;//有值则标记为1
    int sig=0;//从 有到无 改标记为1
    for(i=1;i<=15;i++){
        if(buffer.arr[i]==num){
            if(sig) return 0;//非连续

            count++;
            if(count==total) return 1;//连续

            flag=1;
        }else {
            if (flag) sig=1;
         }
    }
}

static bool if_repeat_card(int *arr, int totalCount) {
    int i, j;
    for (i=0;arr[i]!=0 && i+1<totalCount;i++) {
        for (j=i+1; arr[j]!=0; j++) {
            if (arr[i]==arr[j]) {
                return true;
            }
        }
    } 
    return false;
}

/* 获取缓冲区牌类类型 */
static int type_buffer(){
    if (if_repeat_card(buffer.brr, sizeof(buffer.brr)) == true) {
        return -1;
    }
    
    int i, one=0,pair=0,triple=0,quattuor=0,zero=0;
    for(i=1;i<=15;i++){//统计单张，对子，三同，四同各有多少
        if(buffer.arr[i] == 1)
                one++;
        else if(buffer.arr[i] == 2)
                pair ++;
        else if(buffer.arr[i] == 3)
                triple ++;
        else if(buffer.arr[i] == 4)
                quattuor ++;
        else zero++;
    }
    //printf("单%d 对%d 三%d 四%d 零%d,sum%d===\n",one,pair,triple,quattuor,zero,buffer.sum);
    if(!(buffer.sum)){
        return -1;//非法
    }
    else if(buffer.sum<=5){//1~5
        if(one == 1 && !pair && !triple && !quattuor)//单张
            return 1;
        else if(pair == 1 && !one && !triple && !quattuor)//对子
            return 2;
        else if(one == 2 &&buffer.arr[14]&&buffer.arr[15])//王炸
            return 3;
        else if(triple == 1 && !one && !pair && !quattuor) //骷髅
            return 4;
        else if(one ==1 && !pair && triple == 1 && !quattuor )//骷髅单
            return 5;
        else if(!one && !pair && !triple && quattuor == 1)//炸
            return 6;
        else if(!one && pair == 1 && triple == 1 && !quattuor)//骷髅对		
            return 7;
        else if(one == 1 && !pair && !triple && !quattuor){//炸带单 
            return 8;
        }
        else if(!pair && !triple && !quattuor && (!buffer.arr[14] && !buffer.arr[15])&& buffer.sum == 5){//顺子
            if( continuum(1,one))//所有1连续
                return 9;
            else {
                return -1;
            }
        }
        else  return -1; 
    }
    else if(buffer.sum>=6){
        if((!one) && (pair == 1) && (!triple) && (quattuor == 1) )//炸带对
            return 10;	
        else if(!one && !pair && !quattuor){//只有2个以上个三张相同 飞机不带
            if(continuum(3,triple))//所有3连续
                return  11;
            else return -1;
        }
        else if(!one && !triple && !quattuor){//连对
            if(continuum(2,pair))
                return 12;
            else return -1;
        }
        else if(buffer.sum == 4*triple){//飞机单
            if(continuum(3,triple))
                return 13;
            else return -1;	
        }
        else if((buffer.sum == 5*triple) && (triple == pair)){//飞机对
            if(continuum(3,triple))
                return 14;
            else return -1;
        }
        else if(!pair && !triple && !quattuor &&(!buffer.arr[14] && !buffer.arr[15])){
            if(continuum(1,one))
                return 9;
            else return -1;
        }
        else return -1;
    }
}

/* 最大下标 */
static int maxindex(int count){
    int i;
    for (i=15;i>=1;i--){
        if(buffer.arr[i] == count) return i;                
    }
}

/* 获取缓冲区牌类等级 */
static int level_buffer(int type){
    switch(type){
            case 1:
                return maxindex(1);
            break;
            case 2:
                return maxindex(2);
            break;
            case 3:
                return 15;
            break;
            case 4:
                return maxindex(3);
            break;
            case 5:
                return maxindex(3);
            break;
            case 6:
                return maxindex(4);
            break;
            case 7:
                return maxindex(3);
            break;
            case 8:
                return maxindex(4);
            break;
            case 9:
                return maxindex(1);
            break;
            case 10:
                return maxindex(4);
            break;
            case 11:
                return maxindex(3);

            break;
            case 12:
                return maxindex(2);
            break;
            case 13:
                return maxindex(3);
            break;
            case 14:
                return maxindex(3);
            break;
    }
}

/* 消减手牌 */
static void annealation(){
    int i=1;
    int j=0;
    int who=game.who;
    for(i = 1,j=0;buffer.brr[j];i++,j++){
    int index = buffer.brr[j];
    player[who].handcard[index].number = 100;
    player[who].size = player[who].size - 1;
    }
    game.sum=buffer.sum;
    game.count=0;
    handcard_sort();
    //printf("出牌成功\n");
}

/* 回合进行中 */  //这个模块有很大的修改空间 例如return 改为break...
void turning(){ 
    turnstart();							/*  开始阶段  */
    handcard_sort();	
    print();			
    int level= 0;
    while(1){
        if(!require()){
            printf("\n\n\n\n\n\n\n\n不要\n");
            game.count++;
            if(game.count == 2){
                game.type=0;
                game.level=0;
                game.sum=0;
            }
            return ;						/*  开始阶段  */
        }									
        buffer_put();						/*  出牌阶段在这函数判断是否弃权比较好  */

        int type = type_buffer();
        int level=level_buffer(type);
        if(type == -1){
            printf("牌类非法!-----\n");
            continue;
        }
        if(type == 3){//王炸
            printf("\n\n\n\n\n王炸!!\n");
            annealation();
            game.type=3;
            game.level=MAXLEVEL;
            return ;
        }
        else if(type == 6){//炸弹
            if(game.type != 6){
                printf("\n\n\n\n炸弹\n");
                annealation();
                game.type = 6;
                game.level = level_buffer(type);
                return; 
            }
            else {
                if(level > game.level){
                    printf("\n\n\n\n\n压死\n");
                    annealation();//消减手牌
                    game.type = 6;
                    game.level = level;
                    return;
                }
                else if(level < game.level||level == game.level){
                    printf("牌的等级不够大\n");
                    continue;
                }
            }   
    }
        else if(game.count == 2 ){//两家弃权
            annealation();
            game.type = type;
            game.level = level;
            return ;
        }
        else {//除了炸弹  两家弃权 王炸  非法 以外的合理牌类
            if(type != game.type){ //不对应
                printf("类型不对应\n");
                continue;
            }
            else { //对应
                if(buffer.sum != game.sum){
                    printf("数量不对应\n");
                    continue;
                }
                if(level < game.level|| level==game.level){
                    printf("牌的等级不够大\n");
                    continue;
                }
                else if(level > game.level){
                    printf("\n\n\n\n\n压死\n");
                    annealation();
                    game.type = type;
                    game.level = level;
                    return ;
                }
            }
        }
    } 
}

/* 0 1 2 3判断是否是谁胜利 0表示没有 1表示玩家一 */
int win(){
    int now = game.who;
    if(!player[now].size)
        return now;
    else return 0;
}

/* 切换当前玩家为下家 */
void turn_switch(){
    int who = game.who;
    who++;
    game.who = who%3;  
}
