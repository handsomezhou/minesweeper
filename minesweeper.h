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

#define MSR_SUCCESS			0
#define MSR_FAILED			-1

#define CONTENT_FLAG		'F'
#define CONTENT_MINE		'@'
#define CONTENT_UNKNOW		'?'
#define CONTENT_NONE		' '
#define	CONTENT_ZERO		' '
#define CONTENT_ONE			'1'
#define CONTENT_TWO			'2'
#define CONTENT_THREE		'3'
#define CONTENT_FOUR		'4'
#define CONTENT_FIVE		'5'
#define CONTENT_SIX			'6'
#define CONTENT_SEVEN		'7'
#define CONTENT_EIGHT		'8'

#define BATTLEFIELD_BEGIN_Y	0
#define BATTLEFIELD_BEGIN_X	0
#define BATTLEFIELD_NLINES  MIN_ROW
#define BATTLEFIELD_NCLOS	MIN_COL

#define COORDINATES_Y		1
#define COORDINATES_X		1

#define TIMER_START			0U

#define KEY_NONE			-1
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
	
	COLOR_FLAG,
	COLOR_UNKNOW,
	COLOR_SAFE_MINE,
	COLOR_UNSAFE_MINE,

	COLOR_NORMAL_MSG,
	COLOR_PROMPT_MSG,
	
	COLOR_INPUT_INFO,
};			

typedef struct value{
	BOOL is_mine;	//According to the judge whether it's mine
	BOOL is_swept;	//According to the judge whether it's swept
	char flag;		//The flag can be 'F' or '?' only when is_swept is equal to FALSE.
	char content;	//mine('@') or number(' ','1'~'8')
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

typedef struct coordinates{
	int y;
	int x;
}coordinates_t;	//coordinates of the current focus

typedef enum status{
	STATUS_PAUSE,
	STATUS_CONTINUING,
	STATUS_OVER,
}status_t;		//Status of the game 

typedef struct minesweeper{
	cross_grid_list_t *cross_grid_list;	
	battlefield_t *battlefield;	
	coordinates_t coordinates;	
	status_t status;
	unsigned int timer;			//For timing (sec)
	int cur_mine_num;			//Number of current mine
}minesweeper_t;	

int init_minesweeper(minesweeper_t **minesweeper);
int key_event(minesweeper_t *minesweeper);
coordinates_t mouse_event(minesweeper_t *minesweeper);
void exit_minesweeper(minesweeper_t *minesweeper);
#endif


