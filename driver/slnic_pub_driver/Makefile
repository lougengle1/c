EXTRA_CFLAGS += -DSHENGLI_LOG
#EXTRA_CFLAGS += -DSHENGLI_LOG_ERROR
#EXTRA_CFLAGS += -DSHENGLI_LOG_WARNING
#EXTRA_CFLAGS += -DSHENGLI_LOG_DEBUG
#EXTRA_CFLAGS += -DSHENGLI_LOG_LACAL
#EXTRA_CFLAGS += -DSHENGLI_LOG_DATA

#EXTRA_CFLAGS += -DSLK_QUEM_EVN
EXTRA_CFLAGS += -DSHENGLI_POLL_MODE
#EXTRA_CFLAGS += -DMINI_KGR_INTERNAL
#EXTRA_CFLAGS += -DSL_RECEIVE_UDP
EXTRA_CFLAGS += $(INCDIR)
#EXTRA_CFLAGS += -DPCI10_USE
#EXTRA_CFLAGS += -DCHERRY_TX_DMA

EXTRA_CFLAGS += -DSHENGLI_DRIVER_VERSION=$(shell cat $(PWD)/version)
EXTRA_CFLAGS += -DSHENGLI_MK_DATE=$(shell date "+%Y%m%d")

#mango drive
EXTRA_CFLAGS += -DSLK_CHERRY

#head file path
SLK_INC_DIR	= -I$(PWD)/slk_tools

#tool file path
SLK_TOOL_DIR = slk_tools/

#publish target path
PUBLISH_TARGET_PATH = ../../bin/linux/debug/

#publish source path
PUBLISH_SOURCE_PATH = ./

ifeq ($(KERNELRELEASE),)
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INCDIR=$(SLK_INC_DIR)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INCDIR=$(SLK_INC_DIR) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INCDIR=$(SLK_INC_DIR) modules_install
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers
	rm -rf $(SLK_TOOL_DIR)src/*.o $(SLK_TOOL_DIR)src/.*.cmd
publish:
	mkdir -p $(PUBLISH_TARGET_PATH)
	cp -rf $(PUBLISH_SOURCE_PATH)load_driver $(PUBLISH_TARGET_PATH)load_driver
	cp -rf $(PUBLISH_SOURCE_PATH)unload_driver $(PUBLISH_TARGET_PATH)unload_driver
	cp -rf $(PUBLISH_SOURCE_PATH)clear_log $(PUBLISH_TARGET_PATH)clear_log

install:
	$(MAKE) INSTALL_MOD_DIR=slnic -C $(KERNELDIR) M=$(PWD) modules_install
	cp test.modules /etc/sysconfig/modules
	chmod u+x /etc/sysconfig/modules/test.modules
	gcc snic-config.c -o snic-config
	cp snic-config /usr/bin
	chmod u+x /usr/bin/snic-config
uninstall:
	 rm -rf /lib/modules/`uname -r`/slnic/sl_nic.ko
	 rm -rf /etc/sysconfig/modules/test.modules
	 rm -rf /usr/bin/snic-config

else
	MODULE_NAME := sl_nic
	RESMAIN_CORE_OBJS := sl_nic_driver.o  control_cdev.o stream_data_cdev.o sl_nic_netdev.o slnic_net_ethtool.o sl_nic_dev.o
	RESMAIN_GLUE_OBJS :=  $(SLK_TOOL_DIR)slk_dev_status.o $(SLK_TOOL_DIR)slk_hw_mem_op.o $(SLK_TOOL_DIR)slk_rx_chan.o  $(SLK_TOOL_DIR)slk_queue.o $(SLK_TOOL_DIR)slk_register.o $(SLK_TOOL_DIR)slk_log.o $(SLK_TOOL_DIR)slk_function.o $(SLK_TOOL_DIR)slk_vl_ring.o $(SLK_TOOL_DIR)slk_gfp.o $(SLK_TOOL_DIR)slk_rx_chan_new.o $(SLK_TOOL_DIR)slk_lib.o 
	$(MODULE_NAME)-objs := $(RESMAIN_GLUE_OBJS) $(RESMAIN_CORE_OBJS)
	obj-m := sl_nic.o
endif  
