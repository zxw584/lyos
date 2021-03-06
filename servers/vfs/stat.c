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
#include <lyos/sysutils.h>
#include "string.h"
#include "lyos/fs.h"
#include "lyos/proc.h"
#include "errno.h"
#include "types.h"
#include "path.h"
#include "proto.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "global.h"
    
/**
 * <Ring 1> Issue the stat request.
 * @param  fs_ep Endpoint of FS driver.
 * @param  fd    File to stat.
 * @param  src   Who want to stat.
 * @param  buf   Buffer.
 * @return       Zero on success.
 */
PUBLIC int request_stat(endpoint_t fs_ep, dev_t dev, ino_t num, int src, char * buf)
{
    MESSAGE m;

    m.type = FS_STAT;
    m.STDEV = dev;
    m.STINO = num;
    m.STSRC = src;
    m.STBUF = buf;

    send_recv(BOTH, fs_ep, &m);

    return m.STRET;
}

/**
 * <Ring 1> Perform the stat syscall.
 * @param  p Ptr to the message.
 * @return   Zero if success.
 */
PUBLIC int do_stat(MESSAGE * p)
{
    endpoint_t src = p->source;
    struct fproc* pcaller = vfs_endpt_proc(src);

    int namelen = p->NAME_LEN + 1;
    char pathname[MAX_PATH];
    if (namelen > MAX_PATH) return ENAMETOOLONG;

    data_copy(SELF, pathname, p->source, p->PATHNAME, namelen);
    pathname[namelen] = 0;

    struct lookup lookup;
    struct inode* pin = NULL;
    struct vfs_mount* vmnt = NULL;
    init_lookup(&lookup, pathname, 0, &vmnt, &pin);
    lookup.vmnt_lock = RWL_READ;
    lookup.inode_lock = RWL_READ;
    pin = resolve_path(&lookup, pcaller);
    if (!pin) return ENOENT;

    int retval = request_stat(pin->i_fs_ep, pin->i_dev, pin->i_num, p->source, p->BUF);

    unlock_inode(pin);
    unlock_vmnt(vmnt);
    put_inode(pin);
    
    return retval;
}

/**
 * <Ring 1> Perform the fstat syscall.
 * @param  p Ptr to the message.
 * @return   Zero if success.
 */
PUBLIC int do_fstat(MESSAGE * p)
{
    endpoint_t src = p->source;
    struct fproc* pcaller = vfs_endpt_proc(src);
    int fd = p->FD;
    char * buf = p->BUF;

    struct file_desc* filp = get_filp(pcaller, fd, RWL_READ);
    if (!filp) return EINVAL;

    /* Issue the request */
    int retval = request_stat(filp->fd_inode->i_fs_ep, 
        filp->fd_inode->i_dev, filp->fd_inode->i_num, src, buf);

    unlock_filp(filp);

    return retval;
}
