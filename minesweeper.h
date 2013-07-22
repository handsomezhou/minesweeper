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

#define MAX_STR				128
#define MAX_PLAY_TIME		999		//sec

#define MSR_SUCCESS			0
#define MSR_FAILED			-1

#define FLAG_MINE			'F'
#define FLAG_UNKNOW			'?'
#define FLAG_NONE			'*'

#define MINE_PAINT_SAFE_NINE	'@' 
#define MINE_PAINT_UNSAFE_MINE	'X'


//Calculate the number of Mine around
#define MINE_NUM_ZERO		0
#define MINE_NUM_ONE		1
#define MINE_NUM_TWO		2
#define MINE_NUM_THREE		3
#define MINE_NUM_FOUR		4
#define MINE_NUM_FIVE		5
#define MINE_NUM_SIX		6
#define MINE_NUM_SEVEN		7
#define MINE_NUM_EIGHT		8
#define MINE_NUM_NINE		9	//it never be 9,it means itself is mines.

#define	MINE_PAINT_ZERO			' '	
#define MINE_PAINT_ONE			'1'
#define MINE_PAINT_TWO			'2'
#define MINE_PAINT_THREE		'3'
#define MINE_PAINT_FOUR			'4'
#define MINE_PAINT_FIVE			'5'
#define MINE_PAINT_SIX			'6'
#define MINE_PAINT_SEVEN		'7'
#define MINE_PAINT_EIGHT		'8'
#define MINE_PAINT_NINE			MINE_PAINT_SAFE_NINE




#define BATTLEFIELD_BEGIN_Y	0
#define BATTLEFIELD_BEGIN_X	0
#define BATTLEFIELD_NLINES  MIN_ROW
#define BATTLEFIELD_NCLOS	MIN_COL

#define TIMER_START			0U

#define KEY_NONE			-1
#define MOUSE_NON_VALID_X	-1
#define MOUSE_NON_VALID_Y	-1
//#define KEY_ENTER			10


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
	COLOR_EIGHT,	//set color for the number of markers mine
	
	COLOR_MINE,
	COLOR_UNKNOW,
	COLOR_NONE,
	
	COLOR_SAFE_MINE,
	COLOR_UNSAFE_MINE,

	COLOR_NORMAL_MSG,
	COLOR_PROMPT_MSG,
	
	COLOR_INPUT_INFO,

	COLOR_BACKGROUND,
};			

typedef struct value{
	BOOL is_mine;	//According to the judge whether it's mine
	BOOL is_swept;	//According to the judge whether it's swept
	char flag;		//The flag can be 'F','f' or '?' only when is_swept is equal to FALSE.
	char paint;		//mine('@') or number(' ','1'~'8')
	int mine_num;	//mine number 
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

typedef struct cross_grid{
	int row_num;			//The number of the row of the rectangle
	int col_num;			//The number of the column of the rectangle
	int mine_num;			//Number of mines 
#if 0
	grid_t *row_head;
	grid_t *col_head;
#endif						//It's no necessary to use two head node to point the cross list in this program.
	grid_t *grid_head;	    //grid_head->down<=>row_head, grid_head->right<=>col_head
}cross_grid_list_t;	//save information of all grid

typedef struct battlefield{
	WINDOW *win;
	int nlines;
	int ncols;
	int begin_y;
	int begin_x;
}battlefield_t;	//use for print to screen

typedef enum status{
	STATUS_INIT,
	STATUS_START,
	STATUS_END_FAILED,
	STATUS_END_SUCCESS,
}status_t;		//Status of the game 

typedef struct minesweeper{
	cross_grid_list_t *cross_grid_list;	
	battlefield_t *battlefield;	
	MEVENT mevent;	
	status_t status;
	unsigned int timer;			//For timing (sec)
	int cur_mine_num;			//Number of current mine
}minesweeper_t;	

int init_minesweeper(minesweeper_t **minesweeper);
void paint_minesweeper(minesweeper_t *minesweeper);
int handle_minesweeper(minesweeper_t *minesweeper);
MEVENT mouse_event(battlefield_t *battlefield);
void exit_minesweeper(minesweeper_t *minesweeper);
#endif


