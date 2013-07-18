#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "minesweeper.h"

int main(void)
{
	minesweeper_t *msr=NULL;
	MEVENT mevent;
	srand((unsigned int)(time(NULL)));
	
	int ret=init_minesweeper(&msr);
	if(MSR_FAILED==ret){
		return MSR_FAILED;
	}

	while(msr->status!=STATUS_OVER)
	{
		
//	while(1){
		mevent=mouse_event(msr->battlefield);
		if(mevent.y!=MOUSE_NON_VALID_Y&&mevent.x!=-MOUSE_NON_VALID_X){
			msr->mevent=mevent;
			handle_minesweeper(msr);
		}
		paint_minesweeper(msr);
//	}
		usleep(10000);
	}
	//just for test
	nodelay(stdscr,FALSE);
	wgetch(stdscr);
	werase(stdscr);
	mvwprintw(stdscr,1,1,"%s","Game over!");
	wgetch(stdscr);
	exit_minesweeper(msr);
	
	return MSR_SUCCESS;
}
