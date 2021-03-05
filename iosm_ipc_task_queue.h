/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2020 Intel Corporation.
 */

#ifndef IOSM_IPC_TASK_QUEUE_H
#define IOSM_IPC_TASK_QUEUE_H

#include <linux/interrupt.h>

#include "iosm_ipc_imem.h"

/**
 * ipc_task_queue_init - Allocate a tasklet
 * @ipc_tasklet:	Pointer to tasklet_struct
 * @dev:		Pointer to device structure
 *
 * Returns: Pointer to allocated ipc_task data-struct or NULL on failure.
 */
struct ipc_task_queue *ipc_task_queue_init(struct tasklet_struct *ipc_tasklet,
					   struct device *dev);

/**
 * ipc_task_queue_deinit - Free a tasklet, invalidating its pointer.
 * @ipc_task:	Pointer to ipc_task instance
 */
void ipc_task_queue_deinit(struct ipc_task_queue *ipc_task);

/**
 * ipc_task_queue_send_task - Synchronously/Asynchronously call a function in
 *			      tasklet context.
 * @imem:		Pointer to iosm_imem struct
 * @func:		Function to be called in tasklet context
 * @arg:		Integer argument for func
 * @msg:		Message pointer argument for func
 * @size:		Size argument for func
 * @wait:		if true wait for result
 *
 * Returns: Result value returned by func or -1 if func could not be called.
 */
int ipc_task_queue_send_task(struct iosm_imem *imem,
			     int (*func)(struct iosm_imem *ipc_imem, int arg,
					 void *msg, size_t size),
			     int arg, void *msg, size_t size, bool wait);

#endif
