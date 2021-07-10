#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

#ifndef SELECT_CARDS_H
#define SELECT_CARDS_H
#include "select_cards.h"
#endif

const int g_logging = 0;      // ログ取りをするか否かを判定するための変数
int opponents_cards[8][15]; // 相手全体のカードテーブル

void read_from_tsv(char *filename, int my_cards[8][15], int opponents_cards[8][15]) {
  /* tsvの読込． */
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("ファイル %s を開けませんでした．\n", filename);
    exit(1);
  }
  for (int i=0; i<4; i++) {
    for (int j=0; j<15; j++) {
      if (fscanf(fp, "%d", &(my_cards[i][j])) == EOF) {
        printf("ファイル %s の読込途中(my_cards[%d][%d])で終端に達しました．\n", filename, i, j);
        exit(1);
      }
    }
  }
  for (int i=0; i<4; i++) {
    for (int j=0; j<15; j++) {
      if (fscanf(fp, "%d", &(opponents_cards[i][j])) == EOF) {
        printf("ファイル %s の読込途中(opponents_cards[%d][%d])で終端に達しました．\n", filename, i, j);
        exit(1);
      }
    }
  }  
  if (fscanf(fp, "%d", &(state.ord)) == EOF) {
    printf("ファイル %s の読込途中(state.ord)で終端に達しました．\n", filename);
    exit(1);
  }
  if (fscanf(fp, "%d", &(state.sequence)) == EOF) {
    printf("ファイル %s の読込途中(state.sequence)で終端に達しました．\n", filename);
    exit(1);
  }
  if (fscanf(fp, "%d", &(state.qty)) == EOF) {
    printf("ファイル %s の読込途中(state.qty)で終端に達しました．\n", filename);
    exit(1);
  }
  if (fscanf(fp, "%d", &(state.rev)) == EOF) {
    printf("ファイル %s の読込途中(state.rev)で終端に達しました．\n", filename);
    exit(1);
  }
  if (fscanf(fp, "%d", &(state.b11)) == EOF) {
    printf("ファイル %s の読込途中(state.b11)で終端に達しました．\n", filename);
    exit(1);
  }
  if (fscanf(fp, "%d", &(state.lock)) == EOF) {
    printf("ファイル %s の読込途中(state.lock)で終端に達しました．\n", filename);
    exit(1);
  }
  if (fscanf(fp, "%d", &(state.onset)) == EOF) {
    printf("ファイル %s の読込途中(state.onset)で終端に達しました．\n", filename);
    exit(1);
  }
  for (int i=0; i<4; i++) {
    if (fscanf(fp, "%d", &(state.suit[i])) == EOF) {
      printf("ファイル %s の読込途中(state.suit[%d])で終端に達しました．\n", filename, i);
      exit(1);
    }
  }
  for (int i=0; i<5; i++) {
    if (fscanf(fp, "%d", &(state.player_qty[i])) == EOF) {
      printf("ファイル %s の読込途中(state.player_qty[%d])で終端に達しました．\n", filename, i);
      exit(1);
    }
  }
  for (int i=0; i<5; i++) {
    if (fscanf(fp, "%d", &(state.player_rank[i])) == EOF) {
      printf("ファイル %s の読込途中(state.player_rank[%d])で終端に達しました．\n", filename, i);
      exit(1);
    }
  }
  for (int i=0; i<5; i++) {
    if (fscanf(fp, "%d", &(state.seat[i])) == EOF) {
      printf("ファイル %s の読込途中(state.seat[%d])で終端に達しました．\n", filename, i);
      exit(1);
    }
  }
  if (fscanf(fp, "%d", &(state.joker)) == EOF) {
    printf("ファイル %s の読込途中(state.joker)で終端に達しました．\n", filename);
    exit(1);
  }
}

void simulate_select_cards(char *filename) {
  int my_cards[8][15] = {{0}};
  int out_cards[8][15] = {{0}};

  read_from_tsv(filename, my_cards, opponents_cards);

  select_cards(out_cards, my_cards, opponents_cards);

  printf("--状況--\n");
  print_state(&state);
  printf("--手札--\n");
  print_cards(my_cards);
  printf("--着手--\n");
  print_cards(out_cards);
}

/* 好きな関数に変えて試す． */
void simulate_card_methods(char *filename) {
  int my_cards[8][15] = {{0}};
  int out_cards[8][15] = {{0}};

  read_from_tsv(filename, my_cards, opponents_cards);

  //suitpass_cards(out_cards, my_cards, tmp);

  printf("--状況--\n");
  print_state(&state);
  printf("--手札--\n");
  print_cards(my_cards);
  printf("--着手--\n");
  print_cards(out_cards);
}


int main(void) {
  simulate_select_cards("simulation.tsv");
  //simulate_card_methods("simulation.tsv");
}