#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

#ifndef SELECT_CARDS_H
#define SELECT_CARDS_H
#include "select_cards.h"
#endif

#ifndef SEARCH_CARDS_H
#define SEARCH_CARDS_H
#include "search_cards.h"
#endif

#ifndef EVALUATE_CARDS_H
#define EVALUEATE_CARDS_H
#include "evaluate_cards.h"
#endif

#define KAIDAN_IDX 1
#define PAIR_IDX 2
#define SINGLE_IDX 3

/* general系関数呼び出し部分の可読性のために用意． */
#define WITH_JOKER 1
#define WITHOUT_JOKER 0
#define INCLUDE_ORDER 1
#define NOT_INCLUDE_ORDER 0
#define LOWER 1
#define HIGHER 0

void set_state_from_own_cards(int cards[8][15]) {
  state.joker = (cards[4][1] == 2);
  state.onset = (cards[5][4] == 1);
  state.b11   = (cards[5][5] == 1); // 現行(2021-06-14)のルール下では未使用．
  state.rev   = (cards[5][6] == 1);
  state.lock  = (cards[5][7] == 1);

  /* 空場のとき，今までの場のカード情報をリセット． */
  if (state.onset) {
    state.qty  = 0;
    state.ord  = 0;
    state.lock = 0;
    for(int i=0; i<5; i++) { state.suit[i] = 0; }
  }

  for (int i=1; i<5; i++) {
    state.player_qty[i]  = cards[6][i];
    state.player_rank[i] = cards[6][5+i];
    state.seat[i]        = cards[6][10+i];
  }
}

void set_state_from_table(int cards[8][15]) {
  int suit, ord;

  /* 場のスート情報初期化． */
  for (int i=0; i<4; i++) {
    state.suit[i] = 0;
  }

  /* カードのある位置を探す． */
  while (cards[suit][ord] == 0) {
    suit++;
    if (suit >= 4) {
      ord++; suit = 0;
    }
  }

  /* 見つけたカードの次が空でなければ階段． */
  if(ord < 14) {
    state.sequence = (cards[suit][ord+1] > 0);
  }

  /* 場のカードの強さ，枚数，スートを保存． */
  int card_num = 0;
  if (!state.sequence) { // 階段以外の場合．
    for (; suit < 4; suit++) {
      if (cards[suit][ord] > 0) {
        card_num++;
        state.suit[suit] = 1;
      }
    }
    if ((cards[suit][ord] == 2) && (card_num == 1)) { // joker単騎なら強さordは最強．
      state.ord = (state.rev ? 0 : 14);
    }
    else {
      state.ord = ord;
    }
  }
  else { // 階段の場合．
    while (ord+card_num<15 && cards[suit][ord + card_num]>0) {
      card_num++;
    }
    if ((!state.rev && !state.b11) ||
        (state.rev  &&  state.b11)) {
      state.ord = ord + card_num - 1;
    }
    else {
      state.ord = ord;
    }
    state.suit[suit] = 1;
  }
  state.qty = card_num;
}

/* 場にカードがある状態をシミュレーションするために，
   cardsが場に出た後に生じるであろう架空のstateを生成． */
state_type simulate_state(int cards[8][15]) {
  state_type st;
  st.onset = 0;
  st.b11 = 0;

  int i,j,count=0;
  int found_joker = 0;
  int suit_count = 0;
  i=j=0;

  //カードのある位置を探す
  while(j<15&&cards[i][j]==0){
    st.suit[i]=0;
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
    if(cards[i][j+1]>0) st.sequence=1;
    else st.sequence=0;
  }
  //枚数を数える また強さを調べる
  if(st.sequence==0){
    //枚数組
    for(;i<5;i++){
      if(cards[i][j]>0){
        count++;
        st.suit[i]=1;
      }
      else{
        st.suit[i]=0;
      }
    }
    
    /* カードが4枚以上ある XOR 既に革命状態 THEN 革命． */
    st.rev = (count >= 4) ^ state.rev;

    //ジョーカー単騎が場に出ているならば、st.ordを最大の強さを示すものに設定
    //ノーマルカードの場合は、その強さをそのままst.ordへ格納
    if ((found_joker == 1) && (count == 1)){
      if (st.rev == 0){
        st.ord = 14;
      }
      else{
        st.ord = 0;
      }
    }
    else{
      st.ord=j;
    }

  }else{
    //階段
    while(j+count<15 && cards[i][j+count]>0){
      count++;
    }

    /* カードが5枚以上ある XOR 既に革命状態 THEN 革命． */
    st.rev = (count >= 5) ^ state.rev;

    if((st.rev==0 && st.b11==0 )||( st.rev==1 && st.b11==1 )){
      st.ord=j+count-1;
    }else{
      st.ord=j;
    }
    st.suit[i]=1;
  }

  //枚数を記憶
  st.qty=count;

  /* 先に出ているカードとシミュレーション対象のカードの
     スートが同じならしばりになっている． */
  int is_locked = 1;
  for (int suit=0; suit<4; suit++) {
    if (state.suit[suit] != st.suit[suit]) {
      is_locked = 0;
      break;
    }
  }
  st.lock = is_locked;
}
void print_state(struct state_type *state) {
  printf("==================================\n");
  printf("print state\n");
  printf("==================================\n");

  printf("state rev                : %d\n",state->rev);
  printf("state lock               : %d\n",state->lock);
  printf("state joker              : %d\n",state->joker);
  
  printf("state qty                : %d\n",state->qty);
  printf("state ord                : %d\n",state->ord);
  printf("state sequence           : %d\n",state->sequence);
  printf("state onset              : %d\n",state->onset);

  printf("state suit               :");
  for (int i=0; i<4; i++) { printf("%d ",state->suit[i]); }
  printf("\n");
  
  printf("state player qty         :");
  for (int i=0; i<5; i++) { printf("%d ",state->player_qty[i]); }
  printf("\n");
  
  printf("state player rank        :");
  for (int i=0; i<5; i++)printf("%d ",state->player_rank[i]);
  printf("\n"); 

  printf("state player_num on seat :");
  for (int i=0; i<5; i++) { printf("%d ",state->seat[i]); }
  printf("\n");

  printf("==================================\n");
}

int is_same_cards(int cards1[8][15], int cards2[8][15]) {
  for (int suit=0; suit<5; suit++) {
    for (int ord=0; ord<15; ord++) {
      if(cards1[suit][ord] != cards2[suit][ord]) {
        return 0;
      }
    }
  }
  
  return 1;
}

void cards_OR(int passive[8][15], int active[8][15]) {
  for (int i=0; i<15; i++) {
    for (int j=0; j<5; j++) {
      if (active[j][i] > 0) { passive[j][i]=1; }
    }
  }
}

void cards_AND(int passive[8][15], int active[8][15]) { 
  for (int i=0; i<15; i++) {
    for (int j=0; j<5; j++) {
      if(passive[j][i]>0 && active[j][i]>0) {
        passive[j][i] = 1;
      }
      else {
        passive[j][i] = 0;
      }
    }
  }
}

void cards_DIFF(int passive[8][15], int active[8][15]) { 
  for (int i=0; i<15; i++) {
    for (int j=0; j<5; j++) {
      if(active[j][i] > 0) { passive[j][i] = 0; }
    }
  }
}

void cards_NOT(int passive[8][15]) { 
  for (int i=0; i<15; i++) {
    for (int j=0; j<5; j++) {
      if (passive[j][i] > 0) {
        passive[j][i] = 0;
      }
      else {
        passive[j][i] = 1;
      }
    }
  }
}

void print_cards(int cards[8][15]) { 
  printf("==================================\n");
  printf("print cards\n");
  printf("==================================\n");
  for (int i=0; i<8; i++) {
    for (int j=0; j<15; j++) {
      printf("%d ", cards[i][j]);
    }
    printf("\n");
  }
  printf("==================================\n");
}

void copy_cards(int dest_cards[8][15], int org_cards[8][15]) { 
  for (int i=0; i<8; i++) {
    for (int j=0; j<15; j++) {
      dest_cards[i][j] = org_cards[i][j];
    }
  }
} 

void clear_cards(int cards[8][15]) { 
  for (int i=0; i<8; i++) {
    for (int j=0; j<15; j++) {
      cards[i][j]=0;
    }   
  }
}

int qty_of_cards(int cards[8][15]) {  
  int count = 0;
  for (int i=0; i<5; i++) {
    for (int j=0; j<15; j++) {
      if (cards[i][j] > 0) { count++; }
    }
  }
  return count;
}

int max_of_cards(int cards[8][15]) {
  int max = 0;
  for (int i=0; i<4; i++) {
    for (int j=0; j<15; j++) {
      if (cards[i][j] > max) { max = cards[i][j]; }
    }
  }
  return max;
}

int is_cards_empty(int cards[8][15]) {  
  return qty_of_cards(cards) == 0;
}

void choose_low_single(int out_cards[8][15], int my_cards[8][15]) {
  clear_cards(out_cards);

  /* 弱い順にカードを探し見つかればそれをout_cardsに載せて終了． */
  for (int ord=1; ord<14; ord++) {
    for (int suit=0; suit<4; suit++) {
      if(my_cards[suit][ord] == 1) {
        out_cards[suit][ord] = 1;
        return;
      }
    }
  }

  /* 見つからなければjokerを載せる． */
  if (state.joker) {
    out_cards[0][14] = 2;
  }
}

void init_opponents_cards(int opponents_cards[8][15]) {
  for (int suit=0; suit<4; suit++) {
    for (int ord=1; ord<14; ord++) {
      opponents_cards[suit][ord] = 1;
    }
  }
  opponents_cards[4][1] = 2;
}

/* 相手のカード全体(= 未観測)から，観測されたカードを引く． */
void update_opponents_cards(int opponents_cards[8][15], int observed_cards[8][15]) {
  cards_DIFF(opponents_cards, observed_cards);
}

void select_change_cards(int out_cards[8][15], int my_cards[8][15], int num_of_change) {
  int count = 0;
  int tmp[8][15];
  
  clear_cards(out_cards);
  while (count < num_of_change) {
    choose_low_single(tmp, my_cards);
    cards_DIFF(my_cards, tmp);
    cards_OR(out_cards, tmp);
    count++;
  }
}

void select_cards(int out_cards[8][15], int my_cards[8][15], int opponent_cards[8][15]) {
  generate_hands(cards2bitboard(my_cards), &stack);

  /* 最も評価値の高い手を選択 */
  board_info tmp_board;
  board_info bestBoard;
  int bestEval = 0;
  printf("#####見つかったカード#####\n");
  while (stack.top >= 0) {
    tmp_board = pop_board_stack(&stack);
    print_board_info(tmp_board);
    int tmp_cards[8][15];
    clear_cards(tmp_cards);
    board_stack opponent_stack;
    generate_hands(cards2bitboard(opponent_cards), &opponent_stack);
    put_board_info_into_cards(tmp_board, tmp_cards);
    state_type st = simulate_state(tmp_cards);
    int eval = evaluate_hand(tmp_board, &opponent_stack, &state, &st);
    printf("eval: %d\n\n", eval);
    if (eval > bestEval) {
      bestEval = eval;
      bestBoard = tmp_board;
    }
  }
  printf("#########################\n");

  clear_cards(out_cards);
  put_board_info_into_cards(bestBoard, out_cards);
}
