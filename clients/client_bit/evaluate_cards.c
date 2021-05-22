#ifndef EVALUATE_CARDS_H
#define EVALUEATE_CARDS_H
#include "evaluate_cards.h"
#endif

int evaluateHand(boardInfo own_cards, state_type *state) {
  if (!isPossibleHand(own_cards, state)) {
    return -INFINITY_EVAL;
  }

  int eval = 0;
  if (state->onset) {
    if (own_cards.cardType == SEQUENCE_HAND) {
      eval += 10000;
    }
    eval += own_cards.numOfCards * 100;
  }

  if (!state->rev) {
    int i;
    for (i=1; i<=14; i++) {
      eval += (own_cards.SCard >> i)*i;
    }
  }
  else {
    int i;
    for (i=0; i<14; i++) {
      eval += (own_cards.WCard >> i)*(14-i);
    }
  }

  return eval;
}

int isPossibleHand(boardInfo own_cards, state_type *state) {
  if (state->onset) { return 1; }

  /* 場と同じ種類の手である必要がある． */
  if (state->sequence && own_cards.cardType != SEQUENCE_HAND) { return 0; }
  else if (own_cards.cardType == SEQUENCE_HAND)              { return 0; }

  /* 場のカードと同枚数である必要がある． */
  if (state->qty != own_cards.numOfCards) { return 0; }

  /* 場のカードより強い必要がある． */
  /* state->ordは0..14の整数で，大きいほど強い．0と14はJoker用． */
  /* 対して，own_cards.SCardは15bit整数で，小さいほど強い． */
  /* そのため，14-state->ordで小さいほど強くさせ，さらにbitシフトでSCardと比較可能にする． */
  uint64_t state_SCard = (1 << (14-state->ord) >> 1);
  if (!state->rev && state_SCard >= own_cards.SCard) { return 0; }

  uint64_t state_WCard = (1 << (14-state->ord) >> 1);
  if (state->rev && state_WCard <= own_cards.WCard) { return 0; }
  
  /* しばりなら，スートが一致する必要がある． */
  if (state->lock) {
    uint64_t state_suit = (state->suit[0] << 3) + (state->suit[1] << 2) + (state->suit[2] << 1) + state->suit[0];
    if (state_suit != own_cards.suit) { return 0; }
  }

  return 1;
}
