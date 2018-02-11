/*
 * This file is part of MultiROM.
 *
 * MultiROM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MultiROM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MultiROM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DTB_H
#define DTB_H

#include "../multirom.h"

enum dtb_partitions {
    DTB_PARTITION_NONE   = 0,
    DTB_PARTITION_SYSTEM = 1,

    DTB_PARTITION_CNT
};

/**
 * Defines the names of the partitions to be removed from the DTB, if present.
 * The order must be identical to the order in {@link dtb_partitions}.
 */
extern const char *dtb_rm_partitions[];

/**
 * Removes problematic partitions trom the given device tree blob, such as a
 * describtion of the "/system" partition. Internally, the function uses the
 * library libfdt to search the "fstab" node in the tree to remove the partition
 * entry.
 * 
 * @param dtb_name Path the the dtb whose problematic partitions shall be
 *      removed.
 * @return Zero, if a modification has been performed, a positive number if no
 *      modification was needed, a negative number on errors.
 */
int remove_dtb_mounts(const char *dtb_name, struct multirom_status *status);

#endif /* DTB_H */
