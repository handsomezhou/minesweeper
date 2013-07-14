#include "minesweeper.h"

int main(void)
{
	minesweeper_t *minesweeper=NULL;
	
	init_minesweeper(&minesweeper);
	
	exit_minesweeper(minesweeper);
	
	return 0;
}

