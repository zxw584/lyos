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
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <lyos/fs.h>
#include <lyos/const.h>
#include <sys/dirent.h>
#include "libmemfs/libmemfs.h"

PRIVATE int stat_type(struct memfs_stat* st)
{
    switch (st->st_mode) {
    case I_REGULAR:
        return DT_REG;
    case I_DIRECTORY:
        return DT_DIR;
    default:
        return DT_UNKNOWN;
    }
}

PUBLIC int memfs_getdents(dev_t dev, ino_t num, struct fsdriver_data * data, u64 * ppos, size_t * count)
{
#define GETDENTS_BUFSIZE 4096
    static char getdents_buf[GETDENTS_BUFSIZE];

    int retval = 0;
    //int done = 0;
    off_t pos = *ppos, i = 0;

    struct memfs_inode* pin = memfs_find_inode(num);
    if (!pin) return EINVAL;

    struct fsdriver_dentry_list list;
    fsdriver_dentry_list_init(&list, data, *count, getdents_buf, sizeof(getdents_buf));

    struct memfs_inode* node;
    list_for_each_entry(node, &pin->i_children, i_list) {
        if (i >= pos) {
            int retval = fsdriver_dentry_list_add(&list, node->i_num, node->i_name,
                strlen(node->i_name) + 1, stat_type(&node->i_stat));

            if (retval < 0) return retval;
            if (retval == 0) break;
        }
        i++;
    }

    ssize_t bytes = fsdriver_dentry_list_finish(&list);
    if (bytes < 0) retval = -bytes;
    else *count = bytes;

    *ppos = i;
    
    return retval;
}
