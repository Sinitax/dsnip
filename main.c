/* REF: https://github.com/rDr4g0n/gidoodle */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include <Imlib2.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))

const char *filename;
XWindowAttributes scr;
Display *display;
Window root, win;
int rx, ry, rw, rh;
GC gc;

void
die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(1);
}

int
handleopt(char flag, const char *arg)
{
	switch (flag) {
	case 'h':
		die("USAGE: dsnip [-h] [-f filename]\n");
	case 'f':
		if (arg) filename = arg;
		return 1;
	default:
		die("Bad flag: -%c\n", flag);
	}

	die("Bad argument: -%c %s\n", flag, arg);
	return 0;
}

void
parseargs(int argc, const char **argv)
{
	int i;

	filename = "out.png";
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && !argv[i][2])
			i += handleopt(argv[i][1], argv[i+1]);
		else
			die("Bad argument: %s\n", argv[i]);
	}
}

void
update(int x1, int y1, int x2, int y2)
{
	rx = MIN(x1, x2);
	ry = MIN(y1, y2);
	rw = MAX(1, MAX(x1, x2) - MIN(x1, x2));
	rh = MAX(1, MAX(y1, y2) - MIN(y1, y2));
}

void
init(void)
{
	XSetWindowAttributes swa;
	XGCValues gcvals;
	Screen *screen;

	display = XOpenDisplay(NULL);
	screen = XDefaultScreenOfDisplay(display);

	root = DefaultRootWindow(display);
	XGetWindowAttributes(display, root, &scr);

	swa.override_redirect = 1;
	win = XCreateWindow(display, root, scr.x, scr.y, scr.width, scr.height,
		0, CopyFromParent, InputOutput, CopyFromParent,
		CWOverrideRedirect, &swa);
	XMapWindow(display, win);

	gcvals = (XGCValues) { 0 };
	gcvals.foreground = XWhitePixelOfScreen(screen);
	gcvals.function = GXxor;
	gc = XCreateGC(display, win, GCFunction | GCForeground, &gcvals);
}

void
deinit(void)
{
	XDestroyWindow(display, win);
	XCloseDisplay(display);
}

void
saveimg(void)
{
	Imlib_Image img;

	imlib_context_set_display(display);
	imlib_context_set_visual(DefaultVisual(display, 0));
	imlib_context_set_drawable(win);

	img = imlib_create_image_from_drawable(0, rx, ry, rw, rh, 1);

	imlib_context_set_image(img);
	imlib_save_image(filename);
	imlib_free_image();
}

void
capture(void)
{
	int draw, dirty, done;
	int px, py, x, y, keysym;
	Cursor cursor;
	XEvent ev;

	cursor = XCreateFontCursor(display, XC_left_ptr);

	XGrabPointer(display, root, False,
		ButtonMotionMask | ButtonPressMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, root, cursor, CurrentTime);

	XGrabKeyboard(display, root, False, GrabModeAsync,
		GrabModeAsync, CurrentTime);

	done = dirty = 0;
	while (!done) {
		if (!XPending(display)) continue;
		XNextEvent(display, &ev);
		switch (ev.type) {
		case KeyPress:
			keysym = XkbKeycodeToKeysym(display, ev.xkey.keycode, 0,
				ev.xkey.state & ShiftMask ? 1 : 0);
			if (keysym == XK_Escape) {
				deinit();
				exit(0);
			}
			break;
		case MotionNotify:
			if (draw && dirty) {
				XDrawRectangle(display, win, gc,
					rx, ry, rw, rh);
			}

			x = ev.xmotion.x;
			y = ev.xmotion.y;
			update(px, py, x, y);

			if (draw) {
				XDrawRectangle(display, win, gc,
					rx, ry, rw, rh);
				dirty = 1;
				XFlush(display);
			}
			break;
		case ButtonPress:
			px = ev.xbutton.x;
			py = ev.xbutton.y;
			draw = 1;
			break;
		case ButtonRelease:
			done = 1;
			break;
		}
	}
}

int
main(int argc, const char **argv)
{
	parseargs(argc, argv);

	init();

	capture();

	saveimg();

	deinit();
}
