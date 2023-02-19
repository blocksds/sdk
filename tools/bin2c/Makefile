# SPDX-License-Identifier: MIT
#
# Copyright (c) 2023, Antonio Niño Díaz

bin2c: bin2c.c
	gcc -Wall -Wextra -std=gnu11 -Wformat-truncation=0 -O3 -o $@ $<

.PHONY: clean

clean:
	rm -rf bin2c
