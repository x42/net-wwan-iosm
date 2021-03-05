/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2020 Intel Corporation.
 */

#ifndef IOSM_IPC_WWAN_H
#define IOSM_IPC_WWAN_H

#define IMEM_WWAN_DATA_VLAN_ID_START 1
#define IMEM_WWAN_CTRL_VLAN_ID_START 257
#define IMEM_WWAN_CTRL_VLAN_ID_END 512

/**
 * ipc_wwan_init - Allocate, Init and register WWAN device
 * @ops_instance:	Instance pointer for callback
 * @dev:		Pointer to device structure
 * @max_sessions:	Maximum number of sessions
 *
 * Returns: Pointer to instance on success else NULL
 */
struct iosm_wwan *ipc_wwan_init(void *ops_instance, struct device *dev,
				int max_sessions);

/**
 * ipc_wwan_deinit - Unregister and free WWAN device, clear pointer
 * @ipc_wwan:	Pointer to wwan instance data
 */
void ipc_wwan_deinit(struct iosm_wwan *ipc_wwan);

/**
 * ipc_wwan_receive - Receive a downlink packet from CP.
 * @ipc_wwan:	Pointer to wwan instance
 * @skb_arg:	Pointer to struct sk_buff
 * @dss:	Set to true if vlan id is greater than
 *		IMEM_WWAN_CTRL_VLAN_ID_START else false
 *
 * Return: 0 on success else -EINVAL or -1
 */
int ipc_wwan_receive(struct iosm_wwan *ipc_wwan, struct sk_buff *skb_arg,
		     bool dss);

/**
 * ipc_wwan_update_stats - Update device statistics
 * @ipc_wwan:	Pointer to wwan instance
 * @id:		Ipc mux channel session id
 * @len:	Number of bytes to update
 * @tx:		True if statistics needs to be updated for transmit
 *		else false
 *
 */
void ipc_wwan_update_stats(struct iosm_wwan *ipc_wwan, int id, size_t len,
			   bool tx);

/**
 * ipc_wwan_tx_flowctrl - Enable/Disable TX flow control
 * @ipc_wwan:	Pointer to wwan instance
 * @id:		Ipc mux channel session id
 * @on:		if true then flow ctrl would be enabled else disable
 *
 */
void ipc_wwan_tx_flowctrl(struct iosm_wwan *ipc_wwan, int id, bool on);

/**
 * ipc_wwan_is_tx_stopped - Checks if Tx stopped for a VLAN id.
 * @ipc_wwan:	Pointer to wwan instance
 * @id:		Ipc mux channel session id
 *
 * Return: true if stopped, false otherwise
 */
bool ipc_wwan_is_tx_stopped(struct iosm_wwan *ipc_wwan, int id);

#endif
