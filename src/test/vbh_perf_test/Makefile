SHELL = /bin/sh

.PHONY: all
all:
	for subdir in `ls -d */`; do \
		if [ -f $$subdir\Makefile ]; then \
			$(MAKE) -C $$subdir; \
		fi \
	done

clean:
	for subdir in `ls -d */`; do \
		if [ -f $$subdir\Makefile ]; then \
			$(MAKE) clean -C $$subdir; \
		fi \
	done
