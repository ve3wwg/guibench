/* app.h */

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

void app_main();
void clear_window();
void get_dim(int *px,int *py);
void draw_point(int x,int y,int pen);
int get_pixel(int x,int y);

#ifdef __cplusplus
}
#endif
#endif

// app.h
