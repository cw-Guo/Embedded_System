obj-m = myio.o

KDIR =/home/171180616/linux
PWD =$(shell pwd)
COMPILE=/opt/armhf-linux-2018.08/bin/arm-none-linux-gnueabi-

ARCH_TYPE=arm

all:
		$(MAKE) CROSS_COMPILE=$(COMPILE) ARCH=$(ARCH_TYPE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
		$(MAKE) CROSS_COMPILE=$(COMPILE) ARCH=$(ARCH_TYPE) -C $(KDIR) SUBDIRS=$(PWD) clean
