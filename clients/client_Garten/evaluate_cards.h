#define INFINITY_EVAL_L 100000
#define INFINITY_EVAL_M 400
#define INFINITY_EVAL_S 20

#define PICKING_SIZE 7 // 着手集合から評価値をいくつか拾うとき，いくつ拾うか．

#ifndef SELECT_CARDS_H
#define SELECT_CARDS_H
#include "select_cards.h"
#endif

#ifndef SEARCH_CARDS_H
#define SEARCH_CARDS_H
#include "search_cards.h"
#endif

int evaluate_hand(board_info my_hands, board_stack *opponent_hands, state_type *state, state_type *state_simulated);
int evaluate_hand_left(board_stack *my_hands_left, board_stack *opponent_hands, state_type *state, state_type *state_simulated);
int default_evaluate_hand(board_info hands, state_type *state);
int evaluate_exceptional_hand(board_info hand);
int is_possible_hand(board_info hands, state_type *state);
