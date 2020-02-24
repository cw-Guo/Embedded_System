#include "mypaint.h"

int main(){
    unsigned char *fbp;
	unsigned long screensize; 
    fbp = fb_open();
	screensize=get_size();    
//unsigned long screensize = (vinfo.xres * vinfo.yres * vinfo.bits_per_pixel)/8;
    
    draw_line(fbp,20,20,200,200,255,0,0);
	draw_circle(fbp,300,300,50,255,255,0);    
	sleep(15);

    draw_block(fbp,20,20,1024,768,255,0,0);
    sleep(10);
	
    memset(fbp,0,screensize);
    munmap(fbp,screensize);
    return 0;
}
