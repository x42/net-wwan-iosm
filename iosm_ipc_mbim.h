/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2020 Intel Corporation.
 */

#ifndef IOSM_IPC_MBIM_H
#define IOSM_IPC_MBIM_H

/**
 * ipc_mbim_init - Initialize and create a character device for MBIM
 *		   communication.
 * @ipc_imem:	Pointer to iosm_imem structure
 * @name:	Pointer to character device name
 *
 * Returns: 0 on success
 */
struct iosm_sio *ipc_mbim_init(struct iosm_imem *ipc_imem, const char *name);

/**
 * ipc_mbim_deinit - Frees all the memory allocated for the ipc mbim structure.
 * @ipc_mbim:	Pointer to the ipc mbim data-struct
 */
void ipc_mbim_deinit(struct iosm_sio *ipc_mbim);

#endif
