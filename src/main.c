#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIN 0
#define LOSS 1
#define SNAKE_COLOUR 1
#define APPLE_COLOUR 2

static void draw_grid(int x, int y, int score);
static void movement(int key, int *x, int *y);
static void spawn_apple(int *x, int *y, int x_offset, int y_offset, int *score);
static void die(int condition, int score);

typedef struct snake_part {
    int x;
    int y;
    struct snake_part *next;
} head;

int main(void) {
    initscr();
    raw();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    if (has_colors()) {
        start_color();
        init_pair(SNAKE_COLOUR, COLOR_GREEN, COLOR_BLACK);
        init_pair(APPLE_COLOUR, COLOR_RED, COLOR_BLACK);
    }
    else {
        endwin();
        printf("Your terminal does not support colors.\n");
        exit(1);
    }

    int x, y, score;
    score = -10;

    getmaxyx(stdscr, y, x);

    int x_offset = x / 10;
    int y_offset = y / 10;

    int x_pos = x / 2;
    int y_pos = y / 2;

    int *px_pos = &x_pos;
    int *py_pos = &y_pos;

    attron(COLOR_PAIR(SNAKE_COLOUR));
    mvaddch(y_pos, x_pos, 'O');
    attroff(COLOR_PAIR(SNAKE_COLOUR));

    srand(time(NULL));
    int random_x, random_y;
    int *prandom_x = &random_x;
    int *prandom_y = &random_y;
    spawn_apple(prandom_x, prandom_y, x_offset, y_offset, &score);

    draw_grid(x_offset, y_offset, score);
    
    int c;
    while ((c = getch()) != 'q') {
        movement(c, px_pos, py_pos);

        if (x_pos == random_x && y_pos == random_y)
            spawn_apple(prandom_x, prandom_y, x_offset, y_offset, &score);
        
        if (x_pos <= x_offset * 3 || x_pos >= x_offset * 7 || y_pos <= y_offset-1 || y_pos >= y_offset * 9) {
            break;
        }
    }

    endwin();
    die(LOSS, score);

    return 0;
}

void draw_grid(int x, int y, int score) {
    int x_copy, y_copy;
    int limit_x = x * 7;
    int limit_y = y * 9;
    static int displacement = 0;
    
    for (x_copy = x * 3, y_copy = y - 1; x_copy < limit_x + 1; ++x_copy)
        mvprintw(y_copy, x_copy, "X");
    
    for (x_copy = x * 3, y_copy = limit_y; x_copy < limit_x + 1; ++x_copy)
        mvprintw(y_copy, x_copy, "X");

    for (x_copy = x * 3, y_copy = y; y_copy < limit_y; ++y_copy)
        mvprintw(y_copy, x_copy, "X");

    for (x_copy = limit_x, y_copy = y; y_copy < limit_y; ++y_copy)
        mvprintw(y_copy, x_copy, "X");

    if ((score/10) / (pow(10, displacement)) >= 1)
        ++displacement;
    
    mvprintw(limit_y + 1, x * 3, "Score:");
    mvprintw(limit_y + 1, limit_x - displacement, "%d", score);

    refresh();
}

void movement(int key, int *x, int *y) {
    mvaddch(*y, *x, ' ');

    switch (key) {
        case KEY_UP:
            --*y;
            break;
        
        case KEY_DOWN:
            ++*y;
            break;
        
        case KEY_LEFT:
            --*x;
            break;
        
        case KEY_RIGHT:
            ++*x;
            break;
    }

    attron(COLOR_PAIR(SNAKE_COLOUR));
    mvaddch(*y, *x, 'O');
    attroff(COLOR_PAIR(SNAKE_COLOUR));

    refresh();
}

static void spawn_apple(int *x, int *y, int x_offset, int y_offset, int *score) {
    char character;
    
    do {
        *x = rand() % ((x_offset * 7 - 1) + 1 - (x_offset * 3 + 1)) + (x_offset * 3 + 1);
        *y = rand() % ((y_offset * 9) - (y_offset)) + y_offset;
        chtype ch = mvwinch(stdscr, *y, *x);
        character = ch & A_CHARTEXT;
    } while (character != ' ');

    attron(COLOR_PAIR(APPLE_COLOUR));
    mvaddch(*y, *x, 'A');
    attroff(COLOR_PAIR(APPLE_COLOUR));

    draw_grid(x_offset, y_offset, *score += 10);
}

void die(int condition, int score) {
    if (condition == WIN)
        printf("You won!\nYour total score is: %d\n", score);
    
    if (condition == LOSS)
        printf("You lost!\nYour total score is: %d\n", score);
}