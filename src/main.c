#include <ncurses.h>
#include <stdio.h>

static void draw_grid(void);
static void movement(int key, int *x, int *y);

int main(void) {
    initscr();
    raw();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    draw_grid();

    int x = 150;
    int y = 35;

    int *px = &x;
    int *py = &y;

    mvprintw(y, x, "O");
    refresh();
    
    int c;
    while ((c = getch()) != 3)
        movement(c, px, py);
    
    endwin();

    return 0;
}

void draw_grid(void) {
    for (int x = 100, y = 10; x < 201; ++x) {
        mvprintw(y, x, "_");
    }
    
    for (int x = 100, y = 60; x < 201; ++x) {
        mvprintw(y, x, "-");
    }

    for (int x = 100, y = 11; y < 60; ++y) {
        mvprintw(y, x, "|");
    }

    for (int x = 200, y = 11; y < 60; ++y) {
        mvprintw(y, x, "|");
    }

    refresh();
}

void movement(int key, int *x, int *y) {
    switch (key) {
        case KEY_UP:
            mvaddch(*y, *x, ' ');
            mvaddch(--*y, *x, 'O');
            break;
        
        case KEY_DOWN:
            mvaddch(*y, *x, ' ');
            mvaddch(++*y, *x, 'O');
            break;
        
        case KEY_LEFT:
            mvaddch(*y, *x, ' ');
            mvaddch(*y, --*x, 'O');
            break;
        
        case KEY_RIGHT:
            mvaddch(*y, *x, ' ');
            mvaddch(*y, ++*x, 'O');
            break;
    }
}