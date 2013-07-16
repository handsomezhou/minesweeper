#include <stdlib.h>
#include <time.h>
#include "minesweeper.h"

int main(void)
{
	minesweeper_t *minesweeper=NULL;
	srand((unsigned int)(time(NULL)));
	
	init_minesweeper(&minesweeper);
	
	exit_minesweeper(minesweeper);
	
	return 0;
}

