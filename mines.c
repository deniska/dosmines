#include <stdio.h>
#include <graph.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

struct cell {
    bool open;
    bool mine;
    bool flag;
    int n;
};

struct cell board[500];
bool game_over = false;
bool win = false;
int board_width = 9;
int board_height = 9;
int mine_count = 10;
int cur_x = 0;
int cur_y = 0;
int difficulty = 0;

void draw_board(void);
void generate_board(void);
void open_cell(int, int);
void open_action(int, int);
bool check_win(void);
void toggle_flag(void);

int main(int argc, char** argv) {
    srand(time(NULL));
    _setvideomode(_VRES16COLOR);
    generate_board();
    draw_board();
    bool quit = false;
    char c;
    while (!quit) {
        c = getch();
        if (c == 0) {
            c = getch();
            if (!game_over && !win) {
                if (c == 72 && cur_y > 0) {
                    cur_y--;
                } else if (c == 80 && cur_y < board_height - 1) {
                    cur_y++;
                } else if (c == 77 && cur_x < board_width - 1) {
                    cur_x++;
                } else if (c == 75 && cur_x > 0) {
                    cur_x--;
                }
            }
        } else if (c == 27) {
            quit = true;
        } else if (c == 'n') {
            generate_board();
        } else if (c == '1') {
            difficulty = 0;
        } else if (c == '2') {
            difficulty = 1;
        } else if (c == '3') {
            difficulty = 2;
        } else if (c == 13 && !game_over) {
            open_action(cur_x, cur_y);
            win = check_win();
        } else if (c == 'f' && !game_over && !win) {
            toggle_flag();
        }
        draw_board();
    }
    _setvideomode(_DEFAULTMODE);
    return 0;
}

int cell_size = 20;
int offset_x = 20;
int offset_y = 20;

void generate_board(void) {
    _clearscreen(0);
    if (difficulty == 0) {
        board_width = 9;
        board_height = 9;
        mine_count = 10;
        cell_size = 40;
        offset_x = 150;
        offset_y = 70;
    } else if (difficulty == 1) {
        board_width = 16;
        board_height = 16;
        mine_count = 40;
        cell_size = 25;
        offset_x = 20;
        offset_y = 20;
    } else if (difficulty = 2) {
        board_width = 30;
        board_height = 16;
        mine_count = 99;
        cell_size = 20;
        offset_x = 20;
        offset_y = 20;
    }
    cur_x = 0;
    cur_y = 0;
    game_over = false;
    win = false;
    for (int i = 0; i < board_width; i++) {
        for (int j = 0; j < board_height; j++) {
            int idx = board_width * j + i;
            board[idx].open = false;
            board[idx].mine = false;
            board[idx].flag = false;
            board[idx].n = 0;
        }
    }
    int cur_mines = 0;
    while (cur_mines < mine_count) {
        int x = rand() % board_width;
        int y = rand() % board_height;
        int idx = board_width * y + x;
        if (board[idx].mine) {
            continue;
        }
        board[idx].mine = true;
        cur_mines++;
        for (int i=-1; i<2; i++) {
            for (int j=-1; j<2; j++) {
                if (x + i >= 0 && x + i < board_width
                        && y + j >= 0 && y + j < board_height) {
                    int idx1 = board_width * (y + j) + x + i;
                    board[idx1].n++;
                }
            }
        }
    }
}

char buf[2];
void draw_board(void) {
    buf[1] = '\0';
    for (int i = 0; i < board_width; i++) {
        for (int j = 0; j < board_height; j++) {
            int idx = board_width * j + i;
            if (!board[idx].open) {
                _setcolor(8);
            } else {
                _setcolor(7);
            }
            if (cur_x == i && cur_y == j) {
                _setcolor(15);
            }
            _rectangle(_GFILLINTERIOR,
                    i*cell_size + offset_x - cell_size/2,
                    j*cell_size + offset_y - cell_size/2,
                    i*cell_size + offset_x + cell_size/2 - 2,
                    j*cell_size + offset_y + cell_size/2 - 2
                    );
            _setcolor(1);
            _moveto(i*cell_size + offset_x, j*cell_size + offset_y);
            if (board[idx].mine && game_over) {
                _setcolor(4);
                _outgtext("*");
            } else if (board[idx].mine && win) {
                _setcolor(4);
                _outgtext("F");
            } else if (board[idx].n > 0 && board[idx].open) {
                if (board[idx].n == 1) {
                    _setcolor(3);
                } else if (board[idx].n == 2) {
                    _setcolor(2);
                } else if (board[idx].n == 3) {
                    _setcolor(12);
                } else if (board[idx].n == 4) {
                    _setcolor(1);
                } else if (board[idx].n == 5) {
                    _setcolor(4);
                } else if (board[idx].n == 6) {
                    _setcolor(6);
                } else if (board[idx].n == 7) {
                    _setcolor(13);
                } else if (board[idx].n == 8) {
                    _setcolor(10);
                }
                buf[0] = '0' + board[idx].n;
                _outgtext(&buf);
            } else if (board[idx].flag) {
                _setcolor(4);
                _outgtext("F");
            }
        }
    }
}

bool check_win(void) {
    for (int i = 0; i < board_width; i++) {
        for (int j = 0; j < board_height; j++) {
            int idx = board_width * j + i;
            struct cell c = board[idx];
            if (!c.open && !c.mine) {
                return false;
            } else if (c.open && c.mine) {
                return false;
            }
        }
    }
    return true;
}

void open_cell(int x, int y) {
    int idx = y * board_width + x;
    if (board[idx].open || board[idx].flag) {
        return;
    }
    board[idx].open = true;
    if (board[idx].mine) {
        game_over = true;
        return;
    }
    if (board[idx].n > 0) {
        return;
    }
    for (int i=-1; i<2; i++) {
        for (int j=-1; j<2; j++) {
            if (x + i >= 0 && x + i < board_width
                    && y + j >= 0 && y + j < board_height) {
                int idx1 = (y + j) * board_width + x + i;
                board[idx1].flag = false;
                open_cell(x + i, y + j);
            }
        }
    }
}

void open_action(int x, int y) {
    int idx = y * board_width + x;
    if (!board[idx].open) {
        open_cell(x, y);
        return;
    }
    int flags_around = 0;
    for (int i=-1; i<2; i++) {
        for (int j=-1; j<2; j++) {
            if (x + i >= 0 && x + i < board_width
                    && y + j >= 0 && y + j < board_height) {
                int idx1 = (y + j) * board_width + x + i;
                if (board[idx1].flag) {
                    flags_around++;
                }
            }
        }
    }
    if (flags_around == board[idx].n) {
        for (int i=-1; i<2; i++) {
            for (int j=-1; j<2; j++) {
                if (x + i >= 0 && x + i < board_width
                        && y + j >= 0 && y + j < board_height) {
                    open_cell(x+i, y+j);
                }
            }
        }
    }
}

void toggle_flag(void) {
    int idx = cur_y * board_width + cur_x;
    if (board[idx].open) {
        return;
    }
    board[idx].flag = !board[idx].flag;
}
