# After running make, insert module with:
# `insmod rgb.ko`
obj-m += rgb.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
