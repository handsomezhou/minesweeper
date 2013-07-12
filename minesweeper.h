/**
  *  Copyright (C) 2013  Handsome Zhou
  */
 
#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <ncurses.h>	

#define GRADE_PRIMARY_ROW		9
#define GRADE_PRIMARY_COL		9
#define GRADE_PRIMARY_MINE		10

#define GRADE_INTERMEDIATE_ROW	16
#define GRADE_INTERMEDIATE_COL	16
#define GRADE_INTERMEDIATE_MINE	40

#define GRADE_ADVANCED_ROW		16
#define GRADE_ADVANCED_COL		30
#define GRADE_ADVANCED_MINE		99

#define MIN_ROW		9
#define	MAX_ROW		24
#define MIN_COL		9
#define MAX_COL		30
#define MIN_MINE	10
#define MAX_MINE	668		//(MAX_ROW-1)*(MAX_COL-1)+1

#define TRUE		1
#define FALSE		0
typedef int BOOL;
enum color{
	COLOR_ZERO=0,
	COLOR_ONE,
	COLOR_TWO,
	COLOR_THREE,
	COLOR_FOUR,
	COLOR_FIVE,
	COLOR_SIX,
	COLOR_SEVEN,
	COLOR_EIGHT,
};

typedef struct value{
	BOOL is_mine;
}value_t;

typedef struct grid{
	int row;		//Row number of the grid
	int col;		//Column number of the grid
	value_t value;	//the value of the grid
	
	struct grid *up;
	struct grid *right;
	struct grid *down;
	struct grid *left;	//Point to the grid element of the different direction
}grid_t;		//Contains information related to mine

typedef struct{
	int row_num;			//The number of the row of the rectangle
	int col_num;			//The number of the column of the rectangle
	int mine_num;			//Number of mines 
	
	grid_t *row_head;
	grid_t *col_head;
}cross_grid_list_t;	//save information of all grid

typedef struct battlefield{
	WINDOW *win;
	int nlines;
	int ncols;
	int begin_y;
	int begin_x;
}battlefield_t;	//use for print to screen

typedef enum status{
	STATUS_PAUSE,
	STATUS_CONTINUING,
	STATUS_OVER,
}status_t;		//Status of the game 

typedef struct minesweeper{
	cross_grid_list_t *cross_grid_list;	
	battlefield_t *battlefield;	
	status_t status;
	unsigned int timer;			//For timing (sec)
	int cur_mine_num;			//Number of current mine
}minesweeper_t;	

#endif


