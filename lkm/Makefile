obj-m+=crypto-lkm.o

all:
	make -C /home/develop/linux-xlnx ARCH=arm CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- M=$(PWD) modules
clean:
	make -C /home/develop/linux-xlnx M=$(PWD) clean