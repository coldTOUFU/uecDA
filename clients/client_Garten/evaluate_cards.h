#define INFINITY_EVAL_L 100000
#define INFINITY_EVAL_M 400
#define INFINITY_EVAL_S 20

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
int is_possible_hand(board_info hands, state_type *state);
