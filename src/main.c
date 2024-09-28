#include <ncurses.h>
#include <stdio.h>

static void draw_grid(int x, int y);
static void movement(int key, int *x, int *y);

int main(void) {
    initscr();
    raw();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    int x, y;

    getmaxyx(stdscr, y, x);

    int x_offset = x / 4;
    int y_offset = y / 10;

    draw_grid(x_offset, y_offset);

    int x_middle = x / 2;
    int y_middle = y / 2;

    int *px_middle = &x_middle;
    int *py_middle = &y_middle;

    mvprintw(y_middle, x_middle, "O");
    refresh();
    
    int c;
    while ((c = getch()) != 'q')
        movement(c, px_middle, py_middle);
    
    endwin();

    return 0;
}

void draw_grid(int x, int y) {
    int x_copy, y_copy;
    int limit_x = (x*4) - x;
    int limit_y = (y*10);
    
    for (x_copy = x, y_copy = y - 1; x_copy < limit_x + 1; ++x_copy) {
        mvprintw(y_copy, x_copy, "_");
    }
    
    for (x_copy = x, y_copy = y * 10; x_copy < limit_x + 1; ++x_copy) {
        mvprintw(y_copy, x_copy, "-");
    }

    for (x_copy = x, y_copy = y; y_copy < limit_y; ++y_copy) {
        mvprintw(y_copy, x_copy, "|");
    }

    for (x_copy = limit_x, y_copy = y; y_copy < limit_y; ++y_copy) {
        mvprintw(y_copy, x_copy, "|");
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