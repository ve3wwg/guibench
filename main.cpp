//////////////////////////////////////////////////////////////////////
// main.cpp -- Main Linux X11 guibench framework for GUI code
// Date: Mon Mar 27 10:00:36 2023   (C) ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include "app.h"

#include <thread>
#include <mutex>
#include <atomic>

static void init_x();
static void close_x();
static void redraw();
static void start_app();

static Display *dis{nullptr};
static int screen{0};
static Window win{0};
static GC gc;

static std::mutex thread_lock;
static std::atomic<bool> app_done{false};

static unsigned long black, white;
static int win_x{192}, win_y{192}, pixel_multiplier{1};
static int pen{1};

static std::recursive_mutex mutex;
static uint8_t *pixels{nullptr};

static void
usage(char const *cmd) {
	char const *cp = strrchr(cmd,'/');

	if ( !cp )
		cp = cmd;
	else	++cp;
	printf("Usage: %s [--multi('m')=m] [--width(x)=n] [--height(y)=n] [--help(h)]\n"
		"where:\n"
		"\t-m is number of pixels (default 1)\n"
		"\t-x is number of pixels wide (default 192)\n"
		"\t-y is number of pixels high (default 192)\n"
		"\t-h is this help.\n",
		cp);
}

//////////////////////////////////////////////////////////////////////
// Start the test code in parallel with X11 event loop
//////////////////////////////////////////////////////////////////////

static void
start_app() {

	thread_lock.lock();			// Don't fully start yet
	app_main();				// Run app main code
	XFlush(dis);				// Flush any pending writes
	printf("app_main() returned..\n");
	sleep(3);				// Pause..
	app_done.store(true);			// Tell X11 loop to exit
}

//////////////////////////////////////////////////////////////////////
// Main program: Options processing and X11 loop
//////////////////////////////////////////////////////////////////////

int
main(int argc,char **argv) {
	static struct option long_options[] = {
		{ "multi",	required_argument,	0, 'm' },
		{ "width",	required_argument,	0, 'x' },
		{ "height",	required_argument,	0, 'y' },
		{ "help",	no_argument,		0, 'h' },
		{ nullptr,	0,			0, 0 }
	};
	int c, option_index;
	
	while ( (c = getopt_long(argc,argv,"m:x:y:h",long_options,&option_index)) != -1 ) {
		switch ( c ) {
		case 'm':
			{
				int optval{atoi(optarg)};

				if ( optarg > 0 )
					pixel_multiplier = optval;
			}
			break;
		case 'x':
			{
				int optval{atoi(optarg)};

				if ( optarg > 0 )
					win_x = optval;
			}
			break;
		case 'y':
			{
				int optval{atoi(optarg)};

				if ( optarg > 0 )
					win_y = optval;
			}
			break;
		case 'h':
			usage(argv[0]);
			exit(0);
		default:
			usage(argv[0]);
			exit(1);
		}
		
	}

	// Set std i/o to be unbuffered
	setvbuf(stdout,nullptr,_IONBF,0);
	setvbuf(stderr,nullptr,_IONBF,0);
	init_x();

	XEvent event;			// the XEvent declaration
	KeySym key;			// for KeyPress Events
	char text[255];			// a char buffer for KeyPress Events

	static bool do_unlock{true};	// When true, hold back test code
	thread_lock.lock();		// Prevent app_main() from getting too far
	std::thread app(start_app);	// Thread for app_main()

	// X11 event loop

	while ( !app_done.load() ) {	// Loop until test code exits
		if ( !XPending(dis) ) {
			usleep(100);	// Nothing to do..
			continue;
		}

		XNextEvent(dis,&event);	// Read X11 event

		mutex.lock();		// Lock out test code for now

		switch ( event.type ) {
		case Expose:
			if ( event.xexpose.count == 0 ) {
				// the window was exposed redraw it!
				redraw();
				if ( do_unlock ) {
					thread_lock.unlock();	// Unleash the cracken
					do_unlock = false;	// But only once
				}
			}
			break;
		case KeyPress:
			if ( XLookupString(&event.xkey,text,255,&key,0) == 1 ) {
				char ch{text[0]};

				printf("You pressed the %c key!\n",ch);

				switch ( ch ) {
				case 'q':
				case 'Q':
					close_x();
					break;
				case 'c':
					clear_window();
					pen = 1;
					break;
				case 'x':
					pen ^= 1;
					printf("Pen now = %d\n",pen);
					break;
				}
			}
			break;
		case ButtonPress:
			// tell where the mouse Button was Pressed
			int x = event.xbutton.x / pixel_multiplier;
			int y = event.xbutton.y / pixel_multiplier;

			draw_point(x,y,pen);
		}
		mutex.unlock();
	}

	app.join();
	if ( pixels ) {
		free(pixels);
		pixels = nullptr;
	}

	XFreeGC(dis,gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);	
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Initialize the X11 Window
//////////////////////////////////////////////////////////////////////

static void
init_x() {

	XInitThreads();
	dis=XOpenDisplay((char *)0);		// Our display device
   	screen=DefaultScreen(dis);		// Our screen device

	black=BlackPixel(dis,screen);		// Black colour
	white=WhitePixel(dis,screen);		// White colour

   	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,win_x*pixel_multiplier,win_y*pixel_multiplier,5,black,white);
	XSetStandardProperties(dis,win,"GUI Bench","GB",None,nullptr,0,nullptr);
	XSelectInput(dis,win,ExposureMask|ButtonPressMask|KeyPressMask);
        gc=XCreateGC(dis,win,0,0);        
	XSetBackground(dis,gc,white);
	XSetForeground(dis,gc,black);
	XClearWindow(dis,win);
	XMapRaised(dis,win);

	pixels = (uint8_t*)malloc(win_x*win_y);
	memset(pixels,0,win_x * win_y);
};

//////////////////////////////////////////////////////////////////////
// Close down TestBench and app
//////////////////////////////////////////////////////////////////////

static void
close_x() {
	XFreeGC(dis,gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);	
	exit(1);				
}

//////////////////////////////////////////////////////////////////////
// Clear Window
//////////////////////////////////////////////////////////////////////

static void
redraw() {
	clear_window();
	memset(pixels,0,win_x*win_y);
}

//////////////////////////////////////////////////////////////////////
// Draw one virtual pixel
//////////////////////////////////////////////////////////////////////

void
draw_point(int x,int y,int pen) {

	assert(x >= 0 && x < win_x);	// X out of range?
	assert(y >= 0 && y < win_y);	// Y out of range?

	int px = y * win_y + x;		// Pixel subscript

	mutex.lock();			// Single thread X11 handling

	if ( !pen )
		XSetForeground(dis,gc,white);

	if ( pixel_multiplier == 1 ) {
		XDrawPoint(dis,win,gc,x,y);
	} else	{
		int mx = x * pixel_multiplier;
		int my = y * pixel_multiplier;

		XFillRectangle(dis,win,gc,mx,my,pixel_multiplier,pixel_multiplier);
	}
	if ( !pen )
		XSetForeground(dis,gc,black);
	XFlush(dis);

	pixels[px] = !!pen;
	mutex.unlock();
}

//////////////////////////////////////////////////////////////////////
// Clear Window
//////////////////////////////////////////////////////////////////////

void
clear_window() {
	mutex.lock();
	XClearWindow(dis,win);
	mutex.unlock();
}

//////////////////////////////////////////////////////////////////////
// Return window dimensions to tested code
//////////////////////////////////////////////////////////////////////

void
get_dim(int *px,int *py) {

	assert(px);
	assert(py);
	*px = win_x;
	*py = win_y;
}

//////////////////////////////////////////////////////////////////////
// GetPixel (slow)
//////////////////////////////////////////////////////////////////////

int
get_pixel(int x,int y) {
	assert(x >= 0 && x < win_x);	
	assert(y >= 0 && y < win_y);
	int px = y * win_y + x;

	return pixels[px];
}

// End main.cpp

