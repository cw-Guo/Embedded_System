
//fd = open("/dev/fb0", O_RDWR);

#include "mypaint.h"
struct fb_var_screeninfo vinfo;//设备参数
unsigned long screensize;

unsigned char* fb_open(){

    int fd;
    int ret;

    fd = open("/dev/fb0",O_RDWR);

    if(fd<0){
	perror("fd open");
        printf("fb_open is error");
        return -1;
    }


    //struct fb_var_screeninfo vinfo;
    //memset(&vinfo, 0, sizeof(vinfo)); //缓冲区清零

    ret =ioctl(fd,FBIOGET_VSCREENINFO, &vinfo); //可以获得显示器色味、分辨率等信息。
    if(ret < 0){
	perror("iocol open");
        printf("iocol to get vinfo  is error");
        return -1;
    }

#if 1
    printf("vinfo.xres             =%d \n",vinfo.xres);
    printf("vinfo.yres             =%d \n",vinfo.yres);
    printf("vinfo.bits_per_pixel   =%d \n",vinfo.bits_per_pixel);
    printf("vinfo.red.length       =%d \n",vinfo.red.length);    
    printf("vinfo.red.offset       =%d \n",vinfo.red.offset);
    
    printf("vinfo.green.length       =%d \n",vinfo.green.length);    
    printf("vinfo.green.offset       =%d \n",vinfo.green.offset);
    printf("vinfo.blue.length       =%d \n",vinfo.blue.length);    
    printf("vinfo.blue.offset       =%d \n",vinfo.blue.offset);

    printf("vinfo.blue.msb_right    =%d\n",vinfo.blue.msb_right);
    
#endif
    screensize = (vinfo.xres * vinfo.yres * vinfo.bits_per_pixel) / 8;
    //获取buffer缓冲区的首地址

    unsigned char *fbp = 0;
    fbp = (unsigned char *)mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    return fbp;
}
unsigned long  get_size(){
	return screensize; 
}
void draw_pixel(unsigned char * fbp, int x, int y, char Red, char Green, char Blue){ // 16色RGB 画点函数

    int offset;
    short color;
    offset =(y * vinfo.xres + x) * vinfo.bits_per_pixel /8;
    color = ((Red>>3) <<11) | (((Green>>2) << 5) & 0x07E0) | ((Blue >> 3) & 0x1F);

    *(unsigned char *)(fbp + offset +0) =color & 0xff;
    *(unsigned char *)(fbp + offset +1) =(color >> 8 )& 0xff;
}

void draw_block(unsigned char * fbp, int x0, int y0, int x1, int y1, char Red, char Green, char Blue){ // 画方块

    //
    int tempx, tempy;
    
    // 判断是否在范围内
    if( (x0 >vinfo.xres) || (x1 >vinfo.xres) || (y0 >vinfo.yres) || (y1 >vinfo.yres)){
        printf("input overload warning!!!");
    }

    //sort
    if(x0 > x1){
        tempx = x1;
        x1 = x0;
        x0 = tempx;  
    }

    if(y0 > y1){
        tempy = y1;
        y1 = y0;
        y0 = tempy;  
    }

    for (int i = x0; i < x1; ++i){//复用画点函数
        for (int j = y0; j < y1; ++j){
            draw_pixel(fbp, i, j, Red, Green, Blue);
        }
    }

}

void draw_line(unsigned char * fbp, int x0, int y0, int x1, int y1, char Red, char Green, char Blue){ // 画线

    float k;
    k = (y1 - y0)/(x1 - x0);
    float b;
    b = y0 - k * x0;

    //sort
    int tempx, tempy;
    if(x0 > x1){ //交换确保x0 小
        tempx = x1;
        x1 = x0;
        x0 = tempx;  

        tempy = y0;
        y0 = y1;
        y1 = tempy;
    }


    int j = 0; // y坐标
    for (int i = x0;i < x1; ++i){
            j=k * i + b;
            draw_pixel(fbp, i, j, Red, Green, Blue);
    }

}

void draw_circle(unsigned char * fbp, int x0, int y0, float r, char Red, char Green, char Blue){
    int R = r;
    //范围判断
    if( (x0 + R > vinfo.xres) || (y0 + R >vinfo.yres ) ){
        printf("input overload!!! this screen's size is %d X %d",vinfo.xres,vinfo.yres);
    }
    int theta =0;
    for (theta = 0; theta <360; ++theta){
        int i = x0 + R * cos(theta);
        int j = y0 + R * sin(theta);
        draw_pixel(fbp, i, j, Red, Green, Blue);
    }
}//to be finished 
//-----------------------
// have not been used 

int max(int a, int b){//比较大小的函数   
    int temp= 0;
    if(a <= b){
        temp = b;
    }
    else {
        temp = a; 
    }
    return temp;
}
//-----------------------



