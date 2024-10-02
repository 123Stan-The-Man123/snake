#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIN 0
#define LOSS 1
#define QUIT 2
#define SNAKE_COLOUR 1
#define APPLE_COLOUR 2
struct snake_part {
    int x;
    int y;
    struct snake_part *next;
};

static void draw_grid(int x, int y, int score);
static int movement(int key, int conflict_key, int *x, int *y);
static void manage_snake(struct snake_part *snake_start, int x, int y);
static void print_snake(struct snake_part *snake_start);
static void spawn_apple(int *x, int *y, int x_offset, int y_offset, int *score);
static void die(int condition, int score);

int main(void) {
    initscr();
    raw();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    nodelay(stdscr, TRUE);

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

    struct snake_part *snake_start = malloc(sizeof(struct snake_part));
    struct snake_part *current_part = snake_start;

    for (int i = 2; i >= 0; --i) {
        current_part->x = x_pos - i;
        current_part->y = y_pos; 

        if (i != 0) {
            current_part->next = malloc(sizeof(struct snake_part));
            current_part = current_part->next;
        }
        
        else
            current_part->next = NULL;
    }

    current_part = snake_start;

    attron(COLOR_PAIR(SNAKE_COLOUR));
    while (current_part != NULL) {
        mvaddch(current_part->y, current_part->x, 'O');
        current_part = current_part->next;
    }

    attroff(COLOR_PAIR(SNAKE_COLOUR));

    srand(time(NULL));
    int random_x, random_y;
    int *prandom_x = &random_x;
    int *prandom_y = &random_y;
    spawn_apple(prandom_x, prandom_y, x_offset, y_offset, &score);

    draw_grid(x_offset, y_offset, score);
    
    int c, status;
    int prev_c = ERR;
    while ((c = getch()) != 'q') {
        status = movement(prev_c, c, px_pos, py_pos);
        manage_snake(snake_start, x_pos, y_pos);
        print_snake(snake_start);

        if (x_pos == random_x && y_pos == random_y)
            spawn_apple(prandom_x, prandom_y, x_offset, y_offset, &score);
        
        if (x_pos <= x_offset * 3 || x_pos >= x_offset * 7 || y_pos <= y_offset-1 || y_pos >= y_offset * 9) {
            break;
        }

        if (c != ERR && status == 0)
            prev_c = c;
        
        if (status == 0)
            usleep(130000);
    }

    endwin();

    if (c == 'q')
        die(QUIT, score);
    else
        die(LOSS, score);
    
    current_part = snake_start;
    struct snake_part *prev_part = current_part;

    while (current_part != NULL) {
        current_part = current_part->next;
        free(prev_part);
        prev_part = current_part;
    }

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

int movement(int key, int conflict_key, int *x, int *y) {
    mvaddch(*y, *x, ' ');
    int conflicted = 0;

    switch (key) {
        case KEY_UP:
            if (conflict_key == KEY_DOWN)
                conflicted = 1;
            
            --*y;
            break;
        
        case KEY_DOWN:
            if (conflict_key == KEY_UP)
                conflicted = 1;
            
            ++*y;
            break;
        
        case KEY_LEFT:
            if (conflict_key == KEY_RIGHT)
                conflicted = 1;
            
            --*x;
            break;
        
        case KEY_RIGHT:
            if (conflict_key == KEY_LEFT)
                conflicted = 1;
            
            ++*x;
            break;
    }

    attron(COLOR_PAIR(SNAKE_COLOUR));
    mvaddch(*y, *x, 'O');
    attroff(COLOR_PAIR(SNAKE_COLOUR));

    refresh();

    if (conflicted)
        return -1;
    
    return 0;
}

void manage_snake(struct snake_part *current_part, int x, int y) {
    mvaddch(current_part->y, current_part->x, ' ');

    while (current_part->next != NULL) {
        current_part->x = current_part->next->x;
        current_part->y = current_part->next->y;
        current_part = current_part->next;
    }

    current_part->x = x;
    current_part->y = y;
}

static void print_snake(struct snake_part *current_part) {
    while (current_part != NULL) {
        attron(COLOR_PAIR(SNAKE_COLOUR));
        mvaddch(current_part->y, current_part->x, 'O');
        attroff(COLOR_PAIR(SNAKE_COLOUR));
        current_part = current_part->next;
    }
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
    
    if (condition == QUIT)
        printf("Your total score is: %d\n", score);
}