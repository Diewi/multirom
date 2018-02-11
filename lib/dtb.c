/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <libfdt.h>

#include "dtb.h"
#include "log.h"

const char *dtb_rm_partitions[] = { "system" };

int remove_dtb_mounts(const char *dtb_name, struct multirom_status *status)
{
    int res = 0;
    int dtb_fstab_node_offset = 0;
    int dtb_part_node_offset = 0;
    int dtb_mod_partitions = 0;

    char *dtb_buf = (char *)utilfdt_read(dtb_name);
    if(dtb_buf == NULL)
    {
        ERROR("Could not read provided DTB.\n");
        res = -1;
        goto ret_fail;
    }

    dtb_fstab_node_offset = fdt_node_offset_by_compatible(dtb_buf, 0,
            "android,fstab");
    res = dtb_fstab_node_offset;
    if(res == -FDT_ERR_NOTFOUND)
    {
        INFO("No fstab entry found in DTB. No modifications needed.\n");
        goto ret_no_mod;
    }
    else if(res < 0)
    {
        ERROR("Error getting fstab node from DTB. Errno: %d",
                dtb_fstab_node_offset);
        goto ret_fail;
    }
    INFO("DTB fstab node found at offset %x.\n", dtb_fstab_node_offset);

    for (int i = DTB_PARTITION_NONE; i < DTB_PARTITION_CNT - 1; ++i)
    {
        INFO("Searching for partition %s in DTB", dtb_rm_partitions[i]);
        dtb_part_node_offset = fdt_subnode_offset(dtb_buf,
                dtb_fstab_node_offset, dtb_rm_partitions[i]);
        res = dtb_part_node_offset;
        if (res == FDT_ERR_NOTFOUND)
        {
            INFO("No %s partition entry found in DTB, despite an fstab entry \
                     was found.\n", dtb_rm_partitions[i]);
            continue;
        }
        else if (res < 0)
        {
            ERROR("Error getting %s partition from DTB. Errno: %d",
                    dtb_rm_partitions[i], dtb_part_node_offset);
            goto ret_fail;
        }
        INFO("DTB %s partition found at offset %x.\n", dtb_rm_partitions[i],
                dtb_part_node_offset);

        res = fdt_del_node(dtb_buf, dtb_part_node_offset);
        if (res != 0)
        {
            ERROR("Failed to delete %s partition entry from DTB. Errno: %d.\n",
                    dtb_rm_partitions[i], res);
            goto ret_fail;
        }
        // The index (i + 1) is equivalent to the enums in dtb_rm_partitions, so
        // use it to mark the actually removed partitions from the DTB.
        dtb_mod_partitions |= (1 << (i + 1));
    }

    if (dtb_mod_partitions == 0)
    {
        goto ret_no_mod;
    }

    res = utilfdt_write_err(dtb_name, dtb_buf);
    if (res != 0)
    {
        ERROR("Could not write DTB image to %s. Errno: %d.\n", dtb_name, res);
        goto ret_fail;
    }

    status->dtb_removed_partitions = dtb_mod_partitions;
    return 0;

ret_no_mod:
    status->dtb_removed_partitions = (1 << DTB_PARTITION_NONE);
    return 0;

ret_fail:
    return res;
}
