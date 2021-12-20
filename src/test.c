#include <test.h>
#include <moveGenerator.h>
#include <board.h>
#include <stdio.h>
#include <time.h>

#define PROG_BAR_LEN 30

void updateBar(int percent) {
    int numChars = percent * PROG_BAR_LEN / 100;
    printf("[");
    for (int i = 0; i < numChars; i++) printf("#");
    for (int i = 0; i < PROG_BAR_LEN-numChars; i++) printf(" ");
    printf("] %d%%", percent);
    fflush(stdout);
}

void time_str(char* time, clock_t diff) {
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    int milli   = msec % 1000;
    int seconds = (msec / 1000) % 1000;
    int minutes = (seconds / 60) % 60;
    int hours   = minutes / 60;

    if (hours > 0) sprintf(time, "%dh %dm", hours, minutes);
    else if (minutes > 0) sprintf(time, "%dm %ds", minutes, seconds);
    else sprintf(time, "%ds %dms", seconds, milli);
}

long run_possible(Board* boards, int depth) {
    if (depth == 0) return 1;
    long total = 0;
    generate_legal(&boards[depth]);
    
    for (int i = 0; i < boards[depth].moves.count; i++) {
        copy_board(&boards[depth-1], &boards[depth]);
        make_move(&boards[depth-1], boards[depth].moves.moves[i]);
        total += run_possible(boards, depth-1);
    }
    
    return total;
}

long possible(Board* start, int depth) {
    Board boards[depth];

    long cur, total = 0;
    int move, src, dest;
    char san[10], fen[100];
    generate_legal(start);
    
    to_fen(fen, start);
    printf("fen: %s\n", fen);
    
    for (int i = 0; i < start->moves.count; i++) {
        copy_board(&boards[depth-1], start);
        move = start->moves.moves[i];

        make_move(&boards[depth-1], move);
        cur = run_possible(boards, depth-1);

        src = get_move_source(move);
        dest = get_move_target(move);
        printf("%s%s: %ld\n", sq_str[src], sq_str[dest], cur);
        
        total += cur;
    }

    printf("\n\tTotal: %ld\n\n", total);

    return total;
}

long possible_fen(const char* fen, int depth) {
    Board board;
    from_fen(fen, &board);
    possible(&board, depth);
}

void perftest(const char* fen, int maxDepth, long* expected) {
    Board boards[maxDepth+1];
    from_fen(fen, &boards[maxDepth]);

    clock_t start, diff;
    char time[10];
    long total;

    for (int i = 0; i <= maxDepth; i++) copy_board(&boards[i], &boards[maxDepth]);
    
    print_board(&boards[maxDepth]);
    for (int depth = 1; depth <= maxDepth; depth++) {
        total = 0;
        start = clock();
        generate_legal(&boards[depth]);
        for (int i = 0; i < boards[depth].moves.count; i++) {
            printf("\r[");
            printf(YEL "waiting" RESET);
            printf("]\t");
            updateBar(100 * i / boards[depth].moves.count);
            copy_board(&boards[depth-1], &boards[depth]);
            make_move(&boards[depth-1], boards[depth].moves.moves[i]);
            total += run_possible(boards, depth-1);
        }
        diff = clock() - start;
        time_str(time, diff);
        printf("\r[");
        if (total == expected[depth-1]) printf(GRN "passed" RESET);
        else printf(RED "failed" RESET);
        printf("] \tdepth: %d nodes: %ld time: %s        \n", depth, total, time);
    }
}

void runMoves(Board* board) {
    Board copy;
    generate_legal(board);
    
    for (int i = 0; i < board->moves.count; i++) {
        copy_board(&copy, board);
        make_move(&copy, board->moves.moves[i]);
        print_move(board->moves.moves[i]);
        print_board(&copy);
    }
}
