#define INFINITY_EVAL_L 100000
#define INFINITY_EVAL_M 400
#define INFINITY_EVAL_S 20

#ifndef DAIHINMIN_H
#define DAIHINMIN_H
#include "daihinmin.h"
#endif

#ifndef SEARCH_CARDS_H
#define SEARCH_CARDS_H
#include "search_cards.h"
#endif

int evaluate_hand(boardInfo own_card, boardStack *opponent_cards, state_type *state, state_type *state_simulated);
int default_evaluate_hand(boardInfo own_card, state_type *state);
int isPossibleHand(boardInfo own_cards, state_type *state);
