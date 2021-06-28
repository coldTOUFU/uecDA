#include "string.h"

#ifndef SEARCH_CARDS_H
#define SEARCH_CARDS_H
#include "search_cards.h"
#endif

/* 2次元配列型 -> bitboard */
uint64_t cards2bitboard(int source[8][15]){
  int i, j;
  uint64_t bitboard = 0;

  /* Jokerがある場合． */
  if (source[4][1] == 2) {
    bitboard++;
  }
  bitboard = bitboard << 1;

  /* 各持ち札をbitboardに反映 */
  for(i=0; i<4; i++){
    for(j=0; j<15; j++){
      if(source[i][j]==1){
        bitboard++;
      }
      bitboard = bitboard << 1;
    }
  }

  return bitboard >> 1;
}

/* bitboard -> boardinfo */
board_info bitboard2board_info(uint64_t source, uint64_t joker) {
  uint64_t cards = source;
  board_info b_info;

  b_info.cards = cards;

  b_info.joker = joker;
  
  b_info.num_of_cards = count_bit(cards);

  /* 一番強いカードの強さが15bitで格納される． */
  /* 各スートを15bitの範囲に右シフトしたものの論理和をとって，
     15bitの範囲より大きいところに立つ1を0b111111111111111との論理積で落とす．
     最後に一番右の(= 一番強い)カードだけ取り出す． */
  cards = source & 0xfffffffffffffff | joker; // sourceの61bit上のjokerをカットし，配置済みjokerを追加．
  cards = (cards | (cards >> 15) | (cards >> 30) | (cards >> 45)) & 0b111111111111111;
  b_info.SCard = cards & (-cards);

  /* 一番弱いカードの強さが15bitで格納される． */
  /* 各スートを15bitの範囲に右シフトしたものの論理和をとって，
     15bitの範囲より大きいところに立つ1を0b111111111111111との論理積で落とす．
     最後に次の手順で一番左の(= 一番弱い)カードだけ取り出す．
     すなわち，bitを右シフトしながら和を取っていき，一番左の1以降にすべて1を立てる．
     最後にそれを1bit右シフトしたもので引けば，一番左のもののみ残る． */
  cards = (source & 0xfffffffffffffff) | joker; // sourceの61bit上のjokerをカットし，配置済みjokerを追加．
  cards = (cards | (cards >> 15) | (cards >> 30) | (cards >> 45)) & 0b111111111111111;
  cards |= (cards >> 1);
  cards |= (cards >> 2);
  cards |= (cards >> 4);
  cards |= (cards >> 8);
  b_info.WCard = cards - (cards>>1);

  /* スートを抽出．まずSCardの分右シフトをしたうえで1bit，16bit，31bit，46bit目にのみ1が立つよう論理積を取る．
     その後，それらを下位4bitに写して16bit以降を論理積でカットする． */
  cards = source & 0xfffffffffffffff | joker; // sourceの61bit上のjokerをカットし，配置済みjokerを追加．
  cards = source/b_info.SCard & 0b1000000000000001000000000000001000000000000001;
  cards = (cards | (cards >> 14) | (cards >> 28) | (cards >> 42)) & 0b111111111111111;
  b_info.suit = cards;

  if (b_info.num_of_cards == 1) {
    b_info.card_type = SINGLE_HAND;
  }
  else {
    b_info.card_type = (count_bit(b_info.suit) == 1 ? SEQUENCE_HAND : PAIR_HAND);
  }

  return b_info;
}

void put_board_info_into_cards(board_info source, int cards[8][15]) {
  int i, j;

  for(i=3; i>=0; i--){
    for(j=14; j>=0; j--){
      if (source.cards % 2) {
        cards[i][j] = 1;
      }
      else {
        cards[i][j] = 0;
      }
      if (source.joker % 2) {
        cards[i][j] = 2;
      }
      source.cards = source.cards >> 1;
      source.joker = source.joker >> 1;
    }
  }
}

void putBitboardIntoCards(uint64_t source, int cards[8][15]) {
  int i, j;

  for(i=3; i>=0; i--){
    for(j=14; j>=0; j--){
      if (source % 2) {
        cards[i][j] = 1;
      }
      else {
        cards[i][j] = 0;
      }
      source = source >> 1;
    }
  }
}

/* ビット表現で，aとbの論理積がbと一致するならaはbを含む． */
int is_inclusion(uint64_t a, uint64_t b) {
  return (a & b) == b;
}

void print_bitboard(uint64_t source){
  int i;
  for(i=0; i<61; i++){
    printf("%lu", (source >> (60-i)) %2);
  } 
  printf("\n");
}

int count_bit(uint64_t source){
  source = (source & 0x5555555555555555) + (source >> 1 & 0x5555555555555555);
  source = (source & 0x3333333333333333) + (source >> 2 & 0x3333333333333333);
  source = (source & 0x0f0f0f0f0f0f0f0f) + (source >> 4 & 0x0f0f0f0f0f0f0f0f);
  source = (source & 0x00ff00ff00ff00ff) + (source >> 8 & 0x00ff00ff00ff00ff);
  source = (source & 0x0000ffff0000ffff) + (source >>16 & 0x0000ffff0000ffff);
  return   (source & 0x00000000ffffffff) + (source >>32 & 0x00000000ffffffff);
}

/* bitboard表現のカードbitboardから，sequenceSize枚の階段を探してboard_stackにpushする． */
void search_sequence(uint64_t bitboard, int sequenceSize, board_stack *board){
  uint64_t filters[10]={0,1,3,7,15,31,63,127,255,511}; // i番目にi枚の階段を探すフィルタがある．
  uint64_t tmpfilter;
  int i,j;

  for(i=0; i<4; i++){
    for(j=0; j<15-sequenceSize; j++){
      /* filterを，i番目のスートでj番目から始めるものに適用できるようシフトする． */
      tmpfilter= (filters[sequenceSize] << (15*i + j));

      /* 手札が現在かけているフィルターを含む(=階段を含む)なら，それをstackに積む． */
      if(is_inclusion(bitboard, tmpfilter)) { 
        //print_bitboard(tmpfilter);
        board_info tmp = bitboard2board_info(tmpfilter, 0);
        push_board_stack(board, tmp);
      }
    }
  }
}

/* Jokerを考慮した階段探索 */
void search_sequence_with_joker(uint64_t bitboard, int sequenceSize, board_stack *board){
  uint64_t filter[10]={0,1,3,7,15,31,63,127,255,511};
  uint64_t tmpfilter;
  int i,j;

  for(i=0; i<4; i++){
    for(j=0; j<15-sequenceSize; j++){
      tmpfilter= (filter[sequenceSize] << (15*i + j));
      if(count_bit(bitboard & tmpfilter) == sequenceSize-1){
        //print_bitboard(tmpfilter);
        board_info tmp = bitboard2board_info(tmpfilter, ~bitboard & tmpfilter);
        push_board_stack(board, tmp);
      }
    }
  }
}

void search_pair(uint64_t bitboard, int pair_size, board_stack *board){
  /* 各フィルター配列の0番目は，そのフィルターの枚数． */
  uint64_t filter[4][7]={ 
    {4,1,32768,1073741824,35184372088832}, /* 各スートに1たてる． */
    {6,32769, 1073741825, 35184372088833, 1073774592, 
     35184372121600, 35185445830656}, /* 各2スートに1たてる．4つから2つ選ぶから6通り． */
    {4,1073774593, 35184372121601, 35185445830657, 
     35185445863424}, /* 各3スートに1立てる．4つから3つ選ぶから4通り． */
    {1,35185445863425}}; /* 各4スートに1立てる． */
  uint64_t tmpfilter;
  int i,j;

  for(i=1; i<filter[pair_size-1][0]+1; i++){
    for(j=0; j<15; j++){
      tmpfilter= (filter[pair_size-1][i] << j);
      if((bitboard & tmpfilter) == tmpfilter){
        //print_bitboard(tmpfilter);
        board_info tmp = bitboard2board_info(tmpfilter, 0);
        push_board_stack(board, tmp);
      }
    }
  }
}

void search_pair_with_joker(uint64_t bitboard, int pair_size, board_stack *board){
  uint64_t filter[4][7]={ 
    {4,1,32768,1073741824,35184372088832}, 
    {6,32769, 1073741825, 35184372088833, 1073774592, 
     35184372121600, 35185445830656}, 
    {4,1073774593, 35184372121601, 35185445830657, 
     35185445863424}, 
    {1,35185445863425}};
  uint64_t tmpfilter;
  int i,j;

  for(i=1; i<filter[pair_size-1][0]+1; i++){
    for(j=0; j<15; j++){
      tmpfilter= (filter[pair_size-1][i] << j);
      if(count_bit(bitboard & tmpfilter) == pair_size-1){
        //printf("=---\n");
        //print_bitboard(tmpfilter);
        //print_bitboard(bitboard & tmpfilter);
        //print_bitboard(~bitboard & tmpfilter);
        board_info tmp = bitboard2board_info(tmpfilter, ~bitboard & tmpfilter);
        push_board_stack(board, tmp);
      }
    }
  }
}

/* 合法手をスタックに詰める */
void generate_hands(uint64_t cards, board_stack *board){
  int number;
  board->top = -1;

  search_sequence(cards, 1, board);
  for(number=3; number< 10; number++){
    search_sequence(cards, number, board);
  }
  for(number=2; number< 5; number++){
    search_pair(cards, number, board);
  }

  /* Jokerがある場合． */
  if((cards>>60)%2){
    for(number=3; number< 10; number++){
      search_sequence_with_joker(cards, number, board);
    }
    for(number=2; number< 5; number++){
      search_pair_with_joker(cards, number, board);
    }
  }
}

void printSuit(int suit) {
  if (suit == 0) {
    printf("スペード");
    return;
  }
  if (suit == 1) {
    printf("ハート");
    return;
  }
  if (suit == 2) {
    printf("ダイヤ");
    return;
  }
  if (suit == 3) {
    printf("クローバー");
    return;
  }
  printf("該当しないスート値%d", suit);
}

void printNum(int num) {
  if (num > 0 && num < 9) {
    printf("%d", num+2);
    return;
  }
  if (num == 9) {
    printf("J");
    return;
  }
  if (num == 10) {
    printf("Q");
    return;
  }
  if (num == 11) {
    printf("K");
    return;
  }
  if (num == 12) {
    printf("A");
    return;
  }
  if (num == 13) {
    printf("2");
    return;
  }
  printf("該当しない数字%d", num);
}

void prettyPrintBitBoard(uint64_t source) {
  printf("------カードの出力------\n");
  print_bitboard(source);
  for (int i=3; i>=0; i--) {
    for (int j=14; j>=0; j--) {
      if (source % 2) {
        printf("スート: ");
        printSuit(i);
        printf("\n");
        printf("数字:");
        printNum(j);
        printf("\n\n");
      }
      source >>= 1;
    }
  }
  printf("-----------------------\n");
}

void print_board_info(board_info source) {
  printf("=======================\n");
  if (source.card_type == SINGLE_HAND) {
    printf("card_type: SINGLE\n");
  }
  else if (source.card_type == PAIR_HAND) {
    printf("card_type: PAIR\n");
  }
  else if (source.card_type == SEQUENCE_HAND) {
    printf("card_type: SEQUENCE\n");
  }
  else {
    printf("card_type: %d\n", source.card_type);
  }

  printf("num_of_cards: %d\n", source.num_of_cards);

  printf("suit: ");
  if (source.suit % 2) {
    printf("クローバー, ");
  }
  source.suit >>= 1;
  if (source.suit % 2) {
    printf("ダイヤ, ");
  }
  source.suit >>= 1;
  if (source.suit % 2) {
    printf("ハート, ");
  }
  source.suit >>= 1;
  if (source.suit % 2) {
    printf("スペード");
  }
  printf("\n");

  printf("cards: \n");
  prettyPrintBitBoard(source.cards);

  printf("joker: \n");
  prettyPrintBitBoard(source.joker);

  printf("SCard: \n");
  prettyPrintBitBoard(source.SCard);

  printf("WCard: \n");
  prettyPrintBitBoard(source.WCard);
  printf("=======================\n");
}

void push_board_stack(board_stack *boards, board_info source){
  boards->top++;
  memset(&(boards->board[boards->top]),0,sizeof(board_info));
  boards->board[boards->top] = source; 
  return;
}

board_info pop_board_stack(board_stack *boards) {
  board_info tmp = boards->board[boards->top];
  boards->top--;
  return tmp;
}
