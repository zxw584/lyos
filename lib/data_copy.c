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
#include <lyos/const.h>
#include <lyos/ipc.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*****************************************************************************
 *                                data_copy
 *****************************************************************************/
PUBLIC int data_copy(endpoint_t dest_pid, int dest_seg, void * dest_addr, 
    endpoint_t src_pid, int src_seg, void * src_addr, int len)
{
	MESSAGE m;

    m.type = DATACOPY;
    
    m.SRC_PID = src_pid;
    m.SRC_SEG = (long)src_seg;
    m.SRC_ADDR = src_addr;

    m.DEST_PID = dest_pid;
    m.DEST_SEG = (long)dest_seg;
    m.DEST_ADDR = dest_addr;

    m.BUF_LEN = len;

    send_recv(BOTH, TASK_MM, &m);

    return m.RETVAL;
}
