#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <iso646.h>
#include <math.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

#define map_size_rows LINES
#define map_size_cols COLS



struct stop {
    double col, row;
    int * n;
    int n_len;
    double f, g, h;
    int from;
};



struct route {
    int x; 
    int y; 
    double d;
};

int main() {

    initscr();
    noecho();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_RED); 
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_BLACK, COLOR_BLUE);



    srand(time(NULL));
    char map[map_size_rows][map_size_cols];
    int ind[map_size_rows][map_size_cols];
    for(int r = 0; r < map_size_rows; r++){
      for(int c = 0; c < map_size_cols; c++){
        if(rand()%10 > 7){
          map[r][c] = 1;
        }else{
          map[r][c] = 0;
        }
        ind[r][c] = -1;
      }
    }

    int i, j, k, l, b, found;
    int p_len = 0;
    int * path = NULL;
    int c_len = 0;
    int * closed = NULL;
    int o_len = 1;
    int * open = (int*)calloc(o_len, sizeof(int));
    double min, tempg;
    int s;
    int e;
    int current;
    int s_len = 0;
    struct stop * stops = NULL;
    int r_len = 0;
    struct route * routes = NULL;

    for (i = 1; i < map_size_rows - 1; i++) {
        for (j = 1; j < map_size_cols - 1; j++) {
            if (!map[i][j]) {
                ++s_len;
                stops = (struct stop *)realloc(stops, s_len * sizeof(struct stop));
                int t = s_len - 1;
                stops[t].col = j;
                stops[t].row = i;
                stops[t].from = -1;
                stops[t].g = DBL_MAX;
                stops[t].n_len = 0;
                stops[t].n = NULL;
                ind[i][j] = t;
            }
        }
    }

    /* index of start stop */
    s = 0;
    /* index of finish stop */
    e = s_len - 1;

    for (i = 0; i < s_len; i++) {
        stops[i].h = sqrt(pow(stops[e].row - stops[i].row, 2) + pow(stops[e].col - stops[i].col, 2));
    }

    for (i = 0; i < map_size_rows; i++) {
        for (j = 0; j < map_size_cols; j++) {
            if (ind[i][j] >= 0) {
                for (k = i - 1; k <= i + 1; k++) {
                    for (l = j - 1; l <= j + 1; l++) {
                        if ((k == i) and (l == j)) {
                            continue;
                        }
                        if (ind[k][l] >= 0) {
                            ++r_len;
                            routes = (struct route *)realloc(routes, r_len * sizeof(struct route));
                            int t = r_len - 1;
                            routes[t].x = ind[i][j];
                            routes[t].y = ind[k][l];
                            routes[t].d = sqrt(pow(stops[routes[t].y].row - stops[routes[t].x].row, 2) + pow(stops[routes[t].y].col - stops[routes[t].x].col, 2));
                            ++stops[routes[t].x].n_len;
                            stops[routes[t].x].n = (int*)realloc(stops[routes[t].x].n, stops[routes[t].x].n_len * sizeof(int));
                            stops[routes[t].x].n[stops[routes[t].x].n_len - 1] = t;
                        }
                    }
                }
            }
        }
    }

    open[0] = s;
    stops[s].g = 0;
    stops[s].f = stops[s].g + stops[s].h;
    found = 0;

    while (o_len && !found) {
        min = DBL_MAX;
        //mvaddch(stops[current].row, stops[current].col, ' ' | COLOR_PAIR(1));
        for (i = 0; i < o_len; i++) {
            if (stops[open[i]].f < min) {
                current = open[i];
                min = stops[open[i]].f;
            }
        }

        if (current == e) {
            found = 1;

            ++p_len;
            path = (int*)realloc(path, p_len * sizeof(int));
            path[p_len - 1] = current;
            while (stops[current].from >= 0) {
                current = stops[current].from;
                ++p_len;
                path = (int*)realloc(path, p_len * sizeof(int));
                path[p_len - 1] = current;
            }
        }

        for (i = 0; i < o_len; i++) {
            if (open[i] == current) {
                if (i not_eq (o_len - 1)) {
                    for (j = i; j < (o_len - 1); j++) {
                        open[j] = open[j + 1];
                    }
                }
                --o_len;
                open = (int*)realloc(open, o_len * sizeof(int));
                break;
            }
        }

        ++c_len;
        closed = (int*)realloc(closed, c_len * sizeof(int));
        closed[c_len - 1] = current;

        for (i = 0; i < stops[current].n_len; i++) {
            b = 0;

            for (j = 0; j < c_len; j++) {
                if (routes[stops[current].n[i]].y == closed[j]) {
                    b = 1;
                }
            }

            if (b) {
                continue;
            }

            tempg = stops[current].g + routes[stops[current].n[i]].d;

            //mvaddch(stops[current].row, stops[current].col, ' ' | COLOR_PAIR(1));
            b = 1;

            if (o_len > 0) {
                for (j = 0; j < o_len; j++) {
                    if (routes[stops[current].n[i]].y == open[j]) {
                        b = 0;
                    }
                }
            }

            if (b || (tempg < stops[routes[stops[current].n[i]].y].g)) {
                stops[routes[stops[current].n[i]].y].from = current;
                stops[routes[stops[current].n[i]].y].g = tempg;
                stops[routes[stops[current].n[i]].y].f = stops[routes[stops[current].n[i]].y].g + stops[routes[stops[current].n[i]].y].h;

                if (b) {
                    ++o_len;
                    open = (int*)realloc(open, o_len * sizeof(int));
                    open[o_len - 1] = routes[stops[current].n[i]].y;
                }
            }
        }
    }
    

    for (i = 0; i < map_size_rows; i++) {
        for (j = 0; j < map_size_cols; j++) {
            if (map[i][j]) {
                mvaddch(i,j,' ' | A_REVERSE);
            } 
        }
        
    }
    attron(COLOR_PAIR(2));
    mvaddch(0,0,' ');
    mvaddch(LINES-1, COLS-1, ' ');
    attroff(COLOR_PAIR(2));

    for (int i = 0; i < c_len; i++) {
      int stopIndex = closed[i];
      if (stopIndex >= 0 && stopIndex < s_len) {
          double col = stops[stopIndex].col;
          double row = stops[stopIndex].row;
          mvaddch(row, col, '.' | COLOR_PAIR(1));
          refresh();
          usleep(1000);
      }
    }

    for (i = 0; i < map_size_rows; i++) {
        for (j = 0; j < map_size_cols; j++) {
            if (!map[i][j]) {
                b = 0;
                for (k = 0; k < p_len; k++) {
                    if (ind[i][j] == path[k]) {
                        ++b;
                    }
                }
                if (b) {
                    attron(COLOR_PAIR(3));
                    mvaddch(i,j,'x' | A_BOLD | COLOR_PAIR(3));
                    attroff(COLOR_PAIR(3));
                } else {
                    mvaddch(i,j,'.');
                }
            }
        }
      
    }

    attron(COLOR_PAIR(2));
    mvaddch(0,0,' ');
    mvaddch(LINES-1, COLS-1, ' ');
    attroff(COLOR_PAIR(2));
    
    for (i = 0; i < s_len; ++i) {
        free(stops[i].n);
    }
    free(stops);
    free(routes);
    free(path);
    free(open);
    free(closed);

    getch();
    endwin();
    return 0;
}