/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2020 Intel Corporation.
 */

#ifndef IOSM_IPC_SIO_H
#define IOSM_IPC_SIO_H

#include <linux/miscdevice.h>
#include <linux/skbuff.h>

#include "iosm_ipc_imem_ops.h"

/* IPC char. device default mode. Only privileged user can access. */
#define IPC_CHAR_DEVICE_DEFAULT_MODE 0600

#define IS_OPEN 0
#define IS_BLOCKING 1
#define WRITE_IN_USE 2
#define IS_DEINIT 3

/**
 * struct iosm_sio_open_file - Reference to struct iosm_sio
 * @sio_dev:	iosm_sio instance
 */
struct iosm_sio_open_file {
	struct iosm_sio *sio_dev;
};

/**
 * struct iosm_sio - State of the char driver layer.
 * @misc:		OS misc device component
 * @sio_fop:		reference to iosm_sio structure
 * @ipc_imem:		imem instance
 * @dev:		Pointer to device struct
 * @pcie:		PCIe component
 * @rx_pending_buf:	Storage for skb when its data has not been fully read
 * @misc:		OS misc device component
 * @devname:		Device name
 * @channel:		Channel instance
 * @rx_list:		Downlink skbuf list received from CP.
 * @read_sem:		Needed for the blocking read or downlink transfer
 * @poll_inq:		Read queues to support the poll system call
 * @flag:		Flags to monitor state of device
 * @wmaxcommand:	Max buffer size
 */
struct iosm_sio {
	struct miscdevice misc;
	struct iosm_sio_open_file *sio_fop;
	struct iosm_imem *ipc_imem;
	struct device *dev;
	struct iosm_pcie *pcie;
	struct sk_buff *rx_pending_buf;
	char devname[IPC_SIO_DEVNAME_LEN];
	struct ipc_mem_channel *channel;
	struct sk_buff_head rx_list;
	struct completion read_sem;
	wait_queue_head_t poll_inq;
	unsigned long flag;
	u16 wmaxcommand;
};

/**
 * ipc_sio_init - Allocate and create a character device.
 * @ipc_imem:	Pointer to iosm_imem structure
 * @name:	Pointer to character device name
 *
 * Returns: Pointer to sio instance on success and NULL on failure
 */
struct iosm_sio *ipc_sio_init(struct iosm_imem *ipc_imem, const char *name);

/**
 * ipc_sio_deinit - Dellocate and free resource for a character device.
 * @ipc_sio:	Pointer to the ipc sio data-struct
 */
void ipc_sio_deinit(struct iosm_sio *ipc_sio);

#endif
