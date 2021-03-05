/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2020 Intel Corporation
 */

#ifndef IOSM_IPC_CHNL_CFG_H
#define IOSM_IPC_CHNL_CFG_H

#include "iosm_ipc_mux.h"

/* Number of TDs on the trace channel */
#define IPC_MEM_TDS_TRC 32

/* Trace channel TD buffer size. */
#define IPC_MEM_MAX_DL_TRC_BUF_SIZE 8192

/* Type of the WWAN ID */
enum ipc_wwan_id {
	IPC_WWAN_DSS_ID_0 = 257,
	IPC_WWAN_DSS_ID_1,
	IPC_WWAN_DSS_ID_2,
	IPC_WWAN_DSS_ID_3,
	IPC_WWAN_DSS_ID_4,
};

/**
 * struct ipc_chnl_cfg - IPC channel configuration structure
 * @id:				VLAN ID
 * @ul_pipe:			Uplink datastream
 * @dl_pipe:			Downlink datastream
 * @ul_nr_of_entries:		Number of Transfer descriptor uplink pipe
 * @dl_nr_of_entries:		Number of Transfer descriptor downlink pipe
 * @dl_buf_size:		Downlink buffer size
 * @accumulation_backoff:	Time in usec for data accumalation
 */
struct ipc_chnl_cfg {
	int id;
	u32 ul_pipe;
	u32 dl_pipe;
	u32 ul_nr_of_entries;
	u32 dl_nr_of_entries;
	u32 dl_buf_size;
	u32 accumulation_backoff;
};

/**
 * ipc_chnl_cfg_get - Get pipe configuration.
 * @chnl_cfg:		Array of ipc_chnl_cfg struct
 * @index:		Channel index (upto MAX_CHANNELS)
 *
 * Return: 0 on success and -1 on failure
 */
int ipc_chnl_cfg_get(struct ipc_chnl_cfg *chnl_cfg, int index);

#endif
