# apt-get install linux-headers-$(uname -r)

KVER := $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	-sudo /sbin/rmmod iosm
	sudo /sbin/insmod iosm.ko

unload:
	sudo /sbin/rmmod iosm

iosm-y = \
	iosm_ipc_task_queue.o	\
	iosm_ipc_imem.o			\
	iosm_ipc_imem_ops.o		\
	iosm_ipc_mmio.o			\
	iosm_ipc_sio.o			\
	iosm_ipc_mbim.o			\
	iosm_ipc_wwan.o			\
	iosm_ipc_uevent.o		\
	iosm_ipc_pm.o			\
	iosm_ipc_pcie.o			\
	iosm_ipc_irq.o			\
	iosm_ipc_chnl_cfg.o		\
	iosm_ipc_protocol.o		\
	iosm_ipc_protocol_ops.o	\
	iosm_ipc_mux.o			\
	iosm_ipc_mux_codec.o

obj-m := iosm.o

# compilation flags
ccflags-y += -DDEBUG
