/*  This file is part of Lyos.

    Lyos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Lyos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lyos.  If not, see <http://www.gnu.org/licenses/>. */

#include "lyos/type.h"
#include "sys/types.h"
#include "lyos/config.h"
#include "errno.h"
#include "stdio.h"
#include "stddef.h"
#include "unistd.h"
#include "assert.h"
#include "lyos/const.h"
#include "string.h"
#include "lyos/fs.h"
#include "lyos/proc.h"
#include "lyos/tty.h"
#include "lyos/console.h"
#include "lyos/global.h"
#include "lyos/proto.h"
#include "lyos/hd.h"
#include "lyos/list.h"
#include "proto.h"
#include "global.h"
#include "tar.h"

PUBLIC int initfs_rdwt(MESSAGE * p)
{
    dev_t dev = (int)p->RWDEV;
    ino_t num = p->RWINO;
    u64 position = p->RWPOS;
    int src = p->RWSRC;
    int rw_flag = p->RWFLAG;
    void * buf = p->RWBUF;
    int nbytes = p->RWCNT;

    char header[512];
    initfs_rw_dev(DEV_READ, dev, initfs_headers[num], 512, header);
    struct posix_tar_header * phdr = (struct posix_tar_header *)header;

    if (phdr->size < nbytes + position) nbytes = phdr->size - position;

    rw_sector((rw_flag == READ) ? DEV_READ : DEV_WRITE, dev, initfs_headers[num] + 512 + position, nbytes, src, buf);
    
    p->RWPOS = position + nbytes;
    p->RWCNT = nbytes;

    return 0;
}