#include "myio.h"

#define dev_Name "myled" 
#define Major_NUM 0 
#define DEBUG 1
#define TEST  1


static int major;

#define LED1  0x01   //1
#define LED2  0x03   //3
#define LED3  0x07   //7
#define LED4  0x0F   //15
#define ON    0xF0
#define OFF   0x00
#define MAX   10
#define CON_ALL ~(1<<22|1<<23|1<<24)  //控制三个灯
#define CLOSE_ALL ~(1<<22|1<<23|1<<24)  //控制三个灯灭

int init_module(void){
    if(!(major = register_chrdev(0, "myled", &myled_fop)))
	{
		printk("unable to get device number!\n");
		return -1;
	}
    #if DEBUG
    gpio_t gpio_manger;

    gpio_manger.pConf =ioremap(GPIO1_OE,4);
    gpio_manger.pDataIn =ioremap(GPIO1_IN,4);
    gpio_manger.pDataOut =ioremap(GPIO1_OUT,4);
    #if TEST
    *(gpio_manger.pConf)  &= ~(1<<22); //控制第二个灯
    *(gpio_manger.pDataOut) |= (1<<22); //2号灯亮
    
    #else 

    unsigned int temp1,temp2;
    temp1 = ~(1<<22);
    temp2 = (1<<22);
    writel(temp1,gpio_manger->pConf);
    writel(temp2,gpio_manger->pDataOut);
    #endif

#endif 

  
}

void cleanup_module(void){
    unregister_chrdev(major, "myled");
	printk("myled module removed\n");
}

int LedOpen(struct inode *_inode, struct file *_file){
    gpio_t *gpio_manger;
	_file->private_data = kmalloc(sizeof(gpio_t), GFP_KERNEL);
    gpio_manger =(gpio_t *)(_file->private_data);

    gpio_manger->pConf =ioremap(GPIO1_OE,4);
    gpio_manger->pDataIn =ioremap(GPIO1_IN ,4);
    gpio_manger->pDataOut =ioremap(GPIO1_OUT,4);
    printk("LEDOpen succeeded !\n");
    return 0;
}

int LedRelease(struct inode *_inode, struct file *_file){
    gpio_t *gpio_manger =(gpio_t *)(_file->private_data);

    *(gpio_manger->pConf)  &= CON_ALL; //控制第2个灯
    *(gpio_manger->pDataOut) &= CLOSE_ALL; //4号灯灭
    iounmap(gpio_manger->pConf);
    iounmap(gpio_manger->pDataIn);
    iounmap(gpio_manger->pDataOut);
    kfree(gpio_manger);
    printk("LEDRelease succeeded !\n");
    return 0;

}
                                    //读到buffer中  //读多少字节    //起始位置
static ssize_t LedRead(struct file *_file, char *_buffer, size_t _count, loff_t *f_Pos){
    
    #if 0
    gpio_t *gpio_manger =(gpio_t *)(_file->private_data);
    int temp;
    temp =(gpio_manger->pDataOut);
    temp &=~((1<<21)| (1<<22)| (1<<23)| (1<<24)); //读取四个灯的状态
    copy_to_user(_buffer,&temp,_count);
    #endif 
    return 0;
}
                                    //用户空间写buffer  //写多少字节    //写入位置
static ssize_t LedWrite(struct file *_file, const char *_buffer, size_t _count, loff_t *f_Pos){

    //char temp[MAX]={0,0,0,0,0,0,0,0,0,0};
    char temp[MAX];
    int sign=0;
    sign = copy_from_user(temp,_buffer,1);
    
    gpio_t *gpio_manger =(gpio_t *)(_file->private_data);
    if(sign){
        printk("Write error!");
        //perror("copy from user error!");
        return -EFAULT;
    }

    int i=0;
    printk("----------------------\n");
    printk("%x\n",temp[0]);
    char flag;
    char status;
    
    while(temp[i])
    {
        flag =temp[i];
        flag = (flag >> 4);
        status = temp[i];
        status =(status<<4);
        printk("%x\n",flag);
        printk("%x\n",status);

        switch(flag){
            case LED1:
                *(gpio_manger->pConf)  &= ~(1<<21); //控制第1个灯
                if(status==ON){
                    *(gpio_manger->pDataOut) |= (1<<21); //1号灯亮
                }
                else{
                    *(gpio_manger->pDataOut) &= ~(1<<21); //1号灯灭
                }
                break;
            case LED2:
                *(gpio_manger->pConf)  &= ~(1<<22); //控制第2个灯
                if(status==ON){
                    *(gpio_manger->pDataOut) |= (1<<22); //2号灯亮
                }
                else{
                    *(gpio_manger->pDataOut) &= ~(1<<22); //2号灯灭
                }
                break;
            case LED3:

                *(gpio_manger->pConf)  &= ~(1<<23); //控制第3个灯
                if(status==ON){
                    *(gpio_manger->pDataOut) |= (1<<23); //3号灯亮
                }
                else{
                    *(gpio_manger->pDataOut) &= ~(1<<23); //3号灯灭
                }

                break;
            case LED4:
                *(gpio_manger->pConf)  &= ~(1<<24); //控制第4个灯
                if(status==ON){
                    *(gpio_manger->pDataOut) |= (1<<24); //4号灯亮
                }
                else{
                    *(gpio_manger->pDataOut) &= ~(1<<24); //4号灯灭
                }
                break;
            default:
                break;
        }
        ++i;
    }
    printk("write succeed!\n");
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LED DRIVER");
MODULE_AUTHOR("171180616");
