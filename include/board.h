#include <chess.h>

void make_move(Board* board, int move);
void san_move(Board* board, const char* san);
void copy_board(Board* new, const Board* old);
Board* create_board();
