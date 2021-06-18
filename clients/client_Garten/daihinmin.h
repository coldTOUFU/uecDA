typedef struct state_type
{
  int ord;            // 現在場に出ているカードの強さ．
  int sequence;       // 場に出ているカードが階段か否か．(1 or 0)
  int qty;            // 場に出ているカードの枚数．
  int rev;            // 革命か否か．(1 or 0)
  int b11;            // 11バックか否か．(1 or 0); UEC標準ルール上では不要．
  int lock;           // しばりか否か．(1 or 0)
  int onset;          // 場に何も出ていないか否か．(1 or 0)
  int suit[5];        // 場に出ているスート．

  int player_qty[5];  // 各プレイヤの残り枚数．
  int player_rank[5]; // 各プレイヤのランク．
  int seat[5];        // 各座席のプレイヤ番号．

  int joker;          // 自分がJokerを持っているか否か．(1 or 0)
} state_type;

state_type state;

void showState(struct state_type *state);
void getField(int cards[8][15]);
void getState(int cards[8][15]);
void getField(int cards[8][15]);

void cardsOr(int cards1[8][15],int cards2[8][15]);
void cardsAnd(int cards1[8][15],int cards2[8][15]);
void cardsDiff(int cards1[8][15],int cards2[8][15]);
void cardsNot(int cards[8][15]);

void outputTable(int table_val[8][15]);
void copyTable(int dest_table[8][15],int org_table[8][15]);
void copyCards(int cardsTar[8][15],int cardsOrg[8][15]);
void clearCards( int cards[8][15]);
void fillCards(int cards[8][15]);  
void clearTable( int cards[8][15]);
int beEmptyCards(int cards[8][15]);
int qtyOfCards(int cards[8][15]);

void updateOpponentsCards(int opponents_cards[8][15], int observed_cards[8][15]);

void lowSolo(int out_cards[8][15],int my_cards[8][15],int joker_flag);

void change(int out_cards[8][15],int my_cards[8][15],int num_of_change);

void select_cards(int out_cards[8][15],int my_cards[8][15], int opponent_cards[8][15]);

int cmpCards(int cards1[8][15],int  cards2[8][15]);
int cmpState(struct state_type* state1,struct state_type* state2);

int getLastPlayerNum(int ba_cards[8][15]);
