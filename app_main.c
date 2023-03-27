#include <unistd.h>
#include <stdio.h>

#include "app.h"

static int dimx, dimy;

void
app_main() {

	get_dim(&dimx,&dimy);	
	printf("Window is %d x %d\n",dimx,dimy);

	clear_window();	

	// Draw an X
	for ( int x = 0; x < dimx; ++x ) {
		int y = x;

		if ( y < dimy ) {
			draw_point(x,y,1);
			draw_point(dimx - x - 1,y,1);
		}
	}

#if 0	// Test get_pixel()
	printf("Pixel(0,0)=%d, Pixel(1,0)=%d\n",
		get_pixel(0,0),
		get_pixel(1,0));
#endif
}
