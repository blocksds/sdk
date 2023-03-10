/*
  Copyright 2007
  International Business Machines Corporation,
  Sony Computer Entertainment, Incorporated,
  Toshiba Corporation,
  All rights reserved.

  Copyright (c) 2023 Antonio Niño Díaz

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
    * Neither the names of the copyright holders nor the names of their
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#define MAXNAMLEN   255

#define DT_UNKNOWN  0
#define DT_FIFO     1
#define DT_CHR      2
#define DT_DIR      3
#define DT_BLK      4
#define DT_REG      5
#define DT_LNK      6
#define DT_SOCK     7
#define DT_WHT      8

struct dirent {
    ino_t           d_ino;    // Inode number
    off_t           d_off;    // Value that would be returned by telldir()
    unsigned short  d_reclen; // Length of this record
    unsigned char   d_type;   // Type of file; not supported by all filesystems
    char            d_name[MAXNAMLEN + 1]; // Null-terminated filename
};

typedef struct {
    // Private pointer to internal state of the directory.
    void    *dp;
    // Index of the current entry (for telldir() and seekdir()).
    int     index;
    // Allow one readdir for each opendir, and store the data here.
    struct  dirent dirent;
} DIR;

#ifdef __cplusplus
}
#endif

#endif // _SYS_DIRENT_H
