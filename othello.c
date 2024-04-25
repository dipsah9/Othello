#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

typedef struct {
    int x;
    int y;
} Position;

Position make_position(int x, int y) {
    Position p = { x, y };
    return p;
}

void print_position(Position p) {
    printf("(%d, %d)\n", p.x, p.y);
}

#define N 8

typedef struct {
    char board[N][N]; 
    char my_stone; 
} Game;

Game init_game(char my_stone) {
    Game g;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            g.board [i][j] = '_';
        }
    }
    int a = N / 2;
    int b = a - 1;
    g.board[b][b] = 'O';
    g.board[a][b] = 'X';
    g.board[b][a] = 'X';
    g.board[a][a] = 'O';
    g.my_stone = my_stone;
    return g;
}

void print_board(Game* g) {
    printf(" |A|B|C|D|E|F|G|H|\n");
    for (int i = 0; i < N; i++) {
        printf("%d|", i + 1);
        for (int j = 0; j < N; j++) {
            printf("%c|", g->board[j][i]);
        }
        printf("\n");
    }
}

bool out_of_bounds(int x, int y) {
    return x < 0 || x > N - 1 || y < 0 || y > N - 1;
}

char my_stone(Game* g) {
    return g->my_stone;
}

char your_stone(Game* g) {
    return (g->my_stone == 'X') ? 'O' : 'X';
}

void switch_stones(Game *g) {
    g->my_stone = (g->my_stone == 'X') ? 'O' : 'X';
}

//checks if the direction is legal
bool legal_dir(Game* g, int x, int y, int dx, int dy) {
    x += dx; y += dy;
    while (!out_of_bounds(x, y) && (g->board[x][y] == your_stone(g))) {
        x += dx; y += dy;
        if (!out_of_bounds(x, y) && (g->board[x][y] == my_stone(g))) {
            return true;
        }
    }
    return false;
}

bool legal(Game* g, int x, int y) {
    return !out_of_bounds(x, y) && (g->board[x][y] == '_') &&
       (legal_dir(g, x, y, -1, -1) || legal_dir(g, x, y, 0, -1) ||
        legal_dir(g, x, y, 1, -1) || legal_dir(g, x, y, -1, 0) ||
        legal_dir(g, x, y, 1, 0) || legal_dir(g, x, y, -1, 1) ||
        legal_dir(g, x, y, 0, 1) || legal_dir(g, x, y, 1, 1));
}

void print_board_helper (Game *g){
    printf(" |A|B|C|D|E|F|G|H|\n");
    for (int i = 0; i < N; i++) {
        printf("%d|", i + 1);
        for (int j = 0; j < N; j++) {
           if(legal(g, j, i)){
            printf("|*");
           }else{
             printf("|%c", g->board[j][i]);
           }
        }
        printf("|\n");
    }
}

void reverse_dir(Game* g, int x, int y, int dx, int dy) {
    if (!legal_dir(g, x, y, dx, dy)) return;
    do {
        g->board[x][y] = my_stone(g);
        x += dx; y += dy;
    } while (!out_of_bounds(x, y) && (g->board[x][y] == your_stone(g)));
}

void reverse(Game* g, int x, int y) {
    reverse_dir(g, x, y, -1, -1); reverse_dir(g, x, y, 0, -1);
    reverse_dir(g, x, y, 1, -1); reverse_dir(g, x, y, -1, 0);
    reverse_dir(g, x, y, 1, 0); reverse_dir(g, x, y, -1, 1);
    reverse_dir(g, x, y, 0, 1); reverse_dir(g, x, y, 1, 1);
}

Position human_move(Game* g) {
    char input[10];
    fgets(input, sizeof(input), stdin);
    if (input[0] == 'q') exit(0);
    Position pos = { -1, -1 };
    if (isalpha(input[0]) && isdigit(input[1])) {
        pos.x = tolower(input[0]) - 'a';
        pos.y = input[1] - '1';
    }
    if (legal(g, pos.x, pos.y)) {
        return pos;
    } else {
        if(input[0] == '?') {print_board_helper(g);}
        else{printf("Invalid position!\n");}
        return human_move(g);
    }
}

int count_stones(Game *g, char c) {
    int result = 0;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            if (g->board[x][y] == c) {
                result++;
            }
        }
    }
    return result;
}

#define POSITION_STACK_SIZE 64
typedef struct {
    int length;
    Position values[POSITION_STACK_SIZE];
} PositionStack;

PositionStack make_position_stack() {
    PositionStack ps;
    ps.length = 0;
    return ps;
}

void push(PositionStack *ps, Position p) {
    if (ps->length == POSITION_STACK_SIZE -1 ){
        printf("Overflow");
     } 
    else { 
         ps->values[ps->length++] = p;
    }

}

Position pop(PositionStack *ps) {
    Position tmp = { -1,-1};
    if(ps->length > 0){
        tmp = ps->values[ps->length - 1];
        ps->length--;
        return tmp;
    }
    return tmp;    
}

Position random_position(PositionStack *ps) {
    int random = rand() % ps->length;
    return ps->values[random];
}

Position computer_move(Game *g) {
    bool valid_position = false;
    PositionStack make_stack = make_position_stack();
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if(legal(g,i,j)){
                valid_position = true;
                push(&make_stack,make_position(i,j));
            }
        } 
    }

    if (!valid_position){
        return make_position(-1,-1);
    } 

    return random_position(&make_stack);
}

int main(void) {
    srand((unsigned int)time(NULL));
    Game g = init_game('X');
    print_board(&g);
    while (true) {
        printf("%c's move: ", my_stone(&g));
        Position pos = human_move(&g);
        reverse(&g, pos.x, pos.y);
        print_board(&g);
        int score = count_stones(&g, my_stone(&g)) - count_stones(&g, your_stone(&g));
        printf("Score for %c: %d\n", my_stone(&g), score);
        switch_stones(&g);

	printf("<---After X's move--->\n");	
        printf("%c's move:\n ", my_stone(&g));
        pos = computer_move(&g);
        print_position(pos);
        reverse(&g, pos.x, pos.y);
        print_board(&g);
        score = count_stones(&g, my_stone(&g)) - count_stones(&g, your_stone(&g));
        printf("Score for %c: %d\n", my_stone(&g), score);
        switch_stones(&g);
    }
    return 0;
}
