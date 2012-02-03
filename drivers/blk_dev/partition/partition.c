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
    
#include "sys/types.h"
#include "lyos/type.h"
#include "stdio.h"
#include "assert.h"
#include "lyos/const.h"
#include "lyos/protect.h"
#include "string.h"
#include "lyos/fs.h"
#include "lyos/proc.h"
#include "lyos/tty.h"
#include "lyos/console.h"
#include "lyos/global.h"
#include "lyos/proto.h"
#include "lyos/hd.h"
#include "lyos/driver.h"

/*****************************************************************************
 *                                get_part_table
 *****************************************************************************/
/**
 * <Ring 1> Get a partition table of a device.
 * 
 * @param drive   Device nr.
 * @param sect_nr The sector at which the partition table is located.
 * @param entry   Ptr to part_ent struct.
 *****************************************************************************/
PUBLIC void get_part_table(int device, int sect_nr, struct part_ent * entry)
{
        char * buf = malloc(SECTOR_SIZE);
        
        rw_sector(DEV_READ, device, (sect_nr) * SECTOR_SIZE, SECTOR_SIZE, 
                            getpid(), buf);
        memcpy(entry, buf + PARTITION_TABLE_OFFSET, 
                      sizeof(struct part_ent) * NR_PART_PER_DRIVE);
        
        /* free(buf); */
}
        
        