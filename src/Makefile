SHELL = /bin/sh

.PHONY: all
all:
	cd $(WORK_ROOT)/src/vbh_comm     && $(MAKE)
	cd $(WORK_ROOT)/src/vbh_sdk      && $(MAKE)
	cd $(WORK_ROOT)/src/test      && $(MAKE)
clean:
	cd $(WORK_ROOT)/src/vbh_comm     && $(MAKE) clean
	cd $(WORK_ROOT)/src/vbh_sdk      && $(MAKE) clean
	cd $(WORK_ROOT)/src/test      && $(MAKE) clean

	