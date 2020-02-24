/*
 *  tslib/src/ts_test.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the GPL.  Please see the file
 * COPYING for more details.
 *
 *
 * Basic test program for touchscreen library.
 */
#include "config.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <math.h>
#include "tslib.h"
#include "fbutils.h"

void circle(int x, int y, int r){
     int R = r;
    //范围判断

    int theta =0;
    for (theta = 0; theta <360; ++theta){
        int i = x + R * cos(theta);
        int j = y + R * sin(theta);
        pixel(i,j,2);
    }
}

static int palette [] =//调色板
{
	0x000000, 0xffe080, 0xffffff, 0xe0c0a0, 0x304050, 0x80b8c0
};
#define NR_COLORS (sizeof (palette) / sizeof (palette [0]))

struct ts_button { //调色板
	int x, y, w, h;
	char *text;
	int flags;
#define BUTTON_ACTIVE 0x00000001
};

/* [inactive] border fill text [active] border fill text */
static int button_palette [8] =
{
	1, 4, 2,1,
	1, 5, 0,1
};
void put_board(int x, int y, unsigned int colidx){
	rect(x-50,y-5,x+50,y+5,colidx);
	fillrect(x-50,y-5,x+50,y+5,colidx);
}

#define NR_BUTTONS 4
static struct ts_button buttons [NR_BUTTONS];


static void sig(int sig)
{
	close_framebuffer();
	fflush(stderr);
	printf("signal %d caught\n", sig);
	fflush(stdout);
	exit(1);
}

static void button_draw (struct ts_button *button)
{
	int s = (button->flags & BUTTON_ACTIVE) ? 4 : 0;
	rect (button->x, button->y, button->x + button->w - 1,
	      button->y + button->h - 1, button_palette [s]);
	fillrect (button->x + 1, button->y + 1,
		  button->x + button->w - 2,
		  button->y + button->h - 2, button_palette [s + 1]);
	put_string_center (button->x + button->w / 2,
			   button->y + button->h / 2,
			   button->text, button_palette [s + 2]);
}

static void my_scores_draw (struct ts_button *button)
{
	if(button->flags==1){
		button_draw(button);
	}
}

static int button_handle (struct ts_button *button, struct ts_sample *samp)
{
	int inside = (samp->x >= button->x) && (samp->y >= button->y) &&
		(samp->x < button->x + button->w) &&
		(samp->y < button->y + button->h);

	if (samp->pressure > 0) {
		if (inside) {
			if (!(button->flags & BUTTON_ACTIVE)) {
				button->flags |= BUTTON_ACTIVE;
				button_draw (button);
			}
		} else if (button->flags & BUTTON_ACTIVE) {
			button->flags &= ~BUTTON_ACTIVE;
			button_draw (button);
		}
	} else if (button->flags & BUTTON_ACTIVE) {
		button->flags &= ~BUTTON_ACTIVE;
		button_draw (button);
        return 1;
	}

        return 0;
}

//清屏
static void refresh_screen ()
{
	int i;

	fillrect (0, 0, xres - 1, yres -1, 0);
	for (i = 0; i < NR_BUTTONS; i++)
		button_draw (&buttons [i]);
}

static void refresh_screen_inloop ()
{
	int i;
	fillrect (0, 50, xres - 1, yres -1, 0);

}

static struct ts_button game_scores[4];
void game_button_init(){
	/* Initialize buttons */

	memset (&game_scores, 0, sizeof (game_scores));
	game_scores[0].w = game_scores [1].w = game_scores [2].w =game_scores [3].w = xres / 4;
	game_scores[0].h = game_scores [1].h = game_scores [2].h =game_scores [3].h =  20;
	game_scores[0].x = 0;
	game_scores[1].x = (4 * xres) / 8;
	game_scores[2].x = (3 * xres) / 4;
	game_scores[3].x = (4 * xres) / 16;

	game_scores[0].y = game_scores [1].y = game_scores [2].y =game_scores[3].y= 50;
	game_scores[0].text = "10 scores";
	game_scores[1].text = "10 scores";
	game_scores[2].text = "20 scores";
	game_scores[3].text = "30 scores";

	for (int i=0;i<4;++i){
		game_scores[i].flags =1;
	}


}
int main()
{
	struct tsdev *ts;
	int x, y;
	unsigned int i;
	unsigned int mode = 0;
	int quit_pressed = 0;
	#define mymod 1
	char *tsdevice=NULL;

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	if( (tsdevice = getenv("TSLIB_TSDEVICE")) != NULL ) {
		ts = ts_open(tsdevice,mymod);
	} else {
		if (!(ts = ts_open("/dev/input/event0", mymod)))
			ts = ts_open("/dev/touchscreen/ucb1x00", mymod);
	}

	if (!ts) {
		perror("ts_open");
		exit(1);
	}

	if (ts_config(ts)) {
		perror("ts_config");
		exit(1);
	}

	if (open_framebuffer()) {
		close_framebuffer();
		exit(1);
	}

	x = xres/2;
	y = yres/2;

	for (i = 0; i < NR_COLORS; i++)
		setcolor (i, palette [i]);

	/* Initialize buttons */
	memset (&buttons, 0, sizeof (buttons));
	buttons [0].w = buttons [1].w = buttons [2].w =buttons [3].w = xres / 4;
	buttons [0].h = buttons [1].h = buttons [2].h =buttons [3].h =  20;
	buttons [0].x = 0;
	buttons [1].x = (4 * xres) / 8;
	buttons [2].x = (3 * xres) / 4;
	buttons [3].x = (4 * xres) / 16;

	buttons [0].y = buttons [1].y = buttons [2].y =buttons [3].y= 10;
	buttons [0].text = "Drag->game";
	buttons [1].text = "Draw";
	buttons [2].text = "Quit";
	buttons [3].text = "game";

	refresh_screen ();
	put_string_center (xres/2, yres/4,   "my TSLIB test program", 1);
	put_string_center (xres/2, yres/4+20,"we offer you a test game for you_designed by 13", 2);
	int sign = 1;
	int x0 = x;
	int y0 = y;
#define speed 1
	int  Cres = speed; //横向改变
	int Cyes = speed;  //纵改变
	int sign_inloop=1;//game lo0p
	int scores = 0;//游戏得分
	game_button_init();

	while (1) {
		struct ts_sample samp;
		int ret;

		/* Show the cross */
		if ((mode & 15) != 1)
			put_cross(x, y, 2 | XORMODE);

		ret = ts_read(ts, &samp, 1);//阻塞方式
		/* Hide it */
		if ((mode & 15) != 1)
			put_cross(x, y, 2 | XORMODE);

		if (ret < 0) {
			perror("ts_read");
			close_framebuffer();
			exit(1);
		}

		if (ret != 1)
			continue;

		for (i = 0; i < NR_BUTTONS; i++)
			if (button_handle (&buttons [i], &samp))
				switch (i) {
				case 0:
					mode = 0;
					refresh_screen ();
					break;
				case 1:
					mode = 1;
					refresh_screen ();
					break;
				case 3:
					mode = 3; //game mode
					//mode = mode | XORMODE;

					refresh_screen();
					
				//	put_board(samp.x, samp.y, 2 | XORMODE);
					break;
				case 2:
					quit_pressed = 1;
				}

		printf("%ld.%06ld: %6d %6d %6d\n", samp.tv.tv_sec, samp.tv.tv_usec,
			samp.x, samp.y, samp.pressure);//
	
		if (samp.pressure > 0) {
			if(mode == 3){

				int cx,cy;
				#if 1
				if(sign){
					cx = x0;
					cy = y0;
					sign =sign -1;
				}
				while(sign_inloop){
					//
					ret = ts_read(ts, &samp, 1);//阻塞方式
					put_board(samp.x, samp.y, 2 | XORMODE);
						//draw
					for (int idx =0; idx<4;++idx){
						//button_draw(&game_scores[idx]);
						my_scores_draw(&game_scores[idx]);
					}
					for (i = 0; i < NR_BUTTONS; i++)
						if (button_handle (&buttons [i], &samp))
							switch (i) {
								case 0:
									mode = 0;
									sign_inloop=0;
									refresh_screen ();
									break;
								case 1:
									mode = 1;
									sign_inloop=0;
									refresh_screen ();
									break;
								case 3:
									mode = 3; //game mode
									break;
								case 2:
									sign_inloop=0;
							}

				circle(cx,cy,10);
				if(cy==yres-60){
					Cyes=-speed;
				}
				
				if(cx==xres){
					Cres = -speed;
					}
				if(cy==60){
					Cyes =speed;
				}
				if (cx ==0){
					Cres =speed;
				}
				int tempx = samp.x;
				int tempy = samp.y;
				if(tempx-50<cx&&cx<tempx+50){
					if(cy+5==tempy){
						Cyes =-speed;
					}
				}
				#if 1
				if(0<cx&&cx<xres/4&&game_scores[0].flags){  //yes
			
					if(cy-5==60){
						game_scores[0].flags=0;
						scores += 10;
						Cyes = speed; 
					}
				}
				if(xres/4<cx&&cx<xres/2&&game_scores[1].flags){
		

					if(cy-5==60){
						game_scores[1].flags=0;
						scores += 30;
						Cyes = speed; 
					}
				}
				if(xres/2<cx&&cx<xres/3&&game_scores[3].flags){
		

					if(cy-5==60){
						game_scores[3].flags=0;
						scores += 10;
						Cyes = speed; 
					}
				}
				if(xres/3<cx&&cx<xres&&game_scores[2].flags){ //yes

					if(cy-5==60){
						game_scores[2].flags=0;
						scores += 20;
						Cyes = speed; 
					}
				}
				#endif
					cx += Cres;
					cy += Cyes;
				//sleep(0.01);
					refresh_screen_inloop();
				}
				#endif
				mode = 3;
			}
			if (mode == 0x80000001){
				line (x, y, samp.x, samp.y, 2 | XORMODE);
			}
				x = samp.x;
				y = samp.y;	
			mode |= 0x80000000; //循环操作

		} else
			mode &= ~0x80000000;
		if (quit_pressed)
			{printf("scores:%d\n",scores);
			break;
			}
	}
	close_framebuffer();

	return 0;
}




