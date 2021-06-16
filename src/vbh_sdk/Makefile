SHELL = /bin/sh

.PHONY: all
all:
	cd $(WORK_ROOT)/src/vbh_sdk/vbh_sdk_comm && $(MAKE)
	cd $(WORK_ROOT)/src/vbh_sdk/plugin       && $(MAKE)
	cd $(WORK_ROOT)/src/vbh_sdk/client_sdk   && $(MAKE)
	cd $(WORK_ROOT)/src/vbh_sdk/client_c_sdk && $(MAKE)
	
clean:
	for subdir in `ls -d */`; do \
		if [ -f $$subdir\Makefile ]; then \
			$(MAKE) clean -C $$subdir; \
		fi \
	done
