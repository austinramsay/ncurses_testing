/*
 * Ncurses testing
 *
 * 'y' is the row
 * 'x' is the column
 *
 */


#include <ncurses.h>
#include <stdlib.h>
#include <sys/random.h>
#include <time.h>
#include <unistd.h>
#include "position.h"

#define ITERMAX 10000
#define FALSE 0
#define TRUE 1


// Prototypes
void draw_initial(int *y, int *x, int yi, int xi, struct position *original, long 
		map_size);
void invert_row(int row, int maxcols, char to_invert, char invert_with, struct 
		position *map, long map_size); 
int position_exists(int y, int x, struct position *map, long map_size);
void char_flip(char flip_to, struct position *map, long map_size); 
int getrandom_wrapper();


// nanosleep() time specification
struct timespec req = {0, 1000000};


/*
 * Initialize ncurses, draw the triangles, and perform inversing and character
 * flipping given argument options supplied from execution.
 */
int main(int argc, char **argv) {

	long iter;
	int yi, xi;
	int y[3], x[3];
	int maxlines, maxcols;

	// 'map' will hold the initial original character positions
	struct position *original = (struct position *)malloc(sizeof(struct 
				position) * ITERMAX);
	struct position *inverted = (struct position *)malloc(sizeof(struct
				position) * ITERMAX);

	/* Initialize curses */
	initscr();
	cbreak();
	noecho();
	clear();
	
	/* Initialize triangle */
	maxlines = LINES - 1;
	maxcols = COLS - 1;

	y[0] = 1;
	x[0] = 1;

	y[1] = maxlines;
	x[1] = maxcols / 2;

	y[2] = 0;
	x[2] = maxcols;

	mvaddch(y[0], x[0], '0');
	mvaddch(y[1], x[1], '1');
	mvaddch(y[2], x[2], '2');

	/* Initialize yi and xi with random values */
	yi = (getrandom_wrapper() % maxlines) + 1;
	xi = (getrandom_wrapper() % maxcols) + 1;

	mvaddch(yi, xi, '.');
	refresh();

	/* Ready to draw triangles, perform char flip and inversions */
	draw_initial(y, x, yi, xi, original, ITERMAX);
	char_flip(45, original, ITERMAX);

	for (int i = 0; i < maxlines; i++) {
		invert_row(i, maxcols, ' ', '0', original, ITERMAX);
	}

	/* End */
	mvaddstr(maxlines, 0, "Press any key to quit");
	refresh();
	getch();
	endwin();

	return 0;
}


/*
 * Given a starting position of 'y' and 'x', draw the first triangle set and
 * store the original positions in 'original'
 */
void draw_initial(int *y, int *x, int yi, int xi, struct position *original, 
		long map_size) {

	int index;

	/* Iterate the triangle */
	for (long i = 0; i < map_size; i++) {

		// Get the randomized seed
		index = (getrandom_wrapper() % 3);
											 
		// Determine the position of the char to write
		yi = (yi + y[index]) / 2;
		xi = (xi + x[index]) / 2;

		// Add delay 
		nanosleep(&req, &req);

		// Write the char and store in the map
		mvaddch(yi, xi, '*');
		
		original[i].x = xi;
		original[i].y = yi;
											 
		refresh();
	}
}


/*
 * Given a map with either written or non-written characters, perform a
 * character flip to the provided char in the args
 */
void char_flip(char flip_to, struct position *map, long map_size) {

	for (long i = 0; i < map_size; i++) {
		nanosleep(&req, &req);	

		mvaddch(map[i].y, map[i].x, flip_to);

		refresh();
	}
}


/* 
 * Given a map, store the inverted positions in a map 'inverted'
 * map1 = map to compare for current characters
 * map2 = map to store open positions in
 */
void get_inverted(int maxrows, int maxcols, struct position *map1, struct position
		*map2, long map1_size, long map2_size) {

	long index = 0; 

	// Iterate through the map and find blanks
	// i = row
	// j = column
	// Whenever a character does not exist at the iteration's row/column,
	// add it to the inverted map.
	for (int i = 0; i < maxrows; i++) {

		for (int j = 0; j < maxcols; j++) {
			if (position_exists(i, j, map1, map1_size) == FALSE) {
				// Add this to the inverted map
				map2[index].y = i;
				map2[index].x = j;
				index++;
				
				if (index >= map2_size) {
					return;
				}
			}
		}
	
	}
}


/*
 * Given a 'row' number and it's maximum number of columns, inverts the row
 * against a map of written chars. Blanks out current chars on the line then
 * writes new chars in the previously unfilled positions.
 */
void invert_row(int row, int maxcols, char replace_with, char invert_with, 
		struct position *map, long map_size) {

	// To invert the row we need to know what positions are already filled in
	// the row. Iterate the line and determine open slots.
	for (int i = 0; i < maxcols; i++) {
		// Add delay
		nanosleep(&req, &req);

		if (position_exists(row, i, map, map_size) == TRUE) {
			mvaddch(row, i, replace_with);
		} else {
			mvaddch(row, i, invert_with);
		}

		refresh();
	}

	return;
}


/*
 * Given an 'y' and 'x' position, determine if the given map has this value in
 * it 
 */
int position_exists(int y, int x, struct position *map, long map_size) {

	for (int i = 0; i < map_size; i++) {
		if (map[i].y == y && map[i].x == x) {
			// Found
			return TRUE;
		}
	}
	
	// Not found
	return FALSE;
}


/*
 * A wrapper function for the Linux system call 'getrandom()'
 * Returns a positive integer value
 */
int getrandom_wrapper() {

	int value; 

	getrandom(&value, sizeof(int), 0);
	
	if (value < 0) {
		value = value * -1; 
	}

	return value; 
}
