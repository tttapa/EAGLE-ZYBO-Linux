obj-m+=crypto-lkm.o

all:
	make -C /home/develop/linux-4.4.1 ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf- 'CFLAGS=-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard -ftree-vectorize -mvectorize-with-neon-quad' M=$(PWD) modules
clean:
	make -C /home/develop/linux-4.4.1 M=$(PWD) clean