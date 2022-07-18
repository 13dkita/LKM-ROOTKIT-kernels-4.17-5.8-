obj-m := lkm.o
lkm-objs := src/rootkit.o src/set_root.o src/chardevices.o src/hide_lkm.o src/hiding_ports.o src/hide_process.o src/hooking.o src/networking.o

LIB = /home/usr/Desktop/PRACA_0906/include
ccflags-y += -I$(LIB)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
