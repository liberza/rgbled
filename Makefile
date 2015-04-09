# After running make, insert module with:
# `insmod rgb.ko`
obj-m += rgb.o

.PHONY: client
.PHONY: test

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

client:
	$(MAKE) -C client

test:
	$(MAKE) -C test

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
