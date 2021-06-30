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

#ifndef SELECT_CARDS_H
#define SELECT_CARDS_H
#include "select_cards.h"
#endif

int evaluate_hand(board_info my_hand, board_stack *opponent_hands, state_type *state, state_type *state_simulated) {
  int abs_eval = default_evaluate_hand(my_hand, state);
  int rel_eval = 0;

  /* 相手の手を評価値として考えるときは，相手により強い手を強いるような着手を評価するために，
     revを反転させて強い手ほど大きい評価値になるようにする(自分の手を評価するときは，強い手を温存するために逆になる)． */
  state_simulated->rev != state_simulated->rev;

  int opponent_evals[BOARDSTACK_SIZE] = {0};
  while (opponent_hands->top >= 0) {
    board_info opponent_card = pop_board_stack(opponent_hands);
    int eval = default_evaluate_hand(opponent_card, state_simulated);
    opponent_evals[opponent_hands->top + 1] = eval; // 後で整列するので場所はどうでもいい．
  }
  quicksort(opponent_evals, 0, BOARDSTACK_SIZE-1);
  for (int i=1; i<=5; i++) { // 上から5つ分．5はテキトーな数字．
    int tmp = opponent_evals[BOARDSTACK_SIZE-i];
    rel_eval += (tmp > 0 ? tmp : 0);
  }
  
  return abs_eval + 0.2*rel_eval; // 上でrel_evalは評価値5つ分なので，1/5倍してみる．
}

int evaluate_hand_left(board_stack *my_hands_left, board_stack *opponent_hands, state_type *state, state_type *state_simulated) {
  int left_eval = 0;

  int my_hand_evals[BOARDSTACK_SIZE] = {0};
  while (my_hands_left->top>=0) {
    board_info my_hand = pop_board_stack(my_hands_left);
    int eval = evaluate_hand(my_hand, opponent_hands, state, state_simulated);
    my_hand_evals[my_hands_left->top + 1] = eval;
  }

  quicksort(my_hand_evals, 0, BOARDSTACK_SIZE-1);
  for (int i=1; i<=5; i++) { // 上から5つ分．5はテキトーな数字．
    int tmp = my_hand_evals[BOARDSTACK_SIZE-i];
    left_eval += (tmp > 0 ? tmp : 0);
  }
  
  return left_eval * 0.2; // 上でleft_evalは評価値5つ分なので，1/5倍してみる．
}

int default_evaluate_hand(board_info hands, state_type *state) {
  if (!state->onset && !is_possible_hand(hands, state)) {
    return -INFINITY_EVAL_L;
  }

  int eval = 0;
  if (state->onset) {
    if (hands.card_type == SEQUENCE_HAND) {
      eval += INFINITY_EVAL_M;
    }
    eval += hands.num_of_cards * INFINITY_EVAL_S;
  }

  /* SCardをiだけ右シフトしたとき1の位に1が立てば，
     このiはSCardの強さが最強から何番目かを表す． */
  /* よって，そのときのiを評価値に足せば，弱いカードほど
     優先して出しやすくなる． */
  /* 革命の場合は，逆にする． */
  if (!state->rev) {
    for (int i=1; i<=14; i++) {
      if ((hands.SCard >> i) % 2) { eval += i; }
    }
  }
  else {
    for (int i=0; i<14; i++) {
      if ((hands.WCard >> i) % 2) { eval += (14-i); }
    }
  }

  return eval;
}

int is_possible_hand(board_info hands, state_type *state) {
  if (state->onset) { return 1; }

  /* 場と同じ種類の手である必要がある． */
  if (state->sequence && hands.card_type != SEQUENCE_HAND)  { return 0; }
  if (!state->sequence && hands.card_type == SEQUENCE_HAND) { return 0; }

  /* 場のカードと同枚数である必要がある． */
  if (state->qty != hands.num_of_cards) { return 0; }

  /* 出すカードの最小が場のカードの最大より強い必要がある． */
  /* state->ordは0..14の整数で，大きいほど強い．0と14はJoker用． */
  /* 対して，hands.SCardは15bit整数で，小さいほど強い． */
  /* そのため，14-state->ordで小さいほど強くさせ，さらにbitシフトでSCardと比較可能にする． */
  uint64_t state_SCard = (1 << (14-state->ord));
  if (!state->rev && state_SCard <= hands.WCard) { return 0; }
  if (state->rev  && state_SCard >= hands.SCard) { return 0; }

  /* しばりなら，スートが一致する必要がある． */
  if (state->lock) {
    uint64_t state_suit = (state->suit[0] << 3) + (state->suit[1] << 2) + (state->suit[2] << 1) + state->suit[3];
    if (state_suit != hands.suit) { return 0; }
  }

  return 1;
}
