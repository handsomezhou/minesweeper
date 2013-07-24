#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "minesweeper.h"

//Initialize and release resources
static int init_cross_grid_list(cross_grid_list_t **cross_grid_list);
static void free_cross_grid_list(cross_grid_list_t  *cross_grid_list);

static int init_screen(cross_grid_list_t *cross_grid_list);
static void end_screen(void);
static void draw_init_msg();
static int get_cross_grid_set(cross_grid_list_t *cross_grid_list);
static BOOL is_digit_str(const char *str);

static int init_grid_head(grid_t **grid);
static int init_grid(grid_t *grid_head, int row_num, int col_num);
static int *set_mine_pos(int row_num, int col_num, int mine_num, int *pos);
static BOOL is_in_pos(int *pos, int pos_len, int num);
static int *sort_mine_pos(int *pos, int length);
static void quick_sort(int *num, int left, int right);
static int init_mine_pos(grid_t *grid_head, int mine_num, int *pos);
static int get_surround_mine_num(grid_t *grid);
static void free_grid(grid_t *grid_head);
static int init_battlefield(battlefield_t **battlefield, int row_num, int col_num);
static void free_battlefield(battlefield_t *battlefield);

//Paint
static void clear_minesweeper();
static void draw_minesweeper(const minesweeper_t *minesweeper_t);
#if 0
static void draw_mines(const grid_t *grid_head);	//just for test
#endif

//Handle
static grid_t *get_grid(const grid_t *grid_head, int row, int col);
static int deal_btn1_event(grid_t *grid, minesweeper_t *minesweeper);
static int deal_btn2_event(grid_t *grid, minesweeper_t *minesweeper);
static grid_t *traverse_grid(grid_t *grid,const grid_t *grid_head);
static BOOL is_success_minesweeper(const grid_t *grid_head); 

static int init_cross_grid_list(cross_grid_list_t **cross_grid_list)
{
	cross_grid_list_t **cgl=cross_grid_list;
	int ret;
	if(NULL!=*cgl){		//not the first init	
		free_cross_grid_list(*cgl);
		*cgl=NULL;
	}

	*cgl=(cross_grid_list_t *)malloc(sizeof(cross_grid_list_t));
	do{
		if(NULL==*cgl){
			break;
		}
	
		(*cgl)->grid_head=NULL;
		(*cgl)->row_num=MIN_ROW;
		(*cgl)->col_num=MIN_COL;
		(*cgl)->mine_num=MIN_MINE;
		ret=init_grid_head(&(*cgl)->grid_head);
		if(MSR_FAILED==ret){
			break;
		}

		return MSR_SUCCESS;
	}while(0);

	return MSR_FAILED;
}

static void free_cross_grid_list(cross_grid_list_t  *cross_grid_list)
{
	cross_grid_list_t *cgl=cross_grid_list;
	if(NULL==cgl){
		return;
	}

	free_grid(cgl->grid_head);
	cgl->grid_head=NULL;
	
	free(cgl);

	return;
}

static int init_screen(cross_grid_list_t *cross_grid_list)
{
	cross_grid_list_t *cgl=cross_grid_list;
	if(NULL==cgl){
		return MSR_FAILED;
	}
	initscr();
	cbreak();
	curs_set(TRUE);
	clear();

	if(has_colors()){
		start_color();

		init_pair(COLOR_ZERO,COLOR_BLUE,COLOR_BLACK);
		init_pair(COLOR_ONE,COLOR_BLUE,COLOR_BLACK);
		init_pair(COLOR_TWO,COLOR_GREEN,COLOR_BLACK);
		init_pair(COLOR_THREE,COLOR_YELLOW,COLOR_BLACK);
		init_pair(COLOR_FOUR,COLOR_MAGENTA,COLOR_BLACK);
		init_pair(COLOR_FIVE,COLOR_RED,COLOR_BLACK);
		init_pair(COLOR_SIX,COLOR_WHITE,COLOR_BLACK);
		init_pair(COLOR_SEVEN,COLOR_RED,COLOR_BLACK);
		init_pair(COLOR_EIGHT,COLOR_RED,COLOR_BLACK);	
		
		init_pair(COLOR_MINE,COLOR_RED,COLOR_BLACK);
		init_pair(COLOR_UNKNOW,COLOR_WHITE,COLOR_BLACK);
		init_pair(COLOR_NONE,COLOR_BLUE,COLOR_BLACK);
		
		init_pair(COLOR_SAFE_MINE,COLOR_CYAN,COLOR_BLACK);
		init_pair(COLOR_UNSAFE_MINE,COLOR_RED,COLOR_BLACK);

		init_pair(COLOR_NORMAL_MSG,COLOR_YELLOW,COLOR_BLACK);
		init_pair(COLOR_PROMPT_MSG,COLOR_RED,COLOR_BLACK);

		init_pair(COLOR_INPUT_INFO,COLOR_GREEN,COLOR_BLACK);

		init_pair(COLOR_BACKGROUND,COLOR_BLUE,COLOR_BLACK);
	}

	nodelay(stdscr,FALSE);
	keypad(stdscr,TRUE);
	draw_init_msg();
	get_cross_grid_set(cgl);
	init_grid(cgl->grid_head,cgl->row_num,cgl->col_num);


	int *pos=(int *)malloc(sizeof(int)*(cgl->mine_num));
	int i=0;
	for(i=0; i<cgl->mine_num; i++){
		*(pos+i)=-1;
	}
	set_mine_pos(cgl->row_num,cgl->col_num,cgl->mine_num,pos);
	
	sort_mine_pos(pos,cgl->mine_num);
	
	init_mine_pos(cgl->grid_head,cgl->mine_num,pos);
	free(pos);
	pos=NULL;

	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;

	for(cur_row=cgl->grid_head->down; cur_row!=NULL; cur_row=cur_row->down){
		for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
			get_surround_mine_num(cur_col);
		}
	}
	
	return MSR_SUCCESS;
	
}

static void end_screen(void)
{
	endwin();
}

static void draw_init_msg()
{
	char game_help[] ="Minesweeper Game Help!";	
	char set_help[]  ="You can set the length and width of the grid,\n and set the number of mine!";
	char mouse_btn1[]="mouse button 1:Click to reveal where no mine";
	char mouse_btn2[]="mouse button 2:Click to mark a grid by using a graph 'F' or '?',\n\t\t'F' indicates mine,'?' indicates unknow!";
	
	char start[]="Press any key to continue!";
	int max_x;
	int max_y;
	int y=0;
	int x=1;
	
	getmaxyx(stdscr,max_y,max_x);
	
	if(has_colors()){attron(COLOR_PAIR(COLOR_NORMAL_MSG)|A_BOLD);}
	mvwprintw(stdscr,++y,(max_x-strlen(game_help))/2,"%s",game_help);
	y++;
	mvwprintw(stdscr,++y,x,"%s",set_help);
	y++;
	mvwprintw(stdscr,++y,x,"%s",mouse_btn1);
	mvwprintw(stdscr,++y,x,"%s",mouse_btn2);
	y+=2;
	if(has_colors()){attroff(COLOR_PAIR(COLOR_NORMAL_MSG)|A_BOLD);}

	if(has_colors()){attron(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}
	
	mvwprintw(stdscr,((max_y/2)>y)?(max_y/2):(y),(max_x-strlen(start))/2,"%s",start);
	box(stdscr,0,0);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}

	wrefresh(stdscr);
	wgetch(stdscr);	
	
}

static int get_cross_grid_set(cross_grid_list_t *cross_grid_list)
{
	cross_grid_list_t *cgl=cross_grid_list;
	if(NULL==cgl){
		return MSR_FAILED;
	}
	char prompt[]="Please set the minesweeper game  rows,columns and mine";
	char range[MAX_STR/2];
	char row[]  ="the number of row  :";
	char col[]  ="the number of col  :";
	char mines[]="the number of mines:";
	char help[]="Please press any key to start!";
	char result[MAX_STR/2];
	char str_row[MAX_STR];
	char str_col[MAX_STR];
	char str_mines[MAX_STR];
	int max_y;
	int max_x;
	int y=0;
	int x=1;
	int ty=0;
	int tx=0;
	int num;
	snprintf(range,MAX_STR/2,"row[%d,%d],col[%d,%d],mines[%d,%d]",MIN_ROW,MAX_ROW,MIN_COL,MAX_COL,MIN_MINE,MAX_MINE);
	getmaxyx(stdscr,max_y,max_x);
	
	werase(stdscr);
	if(has_colors()){attron(COLOR_PAIR(COLOR_NORMAL_MSG)|A_BOLD);}
	mvwprintw(stdscr,++y,(max_x-strlen(prompt))/2,"%s",prompt);
	mvwprintw(stdscr,++y,(max_x-strlen(range))/2,range);
	y++;
	mvwprintw(stdscr,++y,x,"%s",row);
	mvwprintw(stdscr,++y,x,"%s",col);
	mvwprintw(stdscr,++y,x,"%s",mines);
	box(stdscr,0,0);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_NORMAL_MSG)|A_BOLD);}
	
	ty=y-2;
	tx=x+strlen(row);
	if(has_colors()){attron(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}

	memset(str_row,0,sizeof(str_row));
	mvwgetnstr(stdscr,ty,tx,str_row,MAX_STR);
	if(is_digit_str(str_row)==TRUE){
		num=atoi(str_row);
		num=(num>=MIN_ROW&&num<=MAX_ROW)?(num):(MIN_ROW);
	}else{
		num=MIN_ROW;
	}
	cgl->row_num=num;

	memset(str_col,0,sizeof(str_col));
	mvwgetnstr(stdscr,ty+1,tx,str_col,MAX_STR);
	if(is_digit_str(str_col)==TRUE){
		num=atoi(str_col);
		num=(num>=MIN_COL&&num<=MAX_COL)?(num):(MIN_COL);
	}else{
		num=MIN_COL;
	}
	cgl->col_num=num;

	memset(str_mines,0,sizeof(str_mines));
	mvwgetnstr(stdscr,ty+2,tx,str_mines,MAX_STR);
	if(is_digit_str(str_mines)==TRUE){
		num=atoi(str_mines);
		num=(num>=MIN_MINE&&num<=(cgl->row_num-1)*(cgl->col_num-1)+1)?(num):\
			(rand()%(((cgl->row_num-1)*(cgl->col_num-1)+1)-MIN_MINE+1)+MIN_MINE);
	}else{
		num=rand()%(((cgl->row_num-1)*(cgl->col_num-1)+1)-MIN_MINE+1)+MIN_MINE;

	}
	cgl->mine_num=num;

	snprintf(result,MAX_STR/2,"Your settings:row=[%d],col=[%d],mines=[%d]",\
		cgl->row_num,cgl->col_num,cgl->mine_num);
	mvwprintw(stdscr,max_y/2,(max_x-strlen(result))/2,"%s",result);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}

	if(has_colors()){attron(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	mvwprintw(stdscr,max_y/2+1,(max_x-strlen(help))/2,"%s",help);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	
	wgetch(stdscr);
	
	return MSR_SUCCESS;
}

static BOOL is_digit_str(const char *str)
{
	const char *p=str;
	if(NULL==p){
		return FALSE;
	}
	while(*p!='\0'){
		if(*p>'9'||*p<'0'){
			return FALSE;
		}
		p++;
	}

	return TRUE;
}

static int init_grid_head(grid_t **grid)
{
	grid_t **gd=grid;
	if(NULL!=*gd){							
		free_grid(*gd);
		*gd=NULL;
	}

	*gd=(grid_t *)malloc(sizeof(grid_t));
	if(NULL==*gd){
		return MSR_FAILED;
	}

	(*gd)->row=0;
	(*gd)->col=0;
	(*gd)->value.is_mine=FALSE;
	(*gd)->value.is_swept=FALSE;
	(*gd)->value.paint=MINE_PAINT_ZERO;
	(*gd)->value.mine_num=MINE_NUM_ZERO;
	(*gd)->up=NULL;
	(*gd)->right=NULL;
	(*gd)->down=NULL;
	(*gd)->left=NULL;
	
	return MSR_SUCCESS;
}

static int init_grid(grid_t *grid_head, int row_num, int col_num)
{
	if(NULL==grid_head){
		return MSR_FAILED;
	}
	
	grid_t *cur_row=grid_head;	//row_head
	grid_t *cur_col=grid_head;	//col_head
	int row=(row_num>=MIN_ROW && row_num<=MAX_ROW)?(row_num):(MIN_ROW);
	int col=(col_num>=MIN_COL && col_num<=MAX_COL)?(col_num):(MIN_COL);

	//use the head node to store the cross grid of rows and columns
	grid_head->row=row;
	grid_head->col=col;
	
	int x=0;
	int y=0;
	
	
	for(y=0; y<row; y++){
		cur_row->down=(grid_t *)malloc(sizeof(grid_t));
		if(NULL==cur_row->down){
			return MSR_FAILED;
		}
		
		cur_row->down->up=cur_row;
		cur_row->down->down=NULL;
		if(y==0){
			cur_row->down->left=cur_col;
			cur_col->right=cur_row->down;
		}else{
			cur_row->down->left=NULL;
		}
		cur_row->down->right=NULL;
		
		cur_row=cur_row->down;

		cur_col=cur_row;
		for(x=0; x<col; x++){
			if(0!=x){
				cur_col->right=(grid_t *)malloc(sizeof(grid_t));
				cur_col->right->left=cur_col;
				cur_col->right->right=NULL;
				if(y==0){
					cur_col->right->up=NULL;
				}
				cur_col->right->row=y;
				cur_col->right->col=x;
				cur_col->right->value.is_mine=FALSE;
				cur_col->right->value.is_swept=FALSE;
				cur_col->right->value.flag=FLAG_NONE;
				cur_col->right->value.paint=MINE_PAINT_ZERO;
				cur_col->right->value.mine_num=MINE_NUM_ZERO;
				cur_col=cur_col->right;
			}else{
				cur_col->row=y;
				cur_col->col=x;
				cur_col->value.is_mine=FALSE;
				cur_col->value.is_swept=FALSE;
				cur_col->value.flag=FLAG_NONE;
				cur_col->value.paint=MINE_PAINT_ZERO;
				cur_col->value.mine_num=MINE_NUM_ZERO;
			}
				
		}	
	}

	grid_t *prev_row=grid_head->down;
	grid_t *prev_col=NULL;
	
	for(cur_row=prev_row->down;cur_row!=NULL;cur_row=cur_row->down){
		for(prev_col=prev_row->right,cur_col=cur_row->right;\
			cur_col!=NULL; prev_col=prev_col->right,cur_col=cur_col->right){
			prev_col->down=cur_col;
			cur_col->up=prev_col;
			cur_col->down=NULL;
		}
		prev_row=cur_row;
	}
	
	return MSR_SUCCESS;
}

static int *set_mine_pos(int row_num, int col_num, int mine_num, int *pos)
{
	int row=row_num;
	int col=col_num;
	int mine=mine_num;
	int *p=pos;
	int total=row*col;
	int num;
	int i=0;
	BOOL bl;
	do{
		num=rand()%total;
		bl=is_in_pos(p,mine_num,num);
		if(FALSE==bl){
			*(p+i)=num;
			i++;
		}
		
	}while((mine-i)!=0);

	return p;
	
}

static BOOL is_in_pos(int *pos, int pos_len, int num)
{
	int *p=pos;
	int len=pos_len;
	int n=num;
	int i=0;
	for(i=0; i<len; i++){
		if(*(p+i)==-1){//pos init with -1
			break;			
		}else if(*(p+i)==n){
			return TRUE;
		}
	}

	return FALSE;
}

static int *sort_mine_pos(int *pos, int length)
{
	int *p=pos;
	int len=length;
	if(NULL==p||len<=1){
		return p;
	}
	quick_sort(p,0,len-1);
	
	return p;
}

static void quick_sort(int *a, int left, int right)
{
	int *p=a;
	if(NULL==p){
		return ;
	}
	
	int i=left;
	int j=right;
	int m=(i+j)/2;
	int key=*(p+m);

	while(i<j){
		while((key>*(p+i))&&(m>i)){
			i++;
		}
		if(i<m){
			*(p+m)=*(p+i);
			m=i;
		}

		while(j>0&&(j>m&&*(p+j)>key)){
			j--;
		}
		if(m<j){
			*(p+m)=*(p+j);
			m=j;
		}
	}

	*(p+m)=key;
	if(m-left>1){
		quick_sort(p,left,m-1);
	}

	if(right-m>1){
		quick_sort(p,m+1,right);
	}
}

static int init_mine_pos(grid_t *grid_head, int mine_num, int *pos)
{
	grid_t *gh=grid_head;
	int mine=mine_num;
	int *p=pos;
	int i=0;
	int cnt=0;
	
	if(NULL==gh||NULL==p){
		return MSR_FAILED;
	}
	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;

	for(cur_row=gh->down; cur_row!=NULL; cur_row=cur_row->down){
		for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
			if(*(p+i)==cnt){
				cur_col->value.is_mine=TRUE;
				i++;
				if(i>=mine){
					return MSR_SUCCESS;
				}
			}
			cnt++;
		}
	}
	return MSR_SUCCESS;
}

static int get_surround_mine_num(grid_t *grid)
{
	grid_t *gd=grid;
	if(NULL==gd){
		return MSR_FAILED;
	}
	int n=MINE_NUM_ZERO;
	
	if(gd->value.is_mine==TRUE){
		n=MINE_NUM_NINE;

	}else{
		if(gd->up!=NULL){
			if(gd->up->value.is_mine==TRUE){n++;}
			if(gd->up->left!=NULL){
				if(gd->up->left->value.is_mine==TRUE){n++;}
			}
			if(gd->up->right!=NULL){
				if(gd->up->right->value.is_mine==TRUE){n++;}
			}
		}

		if(gd->left!=NULL){
			if(gd->left->value.is_mine==TRUE){n++;}
		}

		if(gd->right!=NULL){
			if(gd->right->value.is_mine==TRUE){n++;}
		}

		if(gd->down!=NULL){
			if(gd->down->value.is_mine==TRUE){n++;}
			if(gd->down->left!=NULL){
				if(gd->down->left->value.is_mine==TRUE){n++;}
			}
			if(gd->down->right!=NULL){
				if(gd->down->right->value.is_mine==TRUE){n++;}
			}
			
		}
		
	}

	gd->value.mine_num=n;
	switch(gd->value.mine_num){
		case MINE_NUM_ZERO:gd->value.paint=MINE_PAINT_ZERO;
			break;
		case MINE_NUM_ONE:gd->value.paint=MINE_PAINT_ONE;
			break;
		case MINE_NUM_TWO:gd->value.paint=MINE_PAINT_TWO;
			break;
		case MINE_NUM_THREE:gd->value.paint=MINE_PAINT_THREE;
			break;
		case MINE_NUM_FOUR:gd->value.paint=MINE_PAINT_FOUR;
			break;
		case MINE_NUM_FIVE:gd->value.paint=MINE_PAINT_FIVE;
			break;
		case MINE_NUM_SIX:gd->value.paint=MINE_PAINT_SIX;
			break;
		case MINE_NUM_SEVEN:gd->value.paint=MINE_PAINT_SEVEN;
			break;
		case MINE_NUM_EIGHT:gd->value.paint=MINE_PAINT_EIGHT;
			break;
		case MINE_NUM_NINE:gd->value.paint=MINE_PAINT_NINE;
			break;
	}

	return gd->value.mine_num;
}

static void free_grid(grid_t *grid_head)
{
	grid_t *gh=grid_head;
	if(NULL==gh){
		return;
	}

	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;
	grid_t *next_col=NULL;
	
	for(cur_row=gh->down; cur_row!=NULL; cur_row=cur_row->down){
		for(cur_col=cur_row; cur_col!=NULL; cur_col=next_col){
			next_col=cur_col->right;
			free(cur_col);
		}
	}
	gh->down=NULL;
	gh->right=NULL;
	free(gh);
}

static int init_battlefield(battlefield_t **battlefield, int row_num, int col_num)
{
	battlefield_t **bf=battlefield;
	int row=row_num;
	int col=col_num;
	if(NULL!=*bf){
		free_battlefield(*bf);
		*bf=NULL;
	}

	*bf=(battlefield_t *)malloc(sizeof(battlefield_t));
	if(NULL==*bf){
		return MSR_FAILED;
	}
	(*bf)->win=stdscr;
	(*bf)->nlines=row+4;
	(*bf)->ncols=col+2;;
	(*bf)->begin_y=BATTLEFIELD_BEGIN_Y;
	(*bf)->begin_x=BATTLEFIELD_BEGIN_X;
	
	return MSR_SUCCESS;
}

static void free_battlefield(battlefield_t *battlefield)
{
	battlefield_t *bf=battlefield;
	if(NULL==bf){
		return;
	}

	free(bf);
}

int init_minesweeper(minesweeper_t **minesweeper)
{
	minesweeper_t **msr=minesweeper;
	int ret;
	if(NULL!=*msr){
		exit_minesweeper(*msr);
		*msr=NULL;
	}

	*msr=(minesweeper_t *)malloc(sizeof(minesweeper_t));

	do{
		if(NULL==*msr){
			break;
		}
		(*msr)->cross_grid_list=NULL;
		(*msr)->battlefield=NULL;
		(*msr)->mevent.y=MOUSE_NON_VALID_Y;
		(*msr)->mevent.x=MOUSE_NON_VALID_X;
		(*msr)->cur_mine_num=MIN_MINE;
		(*msr)->timer=TIMER_START;
		(*msr)->status=STATUS_INIT;
		
		ret=init_cross_grid_list(&(*msr)->cross_grid_list);
		if(ret==MSR_FAILED){
			break;
		}

		ret=init_screen((*msr)->cross_grid_list);
		if(MSR_FAILED==ret){
			break;
		}
		(*msr)->cur_mine_num=(*msr)->cross_grid_list->mine_num;
		
		werase(stdscr);
		int row=(*msr)->cross_grid_list->row_num;
		int col=(*msr)->cross_grid_list->col_num;
		ret=init_battlefield(&(*msr)->battlefield,row,col);
		nodelay(stdscr,TRUE);
		if(MSR_FAILED==ret){
			break;
		}

		return MSR_SUCCESS;
	}while(0);

	return MSR_FAILED;
	
}

static void clear_minesweeper()
{
	werase(stdscr);
}

static void draw_minesweeper(const minesweeper_t *minesweeper)
{
	const minesweeper_t *msr=minesweeper;
	if(NULL==msr||NULL==msr->battlefield||NULL==msr->cross_grid_list){
		return;
	}
	cross_grid_list_t *cgl=msr->cross_grid_list;
	battlefield_t *bf=msr->battlefield;
	int cur_x;
	int cur_y;
	getmaxyx(stdscr,cur_y,cur_x);
	bf->begin_y=(cur_y-bf->nlines)/2;
	bf->begin_x=(cur_x-bf->ncols)/2;
	int y=0;
	int x=0;
	
	if(has_colors()){attron(COLOR_PAIR(COLOR_BACKGROUND)|A_BOLD);}
	for(y=bf->begin_y; y<bf->nlines+bf->begin_y; y++){
		for(x=bf->begin_x; x<bf->ncols+bf->begin_x; x++){
			if(x==bf->begin_x||(x==bf->ncols-1+bf->begin_x)||\
				y==bf->begin_y||(y==bf->nlines-1+bf->begin_y)||(y==bf->nlines-3+bf->begin_y)){
				mvwaddch(bf->win,y,x,'#');
			}
		}
	}
	if(has_colors()){attroff(COLOR_PAIR(COLOR_BACKGROUND)|A_BOLD);}
	
	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;
	x=bf->begin_x+1;
	y=bf->begin_y+1;
	for(cur_row=cgl->grid_head->down;cur_row!=NULL; cur_row=cur_row->down,y++,x=bf->begin_x+1){
		for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right,x++){
			if(cur_col->value.is_swept==TRUE){
				if(cur_col->value.is_mine==TRUE){
					switch(cur_col->value.paint){
						case MINE_PAINT_SAFE_NINE:
							if(has_colors()){attron(COLOR_PAIR(COLOR_SAFE_MINE)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_SAFE_MINE)|A_BOLD);}
							break;
						case MINE_PAINT_UNSAFE_MINE:
							if(has_colors()){attron(COLOR_PAIR(COLOR_UNSAFE_MINE)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_UNSAFE_MINE)|A_BOLD);}
							break;
					}
				}else{
					switch(cur_col->value.paint){
						case MINE_PAINT_ZERO:
							if(has_colors()){attron(COLOR_PAIR(COLOR_ZERO)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_ZERO)|A_BOLD);}
							break;
						case MINE_PAINT_ONE:
							if(has_colors()){attron(COLOR_PAIR(COLOR_ONE)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_ONE)|A_BOLD);}
							break;
						case MINE_PAINT_TWO:
							if(has_colors()){attron(COLOR_PAIR(COLOR_TWO)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_TWO)|A_BOLD);}
							break;
						case MINE_PAINT_THREE:
							if(has_colors()){attron(COLOR_PAIR(COLOR_THREE)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_THREE)|A_BOLD);}
							break;
						case MINE_PAINT_FOUR:
							if(has_colors()){attron(COLOR_PAIR(COLOR_FOUR)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_FOUR)|A_BOLD);}
							break;
						case MINE_PAINT_FIVE:
							if(has_colors()){attron(COLOR_PAIR(COLOR_FIVE)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_FIVE)|A_BOLD);}
							break;
						case MINE_PAINT_SIX:
							if(has_colors()){attron(COLOR_PAIR(COLOR_SIX)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_SIX)|A_BOLD);}
							break;
						case MINE_PAINT_SEVEN:
							if(has_colors()){attron(COLOR_PAIR(COLOR_SEVEN)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_SEVEN)|A_BOLD);}
							break;
						case MINE_PAINT_EIGHT:
							if(has_colors()){attron(COLOR_PAIR(COLOR_EIGHT)|A_BOLD);}
								mvwaddch(bf->win,y,x,cur_col->value.paint);
							if(has_colors()){attroff(COLOR_PAIR(COLOR_EIGHT)|A_BOLD);}
							break;
					}
				}
			}else{
				switch(cur_col->value.flag){
					case FLAG_MINE://'F'
						if(has_colors()){attron(COLOR_PAIR(COLOR_MINE)|A_BOLD);}
							mvwaddch(bf->win,y,x,cur_col->value.flag);
						if(has_colors()){attroff(COLOR_PAIR(COLOR_MINE)|A_BOLD);}
						break;
					case FLAG_UNKNOW:
						if(has_colors()){attron(COLOR_PAIR(COLOR_UNKNOW)|A_BOLD);}
							mvwaddch(bf->win,y,x,cur_col->value.flag);
						if(has_colors()){attroff(COLOR_PAIR(COLOR_UNKNOW)|A_BOLD);}
						break;
					case FLAG_NONE:
						if(has_colors()){attron(COLOR_PAIR(COLOR_NONE)|A_BOLD);}
							mvwaddch(bf->win,y,x,cur_col->value.flag);
						if(has_colors()){attroff(COLOR_PAIR(COLOR_NONE)|A_BOLD);}
						break;
				}
			}
		}
	}
	if(has_colors()){attron(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	mvwprintw(bf->win,bf->begin_y+bf->nlines-2,bf->begin_x+1,"T%3d",msr->timer);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	
	if(has_colors()){attron(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}
	mvwprintw(bf->win,bf->begin_y+bf->nlines-2,bf->begin_x+bf->ncols/2+1,"@%3d",msr->cur_mine_num);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}
	
	if(has_colors()){attron(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}
	box(stdscr,0,0);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}
	
	wmove(stdscr,msr->mevent.y,msr->mevent.x);
	wrefresh(bf->win);
	
}

#if 0
static void draw_mines(const grid_t *grid_head)
{
	const grid_t *gh=grid_head;
	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;
	int y=1;
	int x=1;
	if(has_colors()){attron(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}
	for(cur_row=gh->down;cur_row!=NULL; cur_row=cur_row->down,y++,x=1){
		for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right,x++){
			mvwprintw(stdscr,y,x,"%c",cur_col->value.paint);
		}
	}
	if(has_colors()){attroff(COLOR_PAIR(COLOR_INPUT_INFO)|A_BOLD);}
	wrefresh(stdscr);
}
#endif

static grid_t *get_grid(const grid_t *grid_head,const int row, const int col)
{
	const grid_t *gd=grid_head;
	if(NULL==gd){
		return NULL;
	}
	
	if((row>gd->row-1)||(col>gd->col-1)){// 0<=row<row_num, 0<=col<col_num,
		return NULL;
	}
	int y=0;
	int x=0;
	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;
	for(cur_row=gd->down; cur_row!=NULL; cur_row=cur_row->down,y++,x=0){
		for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right,x++){
			if(row==y&&col==x){
				return cur_col;
			}
		}
	}

	return NULL;
}

static int deal_btn1_event(grid_t *grid, minesweeper_t *minesweeper)
{
	grid_t *gd=grid;
	minesweeper_t *msr=minesweeper;
	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;
	BOOL ret;
	
	if(NULL==gd||NULL==msr||NULL==msr->cross_grid_list||\
		NULL==msr->cross_grid_list->grid_head){
		return MSR_FAILED;
	}
	grid_t *gh=msr->cross_grid_list->grid_head;
	if(gd->value.is_swept==TRUE){
		return MSR_SUCCESS;
	}else{
		if(gd->value.flag!=FLAG_NONE){
			return MSR_SUCCESS;
		}
		gd->value.is_swept=TRUE;
		if(gd->value.is_mine==TRUE){
			gd->value.paint=MINE_PAINT_UNSAFE_MINE;
			//Find all mines
			for(cur_row=gh->down; cur_row!=NULL; cur_row=cur_row->down){
				for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
					if(cur_col->value.is_swept==FALSE){
						if(cur_col->value.is_mine==TRUE){
							cur_col->value.is_swept=TRUE;
						}
					}
				}
			}
			msr->status=STATUS_END_FAILED;
		}else{
			traverse_grid(gd,msr->cross_grid_list->grid_head);
			ret=is_success_minesweeper(msr->cross_grid_list->grid_head);
			if(ret==TRUE){
				msr->status=STATUS_END_SUCCESS;
			}
		}
	}
	return MSR_SUCCESS;
}

static int deal_btn2_event(grid_t *grid, minesweeper_t *minesweeper)
{
	grid_t *gd=grid;
	minesweeper_t *msr=minesweeper;

	if(gd->value.is_swept==TRUE){
		return MSR_SUCCESS;
	}else{
		switch(gd->value.flag){
			case FLAG_MINE:
				gd->value.flag=FLAG_UNKNOW;
				msr->cur_mine_num++;
				break;
			case FLAG_UNKNOW:
				gd->value.flag=FLAG_NONE;
				break;
			case FLAG_NONE:
				gd->value.flag=FLAG_MINE;
				msr->cur_mine_num--;
				break;
		}
	}
	
	return MSR_SUCCESS;
}

static grid_t *traverse_grid(grid_t *grid,const grid_t *grid_head)
{
	grid_t *gd=grid;
    const grid_t *gh=grid_head;
	if(NULL==gd||gh==gd){
		return NULL;
	}
	if(gd->value.mine_num!=MINE_NUM_ZERO){
		return NULL;
	}
	gd->value.is_swept=TRUE;
	BOOL around_mine=FALSE;

	if(around_mine==FALSE){
		//continue to sweep other grid
		if(NULL!=gd->up&&gh!=gd->up){
			if(gd->up->value.is_swept!=TRUE&&gd->up->value.flag!=FLAG_MINE){
				traverse_grid(gd->up,gh);
			}
			if(NULL!=gd->up->left&&gh!=gd->up->left){
				if(gd->up->left->value.is_swept!=TRUE&&gd->up->left->value.flag!=FLAG_MINE){
					traverse_grid(gd->up->left,gh);
				}
			}
			if(NULL!=gd->up->right){
				if(gd->up->right->value.is_swept!=TRUE&&gd->up->right->value.flag!=FLAG_MINE){
					traverse_grid(gd->up->right,gh);
				}
			}
			
		}

		if(NULL!=gd->left&&gh!=gd->left){
			if(gd->left->value.is_swept!=TRUE&&gd->left->value.flag!=FLAG_MINE){
				traverse_grid(gd->left,gh);
			}
		}
		
		if(NULL!=gd->right){
			if(gd->right->value.is_swept!=TRUE&&gd->right->value.flag!=FLAG_MINE){
				traverse_grid(gd->right,gh);
			}
		}

		if(NULL!=gd->down){
			if(gd->down->value.is_swept!=TRUE&&gd->down->value.flag!=FLAG_MINE){
				traverse_grid(gd->down,gh);
			}
			if(NULL!=gd->down->left){
				if(gd->down->left->value.is_swept!=TRUE&&gd->down->left->value.flag!=FLAG_MINE){
					traverse_grid(gd->down->left,gh);
				}
			}
			
			if(NULL!=gd->down->right){
				if(gd->down->right->value.is_swept!=TRUE&&gd->down->right->value.flag!=FLAG_MINE){
					traverse_grid(gd->down->right,gh);
				}
			}
			
		}

		//Set the surrounding grid swept
		if(NULL!=gd->up&&gh!=gd->up){
			if(gd->up->value.is_swept!=TRUE&&gd->up->value.flag!=FLAG_MINE){
				gd->up->value.is_swept=TRUE;
			}
			if(NULL!=gd->up->left&&gh!=gd->up->left){
				if(gd->up->left->value.is_swept!=TRUE&&gd->up->left->value.flag!=FLAG_MINE){
					gd->up->left->value.is_swept=TRUE;
				}
			}
			if(NULL!=gd->up->right){
				if(gd->up->right->value.is_swept!=TRUE&&gd->up->right->value.flag!=FLAG_MINE){
					gd->up->right->value.is_swept=TRUE;
				}
			}
		}

		if(NULL!=gd->left&&gh!=gd->left){
			if(gd->left->value.is_swept!=TRUE&&gd->left->value.flag!=FLAG_MINE){
				gd->left->value.is_swept=TRUE;
			}
		}
		
		if(NULL!=gd->right){
			if(gd->right->value.is_swept!=TRUE&&gd->right->value.flag!=FLAG_MINE){
				gd->right->value.is_swept=TRUE;
			}
		}

		if(NULL!=gd->down){
			if(gd->down->value.is_swept!=TRUE&&gd->down->value.flag!=FLAG_MINE){
				gd->down->value.is_swept=TRUE;
			}
			if(NULL!=gd->down->left){
				if(gd->down->left->value.is_swept!=TRUE&&gd->down->left->value.flag!=FLAG_MINE){
					gd->down->left->value.is_swept=TRUE;
				}
			}
			if(NULL!=gd->down->right){
				if(gd->down->right->value.is_swept!=TRUE&&gd->down->right->value.flag!=FLAG_MINE){
					gd->down->right->value.is_swept=TRUE;
				}
			}
		}

	}
	
	return NULL;
}

static BOOL is_success_minesweeper(const grid_t *grid_head)
{
	const grid_t *gh=grid_head;
	if(NULL==gh){
		return FALSE;
	}

	grid_t *cur_row=NULL;
	grid_t *cur_col=NULL;
	for(cur_row=gh->down; cur_row!=NULL; cur_row=cur_row->down){
		for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
			if(cur_col->value.is_mine==FALSE){
				if(cur_col->value.is_swept==FALSE){
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

void paint_minesweeper(minesweeper_t *minesweeper)
{
	minesweeper_t *msr=minesweeper;
	if(NULL==msr){
		return;
	}
	clear_minesweeper();
	
	draw_minesweeper(msr);
	
#if 0
	draw_mines(msr->cross_grid_list->grid_head);
#endif

}

int handle_minesweeper(minesweeper_t *minesweeper)
{
	minesweeper_t *msr=minesweeper;
	MEVENT *mt=&msr->mevent;
	if(NULL==msr||NULL==mt){
		return MSR_FAILED;
	}
	int row=msr->mevent.y-msr->battlefield->begin_y-1;
	int col=msr->mevent.x-msr->battlefield->begin_x-1;
	
	grid_t *gd=get_grid(msr->cross_grid_list->grid_head,row,col);
	if(gd==NULL){
		return MSR_FAILED;
	}
	
	switch(mt->bstate){
		case BUTTON1_RELEASED:
		case BUTTON1_PRESSED:
		case BUTTON1_CLICKED:
		case BUTTON1_DOUBLE_CLICKED:
		case BUTTON1_TRIPLE_CLICKED:
			msr->status=STATUS_START;
			deal_btn1_event(gd,msr);
			break;
	
		case BUTTON2_RELEASED:
		case BUTTON2_PRESSED:
		case BUTTON2_CLICKED:
		case BUTTON2_DOUBLE_CLICKED:
		case BUTTON2_TRIPLE_CLICKED:
			msr->status=STATUS_START;
			deal_btn2_event(gd,msr);
			break;
	}
	
	return MSR_SUCCESS;
}

MEVENT mouse_event(battlefield_t *battlefield)
{ 
	battlefield_t *bf=battlefield;
	int ch=-1;
	MEVENT mevent;
	mevent.x=MOUSE_NON_VALID_X;
	mevent.y=MOUSE_NON_VALID_Y;
	if(NULL==bf){
		return mevent;
	}
	
	mousemask(ALL_MOUSE_EVENTS,NULL);
	
	ch=wgetch(bf->win);
	if(-1!=ch){
		if(KEY_MOUSE==ch){
			if(getmouse(&mevent)!=OK){
				if(has_colors()){attron(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
				mvwprintw(bf->win,\
					bf->begin_y+bf->nlines-2,\
					bf->begin_x+1,"%s","getmouse event failed");	
				if(has_colors()){attroff(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
			}else{
				if(mevent.y>=bf->begin_y+1&&mevent.y<=bf->begin_y+bf->nlines-4&&\
					mevent.x>=bf->begin_x+1&&mevent.x<=bf->begin_x+bf->ncols-2){
					wmove(bf->win,mevent.y,mevent.x);
				}else{
					mevent.x=MOUSE_NON_VALID_X;
					mevent.y=MOUSE_NON_VALID_Y;
				}		
			}
		}
	}
	
	return mevent;
}

void exit_minesweeper(minesweeper_t *minesweeper)
{
	minesweeper_t *msr=minesweeper;
	
	do{
		if(NULL==msr){
			break;
		}

		free_battlefield(msr->battlefield);
		msr->battlefield=NULL;

		end_screen();

		free_cross_grid_list(msr->cross_grid_list);
		msr->cross_grid_list=NULL;
		
		free(msr);
		msr=NULL;
		
	}while(0);
	
	return ;
}

