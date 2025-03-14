obj-m += producer_consumer.o
 
all:
	make -C /usr/src/linux M=$(PWD) modules 
clean:
	make -C /usr/src/linux M=$(PWD) clean 