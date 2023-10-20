# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

.PHONY: all clean

MAKE	:= make

all:
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			$(MAKE) -C $$i --no-print-directory || { exit 1;} \
		fi; \
	done;

clean:
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			$(MAKE) -C $$i clean --no-print-directory || { exit 1;} \
		fi; \
	done;
