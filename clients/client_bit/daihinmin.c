/*daifugo*/ 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

#ifndef DAIHINMIN_H
#define DAIHINMIN_H
#include "daihinmin.h"
#endif
extern const int g_logging;

void getState(int cards[8][15]){
  /*
    カードテーブルから得られる情報を読み込む
    引数は手札のカードテーブル
    情報は広域変数stateに格納される
  */
  int i;
  //状態
  if(cards[5][4]>0) state.onset=1; //場にカードがないとき 1
  else              state.onset=0;
  if(cards[5][6]>0) state.rev=1;   //革命状態の時 1 
  else              state.rev=0;
  if(cards[5][5]>0) state.b11=1;   //11バック時 1 未使用
  else              state.b11=0;
  if(cards[5][7]>0) state.lock=1;  //しばり時 1
  else              state.lock=0;

  if(state.onset==1){   //新たな場のとき札の情報をリセット
    state.qty=0;
    state.ord=0;
    state.lock=0;
    for(i=0;i<5;i++)state.suit[i]=0;
  }
  
  for(i=0;i<5;i++) state.player_qty[i]=cards[6][i];   //手持ちのカード
  for(i=0;i<5;i++) state.player_rank[i]=cards[6][5+i];//各プレーヤのランク
  for(i=0;i<5;i++) state.seat[i]=cards[6][10+i];      //誰がどのシートに座っているか
                                                      //シートiにプレーヤ STATE.SEAT[I]が座っている

  if(cards[4][1]==2) state.joker=1;     //Jokerがある時 1
  else               state.joker=0;

  
}

void getField(int cards[8][15]){
  /*
    場に出たカードの情報を得る。
    引数は場に出たカードのテーブル
    情報は広域変数stateに格納される
  */
  int i,j,count=0;
  int found_joker = 0;
  i=j=0;
  
  //カードのある位置を探す
  while(j<15&&cards[i][j]==0){
    state.suit[i]=0;
    i++;
    if(i==4){
      j++;
      i=0;
    }
  }

  //見つけたカードがジョーカーならば、found_joker=1。
  if (cards[i][j] == 2){
    found_joker = 1;
  }

  //階段が否か
  if(j<14){
    if(cards[i][j+1]>0) state.sequence=1;
    else state.sequence=0;
  }
  //枚数を数える また強さを調べる
  if(state.sequence==0){
    //枚数組
    for(;i<5;i++){
      if(cards[i][j]>0){
	count++;
	state.suit[i]=1;
      }else{
	state.suit[i]=0;
      }
    }
    
    //ジョーカー単騎が場に出ているならば、state.ordを最大の強さを示すものに設定
    //ノーマルカードの場合は、その強さをそのままstate.ordへ格納
    if ((found_joker == 1) && (count == 1)){
      if (state.rev == 0){
	state.ord = 14;
      }else{
	state.ord = 0;
      }
    }else{
      state.ord=j;
    }
 
  }else{
    //階段
    while(j+count<15 && cards[i][j+count]>0){
      count++;
    }
    if((state.rev==0 && state.b11==0 )||( state.rev==1 && state.b11==1 )){
      state.ord=j+count-1;
    }else{
      state.ord=j;
    }
    state.suit[i]=1;
  }
  //枚数を記憶
  state.qty=count;
 
  if(state.qty>0){ //枚数が0より大きいとき 新しい場のフラグを0にする
    state.onset=0;
  }
}


void showState(struct state_type *state){
  /*引数で渡された状態stateの内容を表示する*/
  int i;
  printf("state rev   : %d\n",state->rev);
  printf("state lock  : %d\n",state->lock);
  printf("state joker : %d\n",state->joker);
  
  printf("state qty   : %d\n",state->qty);
  printf("state ord   : %d\n",state->ord);
  printf("state seq   : %d\n",state->sequence);
  printf("state onset : %d\n",state->onset);
  printf("state suit :");
  for(i=0;i<4;i++)printf("%d ",state->suit[i]);
  printf("\n"); printf("state player qty :");
  for(i=0;i<5;i++)printf("%d ",state->player_qty[i]);
  printf("\n"); printf("state player rank :");
  for(i=0;i<5;i++)printf("%d ",state->player_rank[i]);
  printf("\n"); printf("state player_num on seat :");
  for(i=0;i<5;i++)printf("%d ",state->seat[i]);
  printf("\n");
}

//それぞれカードの和 共通 差分 逆転 をとる
void cardsOr(int cards1[8][15],int cards2[8][15]){
  /*
    cards1にcards2にあるカードを加える
  */
  int i,j;
  
  for(i=0;i<15;i++)
    for(j=0;j<5;j++)
      if(cards2[j][i]>0)cards1[j][i]=1; 
}

void cardsAnd(int cards1[8][15],int cards2[8][15]){ 
  /*
    cards1のカードのうち、cards2にあるものだけをcards1にのこす。
  */
  int i,j;
  
  for(i=0;i<15;i++)
    for(j=0;j<5;j++)
      if(cards1[j][i]==1&&cards2[j][i]==1) cards1[j][i]=1;
      else cards1[j][i]=0;
}

void cardsDiff(int cards1[8][15],int cards2[8][15]){ 
  /*
    cards1からcards2にあるカードを削除する
  */
  int i,j;
  
  for(i=0;i<15;i++)
    for(j=0;j<5;j++)
      if(cards2[j][i]==1) cards1[j][i]=0;
}
void cardsNot(int cards[8][15]){ 
  /*
    カードの有無を反転させる
  */
  int i,j;
  
  for(i=0;i<15;i++)
    for(j=0;j<5;j++)
      if(cards[j][i]==1) cards[j][i]=0;
      else cards[j][i]=1;
}


void outputTable(int table[8][15]){ 
  /*
    引数で渡されたカードテーブルを出力する
  */
  int i,j;
  for(i=0;i<8;i++){
    for(j=0;j<15;j++){
      printf("%i ",table[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void copyTable(int dest_table[8][15], int org_table[8][15]){ 
  /*
    引数で渡されたカードテーブルorg_tableを
    カードテーブルdest_tableにコピーする
  */
  int i,j;
  for(i=0;i<8;i++){
    for(j=0;j<15;j++){
      dest_table[i][j]=org_table[i][j];
    }
  }
} 

void copyCards(int dest_cards[8][15],int org_cards[8][15]){ 
  /*
    引数で渡されたカードテーブルorg_cardsのカード情報の部分を
    カードテーブルdest_cardsにコピーする
  */
  int i,j;
  for(i=0;i<5;i++){
    for(j=0;j<15;j++){
      dest_cards[i][j]=org_cards[i][j];
    }
  }
}


void clearCards(int cards[8][15]){  
  /*
    引数で渡されたカードテーブルcardsのカード情報の部分を全て0にし、カードを一枚も無い状態にする。
  */
  int s,t;
  
  for(s=0;s<5;s++){
    for(t=0;t<15;t++){
      cards[s][t]=0;
    }   
  }
}

void clearTable(int cards[8][15]){ 
  /*
    引数で渡されたカードテーブルcardsを全て0にする。
  */
  int s,t;
  
  for(s=0;s<8;s++){
    for(t=0;t<15;t++){
      cards[s][t]=0;
    }   
  }
}

int beEmptyCards(int cards[8][15]){  
  /*
    引数で渡されたカードテーブルcardsの含むカードの枚数が0のとき1を、
    それ以外のとき0を返す
  */
  int i,j,f=1;
  
  for(i=0;i<5;i++){
    for(j=0;j<15;j++){
      if(cards[i][j]>0)f=0;
    }
  }
  return f;
}

int qtyOfCards(int cards[8][15]){  
  /*
    引数で渡されたカードテーブルcardsの含むカードの枚数を返す
  */
  int i,j,count=0;
  
  for(i=0;i<5;i++)
    for(j=0;j<15;j++)
      if(cards[i][j]>0)count++;
  
  return count;
}

void lowSolo(int out_cards[8][15],int my_cards[8][15],int joker_flag){
  /*
    低い方から探して,最初に見つけたカードを一枚out_cardsにのせる。
    joker_flagが1のとき,カードが見つからなければ,jokerを一枚out_cardsにのせる。
  */
  int i,j,find_flag=0;

  clearTable(out_cards);                  //テーブルをクリア
  for(j=1;j<14&&find_flag==0;j++){        //低い方からさがし
    for(i=0;i<4&&find_flag==0;i++){
      if(my_cards[i][j]==1){              //カードを見つけたら               
	find_flag=1;                      //フラグを立て
	out_cards[i][j]=my_cards[i][j];   //out_cardsにのせ,ループを抜ける。
      }
    }
  }
  if(find_flag==0&&joker_flag==1){       //見つからなかったとき
    out_cards[0][14]=2;                  //ジョーカーをのせる
  }
}

void change(int out_cards[8][15],int my_cards[8][15],int num_of_change){
  /*
    カード交換時のアルゴリズム
    大富豪あるいは富豪が、大貧民あるいは貧民にカードを渡す時のカードを
    カードテーブルmy_cardsと交換枚数num_of_changeに応じて、
    低いほうから選びカードテーブルout_cardsにのせる
  */
  int count=0;
  int one_card[8][15];
  
  clearTable(out_cards);
  while(count<num_of_change){
    lowSolo(one_card,my_cards,0);
    cardsDiff(my_cards,one_card);
    cardsOr(out_cards,one_card);
    count++;
  }
}

void lead(int out_cards[8][15],int my_cards[8][15]){
  /*
    新しくカードを提出するときの選択ルーチン
  */
  generateHands(cards2Bitboard(my_cards), &stack);
  
  /* 最も評価値の高い手を選択 */
  boardInfo tmpBoard;
  boardInfo bestBoard;
  int bestEval = 0;
  while (stack.top > 0) {
    tmpBoard = popBoardStack(&stack);
    int eval = evaluateHand(tmpBoard, &state);
    if (eval > bestEval) {
      bestEval = eval;
      bestBoard = tmpBoard;
    }
  }
  
  clearTable(out_cards);
  putBoardInfoIntoCards(bestBoard, out_cards);
}

void follow(int out_cards[8][15],int my_cards[8][15]){
  /*
    他のプレーヤーに続いてカードを出すときのルーチン
  */
  generateHands(cards2Bitboard(my_cards), &stack);
  
  /* 最も評価値の高い手を選択 */
  boardInfo tmpBoard;
  boardInfo bestBoard;
  int bestEval = 0;
  while (stack.top > 0) {
    tmpBoard = popBoardStack(&stack);
    int eval = evaluateHand(tmpBoard, &state);
    if (eval > bestEval) {
      bestEval = eval;
      bestBoard = tmpBoard;
    }
  }
  
  clearTable(out_cards);
  putBoardInfoIntoCards(bestBoard, out_cards);
}

int cmpCards(int cards1[8][15],int  cards2[8][15]){
  /*
    カードテーブルcards1、cards2のカード部分を比較し、
    異なっていれば1、一致していれば0を返す
  */
  int i,j,flag=0;
  
  for(i=0;i<5;i++)
    for(j=0;j<15;j++)
      if(cards1[i][j]!=cards2[i][j])
	flag=1;
  
  return flag;
}

int cmpState(struct state_type* state1,struct state_type* state2){
  /*
    状態を格納するstate1とstate2を比較し、一致すれば0を、
    異なっていればそれ以外を返す
  */
  int i,flag=0;
  if(state1->ord != state2->ord) flag+=1;
  if(state1->qty != state2->qty) flag+=2;
  if(state1->sequence != state2->sequence) flag+=4;
  for(i=0;i<5;i++)
    if(state1->suit[i]!=state2->suit[i]) flag+=8;
  if(state1->onset != state2->onset) flag+=16;
  return flag;
}

int getLastPlayerNum(int ba_cards[8][15]){
  /*
    最後パス以外のカード提出をしたプレーヤーの番号を返す。
    この関数を正常に動作させるためには、
    サーバから場に出たカードをもらう度に
    この関数を呼び出す必要がある。
  */
  static struct state_type last_state;
  static int last_player_num=-1;
  
  if(g_logging==1){  //ログの表示
    printf("Now state \n");
    showState(&state);
    printf(" Last state \n");
    showState(&last_state);
  }
  
  if(cmpState(&last_state,&state)!=0){ //場の状態に変化が起きたら
    last_player_num =ba_cards[5][3];   //最後のプレーヤと
    last_state=state;                  //最新の状態を更新する
  }
  
  if(g_logging==1){ //ログの表示
    printf("last player num : %d\n",last_player_num);
  }
  
  return last_player_num;
}
