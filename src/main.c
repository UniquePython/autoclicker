#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>

void INThandler()
{
	exit(0);
}

int main()
{
	initscr();
	cbreak();
	noecho();

	char devname[] = "/dev/input/event3"; // keyboard
	int device = open(devname, O_RDONLY);
	struct input_event ev;

	signal(SIGINT, INThandler);

	bool autoclickerOn = false;

	while (1)
	{
		ssize_t bytes_read = read(device, &ev, sizeof(ev));
		if (bytes_read != sizeof(ev))
		{
			perror("read");
			return 1;
		}

		if (ev.type == 1 && ev.value == 1)
		{
			if (ev.code == 31 && !autoclickerOn) // S = start
			{
				autoclickerOn = true;
				printw("Autoclicker on!\n");
			}
			if (ev.code == 25 && autoclickerOn) // P = pause
			{
				autoclickerOn = false;
				printw("Autoclicker off!\n");
			}
			if (ev.code == 16) // Q = quit
				return 0;
		}
		refresh();
	}

	endwin();

	return 0;
}