#include <stdlib.h>
#include <unistd.h>

#include <time.h>
#include <string.h>

#include <ncurses.h>

#define ms(x) 1000 * (x)

#define REQUIRED_LINES 24
#define REQUIRED_COLS 80

#define SNAKEWIN_LINES 20
#define SNAKEWIN_COLS 60

WINDOW* snake_box;
int score = 0;

int head_x = SNAKEWIN_COLS / 2;
int head_y = SNAKEWIN_LINES / 2;

int food_x = -1;
int food_y = -1;

WINDOW* window_create(int, int, int, int);
void window_destroy(WINDOW*);

void wmovecenter(WINDOW*, int);
void wprintcenter(WINDOW*, int, const char*, attr_t);

void game_init();
void game_destroy();

void generate_food();
char head_hit_food();
void score_update();
void head_update(char);
int game_refresh();

int main(void)
{
    srand(time(NULL));

    initscr();
    raw();
    noecho();

    if (LINES < REQUIRED_LINES || COLS < REQUIRED_COLS) {
        printw("Please resize your window bigger...\n");
        printw("You currently have %d LINES and %d COLS.\n", LINES, COLS);
        printw("It is required to have %d LINES and %d "
               "COLS to run this program.\n", REQUIRED_LINES, REQUIRED_COLS);

        refresh();
        getch();
        endwin();
        return 1;
    }
    curs_set(0);
    clear();
    refresh();

    game_init();

    // this three lines of code doesn't work idk why
   /*  wprintcenter(stdscr, SNAKEWIN_LINES + 2, "Press any key to start", A_BLINK);
    getch();
    wprintcenter(stdscr, SNAKEWIN_LINES + 2, " FCDUK                ", A_NORMAL); */
    
    nodelay(snake_box, TRUE);

    while (game_refresh() > 0);

    
    endwin();

    return 0;
}

WINDOW* window_create(int lines, int cols, int line, int col)
{
    WINDOW* window = newwin(lines, cols, line, col);
    box(window, 0, 0);

    return window;
}

void window_destroy(WINDOW* what)
{
    wborder(what, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(what);
    delwin(what);
}

void wmovecenter(WINDOW* win, int line)
{
    int win_maxx = getmaxx(win);
    int scr_maxx = getmaxx(stdscr);
    int starting_column = (scr_maxx / 2) - (win_maxx / 2);

    mvwin(win, line, starting_column);
}

void wprintcenter(WINDOW* win, int line, const char* str, attr_t attr)
{
    int str_length = strlen(str);
    int win_maxx = getmaxx(win);
    int starting_column = (win_maxx / 2) - (str_length / 2);

    attron(attr);
    mvwprintw(win, line, starting_column, str);
    attroff(attr);
}


void game_init()
{
    snake_box = window_create(SNAKEWIN_LINES, SNAKEWIN_COLS, 1, 1);
    wmovecenter(snake_box, 1);
    wrefresh(snake_box);
    score_update();
    generate_food();

    head_update('w');
}

void game_destroy()
{
    wclear(snake_box);
    wrefresh(snake_box);
    window_destroy(snake_box);
    
}

void generate_food()
{
    if (food_y != -1 && food_x != -1);
       // mvwaddch(snake_box, food_y, food_x, ' ');

    // food bounds (where food shall be):
    // x: [1, SNAKEWIN_COLS - 2]
    // y: [1, SNAKEWIN_LINES - 2]

    food_y = rand() % SNAKEWIN_LINES;
    // possible output : [0, SNAKEWIN_LINES - 1]
    food_x = rand() % SNAKEWIN_COLS;
    // possible output: [0, SNAKEWIN_COLS - 1]

    if (food_y == 0) food_y += 1;
    else if ((SNAKEWIN_LINES - food_y) <= 2) food_y -= 1;

    if (food_x == 0) food_x += 1;
    else if ((SNAKEWIN_COLS - food_x) <= 2) food_x -= 1;

    mvwaddch(snake_box, food_y, food_x, ACS_DIAMOND);
}

char head_hit_food()
{
    if (head_y == food_y && head_x == food_x) return 1;
    else return 0;
}

void score_update()
{
    char text[20];
    snprintf(text, 20, "Your score: %d", score);
    wmove(stdscr, 0, 0);
    clrtoeol();
    wprintcenter(stdscr, 0, text, A_NORMAL);
    wrefresh(stdscr);
}

void head_update(char direction)
{
    static char current_direction = 0;
    char new_direction = direction;

    if (current_direction != new_direction)
        if (new_direction == 'w' ||
            new_direction == 'a' || 
            new_direction == 's' || 
            new_direction == 'd')
            current_direction = new_direction;

    mvwaddch(snake_box, head_y, head_x, ' ');

    int head_icon;
    switch(current_direction) {
        case 'w':
            head_y--;
            head_icon = ACS_UARROW;
        break;
        case 'a':
            head_x--;
            head_icon = ACS_LARROW;
        break;
        case 's':
            head_y++;
            head_icon = ACS_DARROW;
        break;
        case 'd':
            head_x++;
            head_icon = ACS_RARROW;
        break;
    } 
    mvwaddch(snake_box, head_y, head_x, head_icon | A_BOLD);
}

int game_refresh()
{
    int c = wgetch(snake_box);
    if (c == 'q') {
        nocbreak();
        return -1;
    }
    
    if (c == 'w')        head_update(c);
    else if (c == 'a')   head_update(c);
    else if (c == 's')   head_update(c);
    else if (c == 'd')   head_update(c);
    else                 head_update(0);

    if (head_hit_food()) {
        score++;
        score_update();
        generate_food();
    }
    
    wrefresh(snake_box);
    usleep(ms(200));
    return 1;
}

/* TODO:
 * 1. Add the tail before the head
 * 2. Make the snake "bend"
*/
