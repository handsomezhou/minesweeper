#include <stdlib.h>
#include "minesweeper.h"


static int init_grid_head(grid_t **grid);
static int init_grid(grid_t *row_head, grid_t *col_head, int row_num, int col_num);
void show_grid(cross_grid_list_t *cross_grid_list);	//just for test

static void free_grid(grid_t *grid_head);

static int init_cross_grid_list(cross_grid_list_t **cross_grid_list);
static void free_cross_grid_list(cross_grid_list_t  *cross_grid_list);

static int init_battlefield(battlefield_t **battlefield);
static void free_battlefield(battlefield_t *battlefield);

static int init_grid_head(grid_t **grid)
{
	grid_t **gd=grid;
	if(NULL!=*gd){							
		free_grid(*gd);
		*gd=NULL;
	}

	*gd=(grid_t *)malloc(sizeof(grid_t));
	if(NULL==*gd){
		return INIT_FAILED;
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
	
	return INIT_SUCCESS;
}

static int init_grid(grid_t *row_head, grid_t *col_head, int row_num, int col_num)
{
	grid_t *rh=row_head;
	grid_t *ch=col_head;
	int row=(row_num>=MIN_ROW && row_num<=MAX_ROW)?(row_num):(MIN_ROW);
	int col=(col_num>=MIN_COL && col_num<=MAX_COL)?(col_num):(MIN_COL);
	int x=0;
	int y=0;
	
	if(NULL==rh||NULL==ch){
		return INIT_FAILED;
	}
	grid_t *cur_row=rh;
	grid_t *cur_col=ch;
	for(y=0; y<row; y++){
		cur_row->down=(grid_t *)malloc(sizeof(grid_t));
		if(NULL==cur_row->down){
			return INIT_FAILED;
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
				cur_col=cur_col->right;
			}else{
				cur_col->row=y;
				cur_col->col=x;
			}
				
		}	
	}

	grid_t *prev_row=rh->down;
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
	
	return INIT_SUCCESS;
}

void show_grid(cross_grid_list_t *cross_grid_list)
{
	cross_grid_list_t *cgl=cross_grid_list;
	do{
		if(NULL==cgl||NULL==cgl->row_head||NULL==cgl->col_head){
			break;
		}

		grid_t *cur_row=cgl->row_head->down;
		grid_t *cur_col=cgl->col_head->right;
#if 1
		for(cur_row;cur_row!=NULL; cur_row=cur_row->down){
			for(cur_col=cur_row; cur_col!=NULL; cur_col=cur_col->right){
			//printf("(%d,%d)[%p][%p][%p][%p] ",cur_col->row,cur_col->col,cur_col->up,cur_col->right,cur_col->down,cur_col->left);
			//printf("[%9p] ",cur_col->up);
			//printf("[%9p] ",cur_col->left);
			//printf("[%9p] ",cur_col->right);
			//printf("[%9p] ",cur_col->down);
			}
			printf("\n");
		}
#else
		for(cur_col=cgl->col_head->right; cur_col!=NULL; cur_col=cur_col->right){
			for(cur_row=cur_col;cur_row!=NULL; cur_row=cur_row->down){
				printf("(%d,%d)\t",cur_row->col,cur_row->row);
			}
			printf("\n");
		}
#endif
	}while(0);
	
	return;
}


static void free_grid(grid_t *grid_head)
{
	
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
	
		(*cgl)->row_head=NULL;
		(*cgl)->col_head=NULL;
		(*cgl)->row_num=MIN_ROW;
		(*cgl)->col_num=MIN_COL;
		(*cgl)->mine_num=MIN_MINE;
		ret=init_grid_head(&(*cgl)->row_head);
		if(INIT_FAILED==ret){
			break;
		}

		ret=init_grid_head(&(*cgl)->col_head);
		if(INIT_FAILED==ret){
			break;
		}
		//just for test
		init_grid((*cgl)->row_head,(*cgl)->col_head,(*cgl)->row_num,(*cgl)->col_num);
		show_grid(*cgl);
		return INIT_SUCCESS;
	}while(0);

	return INIT_FAILED;
}

static void free_cross_grid_list(cross_grid_list_t  *cross_grid_list)
{
	cross_grid_list_t *cgl=cross_grid_list;
	if(NULL==cgl){
		return;
	}

	free_grid(cgl->col_head);
	free_grid(cgl->row_head);
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
		return INIT_FAILED;
	}
	(*bf)->win=stdscr;
	(*bf)->nlines=BATTLEFIELD_NLINES;
	(*bf)->ncols=BATTLEFIELD_NCLOS;
	(*bf)->begin_y=BATTLEFIELD_BEGIN_Y;
	(*bf)->begin_x=BATTLEFIELD_BEGIN_X;
	
	return INIT_SUCCESS;
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
		(*msr)->cur_mine_num=MIN_MINE;
		(*msr)->timer=TIMER_START;
		(*msr)->status=STATUS_PAUSE;
		
		ret=init_cross_grid_list(&(*msr)->cross_grid_list);
		if(ret==INIT_FAILED){
			break;
		}

		ret=init_battlefield(&(*msr)->battlefield);
		if(INIT_FAILED==ret){
			break;
		}
		
		return INIT_SUCCESS;
	}while(0);

	return INIT_FAILED;
	
}

void exit_minesweeper(minesweeper_t *minesweeper)
{
	minesweeper_t *msr=minesweeper;
	
	do{
		if(NULL==msr){
			break;
		}
		free_cross_grid_list(msr->cross_grid_list);
		free_battlefield(msr->battlefield);
		free(msr);
		msr=NULL;
		
	}while(0);
	
	return ;
}


