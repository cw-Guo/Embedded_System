#ifndef __MYPAINT__H_
#define __MYPAINT__H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <string.h> 
#include <math.h>


void draw_pixel(unsigned char * fbp, int x, int y, char Red, char Green, char Blue);
void draw_block(unsigned char * fbp, int x0, int y0, int x1, int y1, char Red, char Green, char Blue);
void draw_line(unsigned char * fbp, int x0, int y0, int x1, int y1, char Red, char Green, char Blue);
void draw_circle(unsigned char * fbp, int x0, int y0, float r, char Red, char Green, char Blue);
unsigned long  get_size();
int max(int a, int b);

unsigned char* fb_open();

#endif  //__MYPAINT__C_
