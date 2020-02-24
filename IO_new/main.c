#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define dev_Name "/dev/myled"

#define LED1  0x01   //1
#define LED2  0x03   //3
#define LED3  0x07   //7
#define LED4  0x0F   //15
#define ON    0xF0
#define OFF   0x00

int main(){
    //控制
    int sign=0;
    int fd;
    fd=open("/dev/myled", O_RDWR);
    if(fd<0){
	    perror("fd open");
        printf("fb_open is error");
        return -1;
    }
        
        char buffer;
        buffer =(LED2 << 4)|(ON >> 4); //led2亮
        write(fd,&buffer,sizeof(buffer));
        

    sleep(2);
    close(fd);
}
