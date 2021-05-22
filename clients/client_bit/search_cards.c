#ifndef SEARCH_CARDS_H
#define SEARCH_CARDS_H
#include "search_cards.h"
#endif

/* 2次元配列型 -> bitboard */
uint64_t cards2Bitboard(int source[8][15]){
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
boardInfo bitboard2BoardInfo(uint64_t source, uint64_t joker) {
  uint64_t cards = source;
  boardInfo bInfo;

  bInfo.cards = cards;

  bInfo.joker = joker;
  
  bInfo.numOfCards = countBit(cards);

  /* 一番強いカードの強さが15bitで格納される． */
  /* 一番右の1だけ立てた上で，
     各スートを15bitの範囲に右シフトしたものの論理和をとって，
     15bitの範囲より大きいところに立つ1を0b111111111111111との論理積で落とす． */
  cards = source & 0xfffffffffffffff | joker; // sourceの61bit上のjokerをカットし，配置済みjokerを追加．
  cards = cards&(-cards);
  bInfo.SCard = (cards | (cards >> 15) | (cards >> 30) | (cards >> 45)) & 0b111111111111111;

  /* 一番弱いカードの強さが15bitで格納される． */
  cards = source & 0xfffffffffffffff | joker; // sourceの61bit上のjokerをカットし，配置済みjokerを追加．
  cards |= cards >> 1;
  cards |= cards >> 2;
  cards |= cards >> 4;
  cards |= cards >> 8;
  cards |= cards >> 16;
  cards |= cards >> 64;
  cards ^= cards >> 1;
  bInfo.WCard = (cards | (cards >> 15) | (cards >> 30) | (cards >> 45)) & 0b111111111111111;

  /* スートを抽出．まずSCardの分右シフトをしたうえで1bit，16bit，31bit，46bit目にのみ1が立つよう論理積を取る．
     その後，それらを下位4bitに写して16bit以降を論理積でカットする． */
  cards = source & 0xfffffffffffffff | joker; // sourceの61bit上のjokerをカットし，配置済みjokerを追加．
  cards = source/bInfo.SCard & 0b1000000000000001000000000000001000000000000001;
  cards = (cards | (cards >> 14) | (cards >> 28) | (cards >> 42)) & 0b111111111111111;
  bInfo.suit = cards;

  if (bInfo.WCard == bInfo.SCard && countBit(bInfo.suit) == 1) {
    bInfo.cardType = SINGLE_HAND;
  }
  else {
    bInfo.cardType = (bInfo.WCard == bInfo.SCard ? PAIR_HAND : SEQUENCE_HAND);
  }

  return bInfo;
}

void putBoardInfoIntoCards(boardInfo source, int cards[8][15]) {
  int i, j;

  for(i=3; i>=0; i--){
    for(j=14; j>=0; j--){
      if (source.cards % 2) {
        cards[i][j] = 1;
      }
      if (source.joker % 2) {
        cards[i][j] = 2;
      }
      source.cards = source.cards >> 1;
      source.joker = source.joker >> 1;
    }
  }
}

/* ビット表現で，aとbの論理積がbと一致するならaはbを含む． */
int isContainingOnBit(uint64_t a, uint64_t b) {
  return (a & b) == b;
}

void printBitboard(uint64_t source){
  int i;
  for(i=0; i<61; i++){
    printf("%lu", (source >> (60-i)) %2);
  } 
  printf("\n");
}

int countBit(uint64_t source){
  source = (source & 0x5555555555555555) + (source >> 1 & 0x5555555555555555);
  source = (source & 0x3333333333333333) + (source >> 2 & 0x3333333333333333);
  source = (source & 0x0f0f0f0f0f0f0f0f) + (source >> 4 & 0x0f0f0f0f0f0f0f0f);
  source = (source & 0x00ff00ff00ff00ff) + (source >> 8 & 0x00ff00ff00ff00ff);
  source = (source & 0x0000ffff0000ffff) + (source >>16 & 0x0000ffff0000ffff);
  return   (source & 0x00000000ffffffff) + (source >>32 & 0x00000000ffffffff);
}

/* bitboard表現のカードbCardsから，sequenceSize枚の階段を探してboardStackにpushする． */
void searchSequence(uint64_t bCards, int sequenceSize, boardStack *board){
  uint64_t filters[10]={0,1,3,7,15,31,63,127,255,511}; // i番目にi枚の階段を探すフィルタがある．
  uint64_t tmpfilter;
  int i,j;

  for(i=0; i<4; i++){
    for(j=0; j<15-sequenceSize; j++){
      /* filterを，i番目のスートでj番目から始めるものに適用できるようシフトする． */
      tmpfilter= (filters[sequenceSize] << (15*i + j));

      /* 手札が現在かけているフィルターを含む(=階段を含む)なら，それをstackに積む． */
      if(isContainingOnBit(bCards, tmpfilter)) { 
        //printBitboard(tmpfilter);
        boardInfo tmp = bitboard2BoardInfo(tmpfilter, 0);
        pushBoardStack(board, tmp);
      }
    }
  }
}

/* Jokerを考慮した階段探索 */
void searchSequenceWJoker(uint64_t bCards, int sequenceSize, boardStack *board){
  uint64_t filter[10]={0,1,3,7,15,31,63,127,255,511};
  uint64_t tmpfilter;
  int i,j;

  for(i=0; i<4; i++){
    for(j=0; j<15-sequenceSize; j++){
      tmpfilter= (filter[sequenceSize] << (15*i + j));
      if(countBit(bCards & tmpfilter) == sequenceSize-1){
        //printBitboard(tmpfilter);
        boardInfo tmp = bitboard2BoardInfo(tmpfilter, ~bCards & tmpfilter);
        pushBoardStack(board, tmp);
      }
    }
  }
}

void searchPair(uint64_t bCards, int pairSize, boardStack *board){
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

  for(i=1; i<filter[pairSize-1][0]+1; i++){
    for(j=0; j<15; j++){
      tmpfilter= (filter[pairSize-1][i] << j);
      if((bCards & tmpfilter) == tmpfilter){
        //printBitboard(tmpfilter);
        boardInfo tmp = bitboard2BoardInfo(tmpfilter, 0);
        pushBoardStack(board, tmp);
      }
    }
  }
}

void searchPairWJoker(uint64_t bCards, int pairSize, boardStack *board){
  uint64_t filter[4][7]={ 
    {4,1,32768,1073741824,35184372088832}, 
    {6,32769, 1073741825, 35184372088833, 1073774592, 
     35184372121600, 35185445830656}, 
    {4,1073774593, 35184372121601, 35185445830657, 
     35185445863424}, 
    {1,35185445863425}};
  uint64_t tmpfilter;
  int i,j;

  for(i=1; i<filter[pairSize-1][0]+1; i++){
    for(j=0; j<15; j++){
      tmpfilter= (filter[pairSize-1][i] << j);
      if(countBit(bCards & tmpfilter) == pairSize-1){
        //printf("=---\n");
        //printBitboard(tmpfilter);
        //printBitboard(bCards & tmpfilter);
        //printBitboard(~bCards & tmpfilter);
        boardInfo tmp = bitboard2BoardInfo(tmpfilter, ~bCards & tmpfilter);
        pushBoardStack(board, tmp);
      }
    }
  }
}

void analyzeBoard(boardInfo *target ,int source[8][15]){
  int i,j;
  uint64_t tmpCard;
  uint64_t bitboard=0;
  uint64_t filter[10]={0,1,3,7,15,31,63,127,255,511};
  uint64_t tmpfilter;
  int flag=0;

  for(i=0; i<4;i++){
    for(j=0; j<15;j++){
      if(source[i][j]==1){
        bitboard=bitboard+1;
      }
      bitboard=bitboard<<1;
    }
  }
  bitboard=bitboard>>1;

  target->cards=bitboard;
  target->joker=0;
  target->numOfCards=countBit(bitboard);

  tmpCard=bitboard&(-bitboard);
  target->SCard= (tmpCard | (tmpCard >> 15) | (tmpCard >> 30) | (tmpCard >> 45))&32767;

  for(i=0; i<4; i++){
    for(j=0; j<15-3; j++){
      tmpfilter= (filter[3] << (15*i + j));
      if((bitboard & tmpfilter) == tmpfilter){
        flag=1;
      }
    }
  } 

  if(target->numOfCards==0){
    target->WCard=0;
    target->cardType=0;
  }else if(target->numOfCards==1){
    target->suit=tmpCard/target->SCard;
    target->suit= (target->suit | (target->suit >> 14) | (target->suit >> 28) | (target->suit >> 42))&32767;
    target->WCard=target->SCard;
    target->cardType=1;
  }else if(flag){
    target->suit=tmpCard/target->SCard;
    target->suit= (target->suit | (target->suit >> 14) | (target->suit >> 28) | (target->suit >> 42))&32767;
    target->WCard=target->SCard << (target->numOfCards-1);
    target->cardType=3;
  }else{
    target->suit=bitboard/target->SCard;
    target->suit= (target->suit | (target->suit >> 14) | (target->suit >> 28) | (target->suit >> 42))&32767;
    target->WCard=target->SCard;
    target->cardType=2;
  }

  return;
}

/* 合法手をスタックに詰める */
void generateHands(uint64_t cards, boardStack *board){
  int number;
  board->top = -1;

  searchSequence(cards, 1, board);
  for(number=3; number< 10; number++){
    searchSequence(cards, number, board);
  }
  for(number=2; number< 5; number++){
    searchPair(cards, number, board);
  }

  if((cards>>60)%2){
    for(number=3; number< 10; number++){
      searchSequenceWJoker(cards, number, board);
    }
    for(number=2; number< 5; number++){
      searchPairWJoker(cards, number, board);
    }
  }
}

void pushBoardStack(boardStack *boards, boardInfo source){
  boards->top++;
  memset(&(boards->board[boards->top]),0,sizeof(boardInfo));
  boards->board[boards->top] = source; 
  return;
}

boardInfo popBoardStack(boardStack *boards) {
  boardInfo tmp = boards->board[boards->top];
  boards->top--;
  return tmp;
}
