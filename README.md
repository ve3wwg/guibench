Simple X11 framework to test out MCU GUI code under Linux
=========================================================

Modify app_main.c to invoke your own MCU code to be tested. That code
should call C functions:

	clear_window()
	draw_point()
	get_pixel()

	as necessary (see app.h).

The framework is B&W only and get_pixel() is available as a
support function for development (if used).

USAGE:
------

./guibench --help
Usage: guibench [--multi('m')=m] [--width(x)=n] [--height(y)=n] [--help(h)]
where:
	-m is number of pixels (default 1)
	-x is number of pixels wide (default 192)
	-y is number of pixels high (default 192)
	-h is this help.

NOTES:
------
    1. For high resolution screens, choose --multi (-m) value > 1 to double
       up pixels for easier viewing. Default is -m 1.
    2. Options -x and -y define the size of the display window.
    3. The X11 event loop is handled by main.cpp thread. Thus the 
       tested C program (app_main.c) should not make X11 calls
       without special care.
    4. Tested C program runs in a separate thread from main.cpp.
    5. The draw_point() code is not meant to be efficient. It's
       intention is to allow testing of a MCU GUI drawing function.
    6. valgrind is your friend:

    	$  valgrind --leak-check=full --show-leak-kinds=all ./guibench

	These are normal from XInitThreads(). Ignore or drop the
	--show-leak-kinds=all option from the valgrind run.
	
    ==2256== HEAP SUMMARY:
    ==2256==     in use at exit: 120 bytes in 3 blocks
    ==2256==   total heap usage: 136 allocs, 133 frees, 195,912 bytes allocated
    ==2256== 
    ==2256== 40 bytes in 1 blocks are still reachable in loss record 1 of 3
    ==2256==    at 0x483877F: malloc (vg_replace_malloc.c:307)
    ==2256==    by 0x4885B99: XInitThreads (in /usr/lib/x86_64-linux-gnu/libX11.so.6.4.0)
    ==2256==    by 0x10A940: init_x() (main.cpp:211)
    ==2256==    by 0x10A67F: main (main.cpp:127)
    ==2256== 
    ==2256== 40 bytes in 1 blocks are still reachable in loss record 2 of 3
    ==2256==    at 0x483877F: malloc (vg_replace_malloc.c:307)
    ==2256==    by 0x4885BB6: XInitThreads (in /usr/lib/x86_64-linux-gnu/libX11.so.6.4.0)
    ==2256==    by 0x10A940: init_x() (main.cpp:211)
    ==2256==    by 0x10A67F: main (main.cpp:127)
    ==2256== 
    ==2256== 40 bytes in 1 blocks are still reachable in loss record 3 of 3
    ==2256==    at 0x483877F: malloc (vg_replace_malloc.c:307)
    ==2256==    by 0x4885BD3: XInitThreads (in /usr/lib/x86_64-linux-gnu/libX11.so.6.4.0)
    ==2256==    by 0x10A940: init_x() (main.cpp:211)
    ==2256==    by 0x10A67F: main (main.cpp:127)
    ==2256== 
    ==2256== LEAK SUMMARY:
    ==2256==    definitely lost: 0 bytes in 0 blocks
    ==2256==    indirectly lost: 0 bytes in 0 blocks
    ==2256==      possibly lost: 0 bytes in 0 blocks
    ==2256==    still reachable: 120 bytes in 3 blocks
    ==2256==         suppressed: 0 bytes in 0 blocks

