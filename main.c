/**
  *  Copyright (C) 2013  Handsome Zhou
  */

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "minesweeper.h"

int main(void)
{
	minesweeper_t *msr=NULL;
	MEVENT mevent;
	time_t start;
	time_t end;
	
	srand((unsigned int)(time(NULL)));
	int ret=init_minesweeper(&msr);
	if(MSR_FAILED==ret){
		return MSR_FAILED;
	}

	start=time(NULL);
	
	while(msr->status!=STATUS_END_FAILED&&msr->status!=STATUS_END_SUCCESS){
		mevent=mouse_event(msr->battlefield);
		if(mevent.y!=MOUSE_NON_VALID_Y&&mevent.x!=MOUSE_NON_VALID_X){
			msr->mevent=mevent;
			handle_minesweeper(msr);
		}
		paint_minesweeper(msr);
		usleep(200000);
		end=time(NULL);
		if(end!=start&&msr->status==STATUS_START){
			if(msr->timer!=MAX_PLAY_TIME){
				msr->timer++;
				start=end;
			}else{
				msr->timer=MAX_PLAY_TIME;
			}
			
		}
	}

	char lose[]="Sorry, you lose!";
	char win[]="Congratulations! You win!";
	char quit[]="Press any key to exit!";
	if(has_colors()){attron(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	switch(msr->status){
		case STATUS_END_FAILED:
			mvwprintw(msr->battlefield->win,msr->battlefield->begin_y+msr->battlefield->nlines,\
				msr->battlefield->begin_x+msr->battlefield->ncols/2-strlen(lose)/2,"%s",lose);
			break;
			
		case STATUS_END_SUCCESS:
			mvwprintw(msr->battlefield->win,msr->battlefield->begin_y+msr->battlefield->nlines,\
				msr->battlefield->begin_x+msr->battlefield->ncols/2-strlen(win)/2,"%s",win);
			break;
		default:
			break;
	}
	mvwprintw(stdscr,msr->battlefield->begin_y+msr->battlefield->nlines+1,\
		msr->battlefield->begin_x+msr->battlefield->ncols/2-strlen(quit)/2,"%s",quit);
	if(has_colors()){attroff(COLOR_PAIR(COLOR_PROMPT_MSG)|A_BOLD);}
	
	nodelay(stdscr,FALSE);
	wgetch(stdscr);
	
	exit_minesweeper(msr);
	
	return MSR_SUCCESS;
}
