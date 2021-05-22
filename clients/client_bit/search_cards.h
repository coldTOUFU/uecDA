#include<stdint.h>
#define SINGLE_HAND 0
#define PAIR_HAND 1
#define SEQUENCE_HAND 2

typedef struct tmpboardInfo {
  uint64_t cards;
  uint64_t joker;
  uint64_t WCard;
  uint64_t SCard;
  int numOfCards;
  int cardType;
  uint64_t suit;
} boardInfo;

typedef struct tmpboardStack {
  int top;
  boardInfo board[100];
} boardStack;

boardStack stack;

void pushBoardStack(boardStack *boards, boardInfo source);
boardInfo popBoardStack(boardStack *boards);

uint64_t cards2Bitboard(int source[8][15]);
boardInfo bitboard2BoardInfo(uint64_t source, uint64_t joker);
void putBoardInfoIntoCards(boardInfo source, int cards[8][15]);

int isContainingOnBit(uint64_t a, uint64_t b);
void printBitboard(uint64_t source);
int countBit(uint64_t source);

void searchSequence(uint64_t source, int number, boardStack *board);
void searchSequenceWJoker(uint64_t bCards, int number, boardStack *board);
void searchPair(uint64_t bCards, int number, boardStack *board);
void searchPairWJoker(uint64_t bCards, int number, boardStack *board);
void generateHands(uint64_t cards, boardStack *board);

void alanyzeBoard(boardInfo target ,int source[8][15]);

