
DATE = 190315
SUBDIR = jiahua
obj-m += $(FILE).o
KDIR = /work/linux-3.5/
#3
all:
	@make -C $(KDIR) M=$(PWD) modules
#1
cp:	
	@cp /mnt/hgfs/linuxDriver/lesson/$(DATE)/$(SUBDIR)/$(FILE).c ./
#2
chmod: 
	@chmod 666 $(FILE).c
#4
clean:
	@rm .@rm  .*.cmd  *.mod.*  *.o  *.ord*  *.sym*  .tmp*  -rf


