#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mypaint.h"
typedef unsigned char u8;//1字节
typedef unsigned short u16;//2字节
typedef unsigned int u32; //4字节
#pragma pack (1)
struct Bmp_Header{//14字节
    u16 type;//用于标识BMP和DIB文件的魔数，一般为0x42 0x4D，即ASCII的BM +2
    u32 size;//BMP文件的大小（单位为字节）                           +4
    u16 Reserved_1;//保留；实际值因创建程序而异                       +2
    u16 Reserved_2;//保留；实际值因创建程序而异                       +2
    u32 offset;//位图数据（像素数组）的地址偏移，也就是起始地址。        +4
};


struct DIB_Header{ //该版本为40字节
    u32 _size;//该头结构的大小（40字节）
    int width;//位图宽度，单位为像素（有符号整数）
    int height;//位图高度，单位为像素（有符号整数）
    u16 Num_of_color_planes;//色彩平面数；只有1为有效值
    u16 colour_bit;//每个像素所占位数，即图像的色深。典型值为1、4、8、16、24和32
    u32 Com_type;//所使用的压缩方法
    u32 size_of_p;//图像大小。指原始位图数据的大小（详见后文），与文件大小不是同一个概念。
    int PixelPRow;//图像的横向分辨率，单位为像素每米（有符号整数）
    int PixelPCol;//图像的纵向分辨率，单位为像素每米（有符号整数）
    u32 NOColor;//调色板的颜色数，为0时表示颜色数为默认的2色深个
    u32 NOfkeyColor; //重要颜色数，为0时表示所有颜色都是重要的；通常不使用本项
};

struct Color{//24位b
   unsigned char b;
   unsigned char g;
   unsigned char r;
};


void fillBmp_header(FILE *file, struct Bmp_Header * Bmp_Header_ptr)
{   
    int index;

   // index = fseek(file, 11, SEEK_SET);
    index =fread(Bmp_Header_ptr, 1, 14, file);
    if(index != 14){
        printf("fread in fillbmp Header failded!\n");
    }
}
void fillDIB_Header(FILE *file, struct DIB_Header *DIB_Header_ptr){
    int index;

    index = fseek(file, 14, SEEK_SET);
    
    if (-1 == index)
	{
		printf("fseek in fillDIB failed!\n");
	}

    index =fread(DIB_Header_ptr, 1, 40, file);
    if(index != 40){
        printf("fread in fillbmp Header failded!\n");
    }

}

// to be finished 
void myshow(FILE *file,unsigned char *fbp, struct DIB_Header *DIB_Header_ptr, struct Bmp_Header * Bmp_Header_ptr){
    int w,h;
    struct Color col; //24位色
    unsigned int tmp_c;
    w=DIB_Header_ptr->width;
    h=DIB_Header_ptr->height;
    int Pos=Bmp_Header_ptr->offset;

    printf("POs:%d\n",Pos);

    printf("w:%d\n",w);
    printf("h:%d\n",h);
    int check=0;
    check = fseek(file, Pos, SEEK_SET); //指针偏移到数据段
    if(check==-1){
        printf("fseek in myshow failed!\n");
    }

//while(!feof(file)){
    h=768;
    w=1024;
for(int j=h-1;j>=0;j--){
    for(int i=0;i<w;++i){
                col.r=0;
                col.g=0;
                col.b=0;
                #if 1
                check=fread(&col,1,3,file); //16位颜色 总大小为3个字节
                        //plot
                if (check != 3) {
		            printf("fread in READ_for failed!\n");
                }
                #endif
                draw_pixel(fbp,i,j,col.r,col.g,col.b);//
                Pos=Pos+3;
                fseek(file, Pos, SEEK_SET); //指针偏移到数据段
            }

            Pos=Pos+768;
    }
   // }
}



int main(){
    FILE *_file;
    _file = fopen("test.bmp","rb");
    struct Bmp_Header  Bmp_H;
    struct Bmp_Header *ptr_Bmp_H =&Bmp_H;

    fillBmp_header(_file,ptr_Bmp_H);

    struct DIB_Header  DIB_H;
    struct DIB_Header *ptr_DIB_H =&DIB_H;

    fillDIB_Header(_file,ptr_DIB_H);
    printf("DEBUG------------\n");
    printf("该bmp文件的大小:%d \n",Bmp_H.size);
    printf("该bmp文件的类型:%d \n",Bmp_H.type);

    printf("该bmp文件数据区的偏移:%d \n",Bmp_H.offset);

    printf("该bmp文件色深:%d \n",DIB_H.colour_bit);
    printf("该文件的分辨率%d x %d \n",DIB_H.width,DIB_H.height);

    printf("该bmp文件压缩方法:%d \n",DIB_H.Com_type);
    printf("该bmp文件调色板颜色:%d \n",DIB_H.NOColor);
    printf("该bmp文件 PixelPRow:%d \n",DIB_H.PixelPRow);
    printf("该bmp文件 PixelPCol:%d \n",DIB_H.PixelPCol);


    unsigned char *fbp;
	unsigned long screensize; 
    fbp = fb_open();
	screensize = get_size();    
    myshow(_file,fbp,ptr_DIB_H,ptr_Bmp_H);
    
    sleep(10);
	
    memset(fbp,0,screensize);
    munmap(fbp,screensize);
    return 0;
}