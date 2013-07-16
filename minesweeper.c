#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "minesweeper.h"


static int init_grid_head(grid_t **grid);
static int init_grid(grid_t *grid_head, int row_num, int col_num);
static int *set_mine_pos(int row_num, int col_num, int mine_num, int *pos);
static BOOL is_in_pos(int *pos, int pos_len, int num);

static int *sort_mine_pos(int *pos, int len);
static int init_mine(grid_t *grid_head, int mine_num);
void show_grid(grid_t *grid_head);	//just for test

static void free_grid(grid_t *grid_head);

static int init_cross_grid_list(cross_grid_list_t **cross_grid_list);
static void free_cross_grid_list(cross_grid_list_t  *cross_grid_list);

static int init_battlefield(battlefield_t **battlefield);
static void free_battlefield(battlefield_t *battlefield);

static int init_screen(cross_grid_list_t *cross_grid_list);
static void end_screen(void);

static void draw_init_msg();
static int get_cross_grid_set(cross_grid_list_t *cross_grid_list);
static BOOL is_digit_str(const char *str);
static int get_surround_mine_num(const grid_t *grid);

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
	(*gd)->value.content=CONTENT_ZERO;
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
				cur_col->right->value.flag=CONTENT_NONE;
				cur_col->right->value.flag=CONTENT_ZERO;
				cur_col=cur_col->right;
			}else{
				cur_col->row=y;
				cur_col->col=x;
				cur_col->value.is_mine=FALSE;
				cur_col->value.is_swept=FALSE;
				cur_col->value.flag=CONTENT_NONE;
				cur_col->value.content=CONTENT_ZERO;
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
static int *sort_mine_pos(int *pos, int len)
{

	return NULL;
}

static int init_mine(grid_t *grid_head, int mine_num)
{
#if 0
	grid_t *gh=grid_head;
	int mn=mine_num;
	if(NULL==gh||mn<0){
		return MSR_FAILED;
	}
	char *p=malloc(mn+1);
#endif
	return 0;
}

void show_grid(grid_t *grid_head)
{
	//werase(stdscr);
	//int y=1;
	//int x=1;
	grid_t *gh=grid_head;
	do{
		if(NULL==gh){
			break;
		}

		grid_t *cur_row=NULL;
		grid_t *cur_col=NULL;
#if 1
		for(cur_row=gh->down;cur_row!=NULL; cur_row=cur_row->down){
			for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
				printf("(%d,%d)",cur_col->row,cur_col->col);
				//mvwprintw(stdscr,y,x,"%d-%d ",cur_col->row,cur_col->col);
				//x +=5;
			}
			printf("\n");
			//y++;
		}
#if 0	
		for(cur_row=gh->down;cur_row!=NULL; cur_row=cur_row->down){
			for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
				printf("[%9p] ",cur_col->up);
			}
			printf("\n");
		}
		printf("\n");
		
		for(cur_row=gh->down;cur_row!=NULL; cur_row=cur_row->down){
			for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
				printf("[%9p] ",cur_col->right);
			}
			printf("\n");
		}
		printf("\n");

		for(cur_row=gh->down;cur_row!=NULL; cur_row=cur_row->down){
			for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
				printf("[%9p] ",cur_col->down);
			}
			printf("\n");
		}
		printf("\n");

		for(cur_row=gh->down;cur_row!=NULL; cur_row=cur_row->down){
			for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
				printf("[%9p] ",cur_col->left);
			}
			printf("\n");
		}
#endif
#else
		for(cur_col=gh->right; cur_col!=NULL; cur_col=cur_col->right){
			for(cur_row=cur_col;cur_row!=NULL; cur_row=cur_row->down){
			
			printf("(%d,%d)",cur_row->col,cur_row->row);
			}
			printf("\n");
		}
		printf("\n");
#endif
	}while(0);
	
	return;
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
			//just for test
			//printf("f:(%d,%d)",cur_col->row,cur_col->col);
			free(cur_col);
		}
			//printf("\n");
	}
	gh->down=NULL;
	gh->right=NULL;
	free(gh);
}


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
#if 0
		//just for test
		init_grid((*cgl)->grid_head,(*cgl)->row_num,(*cgl)->col_num);
		show_grid((*cgl)->grid_head);
#endif
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

static int init_battlefield(battlefield_t **battlefield)
{
	battlefield_t **bf=battlefield;
	if(NULL!=*bf){
		free_battlefield(*bf);
		*bf=NULL;
	}

	*bf=(battlefield_t *)malloc(sizeof(battlefield_t));
	if(NULL==*bf){
		return MSR_FAILED;
	}
	(*bf)->win=stdscr;
	(*bf)->nlines=BATTLEFIELD_NLINES;
	(*bf)->ncols=BATTLEFIELD_NCLOS;
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

static int init_screen(cross_grid_list_t *cross_grid_list)
{
	cross_grid_list_t *cgl=cross_grid_list;
	if(NULL==cgl){
		return MSR_FAILED;
	}
	initscr();
	cbreak();
	//noecho();
	curs_set(TRUE);
	clear();

	if(has_colors()){
		start_color();

		init_pair(COLOR_ZERO,COLOR_BLUE,COLOR_BLACK);
		init_pair(COLOR_ONE,COLOR_BLUE,COLOR_BLACK);
		init_pair(COLOR_TWO,COLOR_GREEN,COLOR_BLACK);
		init_pair(COLOR_THREE,COLOR_YELLOW,COLOR_BLACK);
		init_pair(COLOR_FOUR,COLOR_RED,COLOR_BLACK);
		init_pair(COLOR_FIVE,COLOR_RED,COLOR_BLACK);
		init_pair(COLOR_SIX,COLOR_RED,COLOR_BLACK);
		init_pair(COLOR_SEVEN,COLOR_RED,COLOR_BLACK);
		init_pair(COLOR_EIGHT,COLOR_RED,COLOR_BLACK);
		
		init_pair(COLOR_FLAG,COLOR_RED,COLOR_BLUE);
		init_pair(COLOR_UNKNOW,COLOR_WHITE,COLOR_BLUE);
		init_pair(COLOR_SAFE_MINE,COLOR_CYAN,COLOR_BLACK);
		init_pair(COLOR_UNSAFE_MINE,COLOR_RED,COLOR_BLACK);

		init_pair(COLOR_NORMAL_MSG,COLOR_YELLOW,COLOR_BLACK);
		init_pair(COLOR_PROMPT_MSG,COLOR_RED,COLOR_BLACK);

		init_pair(COLOR_INPUT_INFO,COLOR_GREEN,COLOR_BLACK);
	}

	nodelay(stdscr,FALSE);
	keypad(stdscr,TRUE);
	draw_init_msg();
	get_cross_grid_set(cgl);
	init_grid(cgl->grid_head,cgl->row_num,cgl->col_num);

	int *pos=(int *)malloc(cgl->mine_num);
	int i=0;
	for(i=0; i<cgl->mine_num; i++){
		*(pos+i)=-1;
	}
	set_mine_pos(cgl->row_num,cgl->col_num,cgl->mine_num,pos);
	int y=1;
	int x=4;
	werase(stdscr);
	for(i=0; i<cgl->mine_num; i++){
		mvwprintw(stdscr,y,x,"%d",*(pos+i));
		x=x+4;
		if(x/4==5){
			x=4;
			y++;
		}
	}
	wrefresh(stdscr);
	wgetch(stdscr);
//	sort_mine_pos(pos);
	
	//init_mine(cgl);
	
	return MSR_SUCCESS;
	
}
static void end_screen(void)
{
	endwin();
}

static void draw_init_msg()
{
	char game_help[]="Minesweeper Game Help!";	
	char direction[]="Direction key:";	
	char key_up[]   ="UP   :[w] [W] [i] [I] [Arrow up]";	
	char key_right[]="RIGHT:[d] [D] [l] [L] [Arrow right]";
	char key_down[] ="DOWN :[s] [S] [k] [K] [Arrow down]";	
	char key_left[] ="LEFT :[a] [A] [j] [J] [Arrow left]";

	char flag[]	 = "FLAG  :[f] [F]";
	char unknow[]= "UNKNOW:[?]";
	char swept[] = "SWEPT :spacebar";	
	
	char pause[] ="PAUSE :[p] [P]";	
	char quit[]	 ="EXIT  :[q] [Q] [Esc]";
	char help[]  ="HELP  :[h] [H]";
	
	char start[]="Press any key to continue!";
	char prompt[]="The window is too small, Please set the window larger!";
	int max_x;
	int max_y;
	int y=0;
	int x=1;
	
	getmaxyx(stdscr,max_y,max_x);
	
	if(has_colors()){attron(COLOR_PAIR(COLOR_NORMAL_MSG)|A_BOLD);}
	mvwprintw(stdscr,++y,(max_x-strlen(game_help))/2,"%s",game_help);
	mvwprintw(stdscr,++y,x,"%s",direction);
	mvwprintw(stdscr,++y,x,"\t%s",key_up);
	mvwprintw(stdscr,++y,x,"\t%s",key_right);
	mvwprintw(stdscr,++y,x,"\t%s",key_down);
	mvwprintw(stdscr,++y,x,"\t%s",key_left);
	mvwprintw(stdscr,++y,x,"%s",flag);
	mvwprintw(stdscr,++y,x,"%s",unknow);
	mvwprintw(stdscr,++y,x,"%s",swept);
	mvwprintw(stdscr,++y,x,"%s",pause);
	mvwprintw(stdscr,++y,x,"%s",quit);
	mvwprintw(stdscr,++y,x,"%s",help);
	
	box(stdscr,0,0);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_NORMAL_MSG)|A_BOLD);}

	if(has_colors()){attron(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	mvwprintw(stdscr,++y,(max_x-strlen(start))/2,"%s",start);
	if(y>=max_y-1){
		
		mvwprintw(stdscr,y/2,(max_x-strlen(prompt))/2,"%s",prompt);
	}
	if(has_colors()){attroff(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
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
	if(has_colors()){attron(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	
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

static int get_surround_mine_num(const grid_t *grid)
{
	return 0;
}

int init_minesweeper(minesweeper_t **minesweeper)
{
	minesweeper_t **msr=minesweeper;
	int ret;
	if(NULL==*msr){
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
		(*msr)->coordinates.y=COORDINATES_Y;
		(*msr)->coordinates.x=COORDINATES_X;
		(*msr)->cur_mine_num=MIN_MINE;
		(*msr)->timer=TIMER_START;
		(*msr)->status=STATUS_PAUSE;
		
		ret=init_cross_grid_list(&(*msr)->cross_grid_list);
		if(ret==MSR_FAILED){
			break;
		}

		ret=init_screen((*msr)->cross_grid_list);
		if(MSR_FAILED==ret){
			break;
		}
		
		ret=init_battlefield(&(*msr)->battlefield);
		if(MSR_FAILED==ret){
			break;
		}

		return MSR_SUCCESS;
	}while(0);

	return MSR_FAILED;
	
}

int key_event(minesweeper_t *minesweeper)
{
	//minesweeper_t *msr=minesweeper;

	return 0;
	
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
#if 0	//just for test
		show_grid(msr->cross_grid_list->grid_head);
		getch();
#endif
		free_cross_grid_list(msr->cross_grid_list);
		msr->cross_grid_list=NULL;
		
		free(msr);
		msr=NULL;
		
	}while(0);
	
	return ;
}

