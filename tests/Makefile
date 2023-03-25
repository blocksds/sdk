# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

.PHONY: all clean

MAKE	:= make

all:
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			$(MAKE) -C $$i --no-print-directory || { exit 1;} \
		fi; \
	done;
	@rm -fr build-all
	@mkdir -p build-all
	@find . -name "*.nds" -not -path build-all -exec cp -fv {} build-all \;

clean:
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			$(MAKE) -C $$i clean --no-print-directory || { exit 1;} \
		fi; \
	done;
	@rm -fr build-all
