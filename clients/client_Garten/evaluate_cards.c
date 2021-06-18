#ifndef EVALUATE_CARDS_H
#define EVALUEATE_CARDS_H
#include "evaluate_cards.h"
#endif

#ifndef UTIL_H
#define UTIL_H
#include "util.h"
#endif

#ifndef SEARCH_CARDS_H
#define SEARCH_CARDS_H
#include "search_cards.h"
#endif

#ifndef DAIHINMIN_H
#define DAIHINMIN_H
#include "daihinmin.h"
#endif

int evaluate_hand(boardInfo own_card, boardStack *opponent_cards, state_type *state, state_type *state_simulated) {
  int abs_eval = default_evaluate_hand(own_card, state);
  int rel_eval = 0;

  int opponent_evals[BOARDSTACK_SIZE] = {0};
  while (opponent_cards->top >= 0) {
    boardInfo opponent_card = popBoardStack(opponent_cards);
    int eval = default_evaluate_hand(opponent_card, state_simulated);
    opponent_evals[opponent_cards->top + 1] = eval; // 後で整列するので場所はどうでもいい．
  }
  
  quicksort(opponent_evals, 0, BOARDSTACK_SIZE-1);
  for (int i=1; i<=5; i++) { // 上から5つ分．5はテキトーな数字．
    rel_eval -= opponent_evals[BOARDSTACK_SIZE-i];
  }
  
  return abs_eval + 0.2*rel_eval; // 上でrel_evalは評価値5つ分なので，1/5倍してみる．
}

int default_evaluate_hand(boardInfo own_card, state_type *state) {
  if (!state->onset && !isPossibleHand(own_card, state)) {
    return -INFINITY_EVAL_L;
  }

  int eval = 0;
  if (state->onset) {
    if (own_card.cardType == SEQUENCE_HAND) {
      eval += INFINITY_EVAL_M;
    }
    eval += own_card.numOfCards * INFINITY_EVAL_S;
  }

  if (!state->rev) {
    int i;
    for (i=1; i<=14; i++) {
      eval += (own_card.SCard >> i)*i;
    }
  }
  else {
    int i;
    for (i=0; i<14; i++) {
      eval += (own_card.WCard >> i)*(14-i);
    }
  }

  return eval;
}

int isPossibleHand(boardInfo own_card, state_type *state) {
  if (state->onset) { return 1; }

  /* 場と同じ種類の手である必要がある． */
  if (state->sequence && own_card.cardType != SEQUENCE_HAND)  { return 0; }
  if (!state->sequence && own_card.cardType == SEQUENCE_HAND) { return 0; }

  /* 場のカードと同枚数である必要がある． */
  if (state->qty != own_card.numOfCards) { return 0; }

  /* 出すカードの最小が場のカードの最大より強い必要がある． */
  /* state->ordは0..14の整数で，大きいほど強い．0と14はJoker用． */
  /* 対して，own_card.SCardは15bit整数で，小さいほど強い． */
  /* そのため，14-state->ordで小さいほど強くさせ，さらにbitシフトでSCardと比較可能にする． */
  uint64_t state_SCard = (1 << (14-state->ord));
  printf("state->ord: %d\n", state->ord);
  printf("state_SCard: %ld\n", state_SCard);
  printf("own_WCard: %ld\n", own_card.WCard);
  if (!state->rev && state_SCard <= own_card.WCard) { return 0; }
  if (state->rev  && state_SCard >= own_card.SCard) { return 0; }

  /* しばりなら，スートが一致する必要がある． */
  if (state->lock) {
    uint64_t state_suit = (state->suit[0] << 3) + (state->suit[1] << 2) + (state->suit[2] << 1) + state->suit[3];
    if (state_suit != own_card.suit) { return 0; }
  }

  return 1;
}
