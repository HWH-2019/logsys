ifneq ($(KERNELRELEASE),) 
obj-m := mydriver.o 
 
else 
PWD := $(shell pwd) 
KVER := $(shell uname -r) 
KDIR := /usr/src/linux-5.11.1/
all: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules 
clean: 
	rm -rf .*.cmd *.o *.mod.c *.ko .tmp_versions 
endif
