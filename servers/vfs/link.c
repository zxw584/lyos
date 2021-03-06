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

#include <lyos/type.h>
#include <lyos/ipc.h>
#include "sys/types.h"
#include "stdio.h"
#include "unistd.h"
#include "assert.h"
#include "stddef.h"
#include "lyos/const.h"
#include "string.h"
#include "lyos/fs.h"
#include "lyos/proc.h"
#include "errno.h"
#include "types.h"
#include "path.h"
#include "proto.h"
#include "fcntl.h"

PRIVATE int request_ftrunc(endpoint_t fs_ep, dev_t dev, ino_t num, int newsize);

/**
 * Send FTRUNC request to FS driver.
 * @param  fs_ep   Endpoint of the FS driver.
 * @param  dev     The device number.
 * @param  num     The inode number.
 * @param  newsize New size.
 * @return         Zero on success.
 */
PRIVATE int request_ftrunc(endpoint_t fs_ep, dev_t dev, ino_t num, int newsize)
{
    MESSAGE m;

    m.type = FS_FTRUNC;
    m.REQ_DEV = (int)dev;
    m.REQ_NUM = (int)num;
    m.REQ_FILESIZE = newsize;

    //async_sendrec(fs_ep, &m, 0);
    send_recv(BOTH, fs_ep, &m);

    return m.RET_RETVAL;
}

PUBLIC int truncate_node(struct inode * pin, int newsize)
{
    int file_type = pin->i_mode & I_TYPE;
    if (file_type != I_REGULAR) return EINVAL;
    int retval = request_ftrunc(pin->i_fs_ep, pin->i_dev, pin->i_num, newsize);
    if (retval == 0) {
        pin->i_size = newsize;
    }
    return retval;
}
