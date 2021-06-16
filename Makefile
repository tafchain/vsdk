SHELL = /bin/sh

.PHONY: all
all:
	cd $(WORK_ROOT)/lib && cp ./linux_CentOs_8.1/3rd/libaxis2c.so ./libaxis2c.so
	cd $(WORK_ROOT)/lib && cp ./linux_CentOs_8.1/3rd/libACE.so ./libACE.so
	cd $(WORK_ROOT)/lib && cp ./linux_CentOs_8.1/dsc/libdsc.so ./libdsc.so
	
#	dos2unix $(WORK_ROOT)/src/3rd/ACE_wrappers/bin/add_rel_link.sh
#	chmod +x $(WORK_ROOT)/src/3rd/ACE_wrappers/bin/add_rel_link.sh
#	cd $(WORK_ROOT)/src/3rd && $(MAKE)
	cd $(WORK_ROOT)/src && $(MAKE) 
	
clean:
#	cd $(WORK_ROOT)/src/3rd && $(MAKE) clean
	cd $(WORK_ROOT)/lib && rm -rf ./libdsc.so
	cd $(WORK_ROOT)/lib && rm -rf ./libaxis2c.so
	cd $(WORK_ROOT)/lib && rm -rf ./libACE.so
	cd $(WORK_ROOT)/src && $(MAKE) clean
	