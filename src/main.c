#include <stdio.h>
#include <ncurses.h>
#include <stdbool.h>

int main(void)
{
	initscr();
	cbreak();
	noecho();
	timeout(0);

	bool autoclickerOn = false;

	int c;
	printw("Press 's' to start autoclicker, press 'p' to pause autoclicker, press 'q' to exit.\n");

	while ((c = getch()) != 'q')
	{
		if (c != ERR)
		{
			if (c == 's' && !autoclickerOn)
			{
				autoclickerOn = true;
				printw("Autoclicker started!\n");
			}
			if (c == 'p' && autoclickerOn)
			{
				autoclickerOn = false;
				printw("Autoclicker paused!\n");
			}
			refresh();
		}
	}

	endwin();

	return 0;
}
