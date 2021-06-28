typedef struct state_type {
  int ord;            // 場のカードの強さ．
  int sequence;      // 場が階段か．
  int qty;            // 場の枚数．
  int rev;           // 場が革命か．
  int b11;            // 場が11バックか．(未使用)
  int lock;          // 場が縛られてるか．
  int onset;          // 場が空か．
  int suit[5];        // 場のスート．

  int player_qty[5];  // 各プレイヤの手札枚数．
  int player_rank[5]; // 各プレイヤの階級．
  int seat[5];        // 各プレイヤの座り位置．

  int joker;         // 自分がjokerを持っているか．
} state_type;

state_type state;

void set_state_from_own_cards(int cards[8][15]);
void set_state_from_table(int cards[8][15]);
state_type simulate_state(int cards[8][15]);
void print_state(struct state_type *state);
int is_same_cards(int cards1[8][15], int  cards2[8][15]);

void cards_OR(int passive[8][15], int active[8][15]);
void cards_AND(int passive[8][15], int active[8][15]);
void cards_DIFF(int passive[8][15], int active[8][15]);
void cards_NOT(int passive[8][15]);

void print_cards(int cards[8][15]);
void copy_cards(int dest_cards[8][15], int org_cards[8][15]);
void clear_cards(int cards[8][15]);
int qty_of_cards(int cards[8][15]);
int is_cards_empty(int cards[8][15]);

void init_opponents_cards(int opponents_cards[8][15]);
void update_opponents_cards(int opponents_cards[8][15], int observed_cards[8][15]);

void choose_low_pair(int out_cards[8][15], int my_cards[8][15], int group[8][15]);

void select_change_cards(int out_cards[8][15], int my_cards[8][15], int num_of_change);
void select_cards(int out_cards[8][15], int my_cards[8][15], int opponent_cards[8][15]);
