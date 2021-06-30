#include<stdint.h>
#include<stdio.h>

#define SINGLE_HAND 0
#define PAIR_HAND 1
#define SEQUENCE_HAND 2

#define BOARDSTACK_SIZE 500

typedef struct board_info {
  uint64_t cards;
  uint64_t joker;
  uint64_t WCard;
  uint64_t SCard;
  int num_of_cards;
  int card_type;
  uint64_t suit;
  int eval;
} board_info;

typedef struct board_stack {
  int top;
  board_info board[BOARDSTACK_SIZE];
} board_stack;

board_stack stack;

void push_board_stack(board_stack *boards, board_info source);
board_info pop_board_stack(board_stack *boards);

uint64_t cards2bitboard(int source[8][15]);
board_info bitboard2board_info(uint64_t source, uint64_t joker);
void put_board_info_into_cards(board_info source, int cards[8][15]);
void put_bitboard_into_cards(uint64_t source, int cards[8][15]);

int is_inclusion(uint64_t a, uint64_t b);
void print_bitboard(uint64_t source);
void print_board_info(board_info source);
int count_bit(uint64_t source);

void search_sequence(uint64_t source, int number, board_stack *board);
void search_sequence_with_joker(uint64_t bCards, int number, board_stack *board);
void search_pair(uint64_t bCards, int number, board_stack *board);
void search_pair_with_joker(uint64_t bCards, int number, board_stack *board);
void generate_hands(uint64_t cards, board_stack *board);
