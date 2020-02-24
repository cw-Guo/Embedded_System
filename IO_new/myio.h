#ifndef __MYIO__H_
#define __MYIO__H_

#include<linux/kernel.h>
#include<linux/delay.h>
#include<linux/io.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#include<linux/module.h>
// io 设备的物理地址已知。
// 地址映射到内核虚拟地址空间
//通过虚拟地址来访问这些设备


//有四个LED 可控制
// 来自GPIO1 模块 21-24 pin
// 通过 一下的方式控制 usr0 LED(GPIO1_21);

//以下地址 均为 开始地址 
// 长度为 4KB
// 000-FFF
// 12位

#define GPIO1_BASE 0x4804C000
#define GPIO2_BASE 0x481Ac000
//备用
#define GPIO0_BASE 0x44E07000
#define GPIO3_BASE 0x48A4E000

#define GPIO1_OE    (GPIO1_BASE+0x134)
#define GPIO1_IN    (GPIO1_BASE+0x138)
#define GPIO1_OUT   (GPIO1_BASE+0x13C) 

#if 0
void* ioremap(unsigned long phys_addr,
              unsigned long size,
              unsigned long flags);
void iounmap(void* addr);
#endif

#if 0
一个定义为volatile的变量是说这变量可能会被意想不到地改变，
这样，编译器就不会去假设这个变量的值了。
精确地说就是，优化器在用到这个变量时必须每次都小心地重新读取这个变量的值，
而不是使用保存在寄存器里的备份。(高速缓存)

下面是volatile变量的几个例子：
    1). 并行设备的硬件寄存器（如：状态寄存器）
    2). 一个中断服务子程序中会访问到的非自动变量(Non-automatic variables)
    3). 多线程应用中被几个任务共享的变量
//https://www.cnblogs.com/Neddy/archive/2012/02/02/2335343.html
#endif

//file operation 
//https://blog.csdn.net/Sophisticated_/article/details/82998655

//file结构与用户空间的FILE没有任何关联。
//file结构代表一个打开的文件，由内核在open时创建，
//并传递给在该文件上进行操作的所有函数。
int LedOpen(struct inode *_inode, struct file *_file);
int LedRelease(struct inode *_inode, struct file *_file);
                                    //读到buffer中  //读多少字节    //起始位置
static ssize_t LedRead(struct file *_file, char *_buffer, size_t _count, loff_t *f_Pos);
                                    //写到buffer中  //写多少字节    //写入位置
static ssize_t LedWrite(struct file *_file, const char *_buffer, size_t _count, loff_t *f_Pos);
int init_module(void);
void cleanup_module(void);

//内核用inode结构在内部表示文件，
//它和file不同，file表示打开的文件描述符，
//对于单个文件，可能会有许多表示打开的文件描述符的file结构，
//但它们都指向单个inode结构。
struct file_operations myled_fop =
{
    .open = LedOpen,
    .release = LedRelease,
    .read = LedRead,
    .write = LedWrite,
};


typedef struct  GPIO_m{
    volatile int *pConf;
    volatile int *pDataIn;
    volatile int *pDataOut;
}gpio_t;

#endif //__MYIO__H_