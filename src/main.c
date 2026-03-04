#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <string.h>
#include <errno.h>

void INThandler(int signum)
{
	endwin();
	exit(0);
}

void mouseClick(Display *display, int button)
{
	// Press the button
	XTestFakeButtonEvent(display, button, True, 0);
	XFlush(display);
	usleep(100000);

	// Release the button
	XTestFakeButtonEvent(display, button, False, 0);
	XFlush(display);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <click_delay-msecs>\n", argv[0]);
		return 1;
	}
	int delay = atoi(argv[1]);

	Display *display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		fprintf(stderr, "Cannot initialize the display\n");
		return 1;
	}

	initscr();
	cbreak();
	noecho();

	char dev_keyboard[] = "/dev/input/event3";
	int keyboard = open(dev_keyboard, O_RDONLY | O_NONBLOCK);
	if (keyboard < 0)
	{
		perror("Failed to open keyboard");
		XCloseDisplay(display);
		endwin();
		return 1;
	}

	struct input_event ev;

	signal(SIGINT, INThandler);

	bool autoclickerOn = false;

	Window root = RootWindow(display, DefaultScreen(display));
	Window root_ret, child_ret;
	int root_x, root_y, win_x, win_y;
	unsigned int mask;

	while (1)
	{
		ssize_t bytes_read = read(keyboard, &ev, sizeof(ev));

		if (bytes_read > 0 && ev.type == 1 && ev.value == 1)
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
			{
				XCloseDisplay(display);
				endwin();
				return 0;
			}
		}
		refresh();

		if (!autoclickerOn && !XQueryPointer(display, root, &root_ret, &child_ret, &root_x, &root_y, &win_x, &win_y, &mask))
			printw("[ERROR] Could not get current mouse location!\n");

		if (autoclickerOn)
		{
			XTestFakeMotionEvent(display, 0, root_x, root_y, 0);
			XFlush(display);
			mouseClick(display, Button1);
			usleep(delay * 1000);
		}
	}

	XCloseDisplay(display);
	endwin();

	return 0;
}