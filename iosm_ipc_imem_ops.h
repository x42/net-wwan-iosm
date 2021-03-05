/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2020 Intel Corporation.
 */

#ifndef IOSM_IPC_IMEM_OPS_H
#define IOSM_IPC_IMEM_OPS_H

#include "iosm_ipc_mux_codec.h"

/* Maximum length of the SIO device names */
#define IPC_SIO_DEVNAME_LEN 32

/* Maximum wait time for blocking read */
#define IPC_READ_TIMEOUT 500

/* The delay in ms for defering the unregister */
#define SIO_UNREGISTER_DEFER_DELAY_MS 1

/* Default delay till CP PSI image is running and modem updates the
 * execution stage.
 * unit : milliseconds
 */
#define PSI_START_DEFAULT_TIMEOUT 3000

/* Default time out when closing SIO, till the modem is in
 * running state.
 * unit : milliseconds
 */
#define BOOT_CHECK_DEFAULT_TIMEOUT 400

/**
 * imem_sys_sio_open - Open a sio link to CP.
 * @ipc_imem:	Imem instance.
 *
 * Return: channel instance on success, NULL for failure
 */
struct ipc_mem_channel *imem_sys_sio_open(struct iosm_imem *ipc_imem);

/**
 * imem_sys_mbim_open - Open a mbim link to CP.
 * @ipc_imem:	Imem instance.
 *
 * Return: channel instance on success, NULL for failure
 */
struct ipc_mem_channel *imem_sys_mbim_open(struct iosm_imem *ipc_imem);

/**
 * imem_sys_sio_close - Release a sio link to CP.
 * @ipc_sio:		iosm sio instance.
 */
void imem_sys_sio_close(struct iosm_sio *ipc_sio);

/**
 * imem_sys_sio_read - Copy the rx data to the user space buffer and free the
 *		       skbuf.
 * @ipc_sio:	Pointer to iosm_sio structi.
 * @buf:	Pointer to destination buffer.
 * @size:	Size of destination buffer.
 * @skb:	Pointer to source buffer.
 *
 * Return: Number of bytes read, -EFAULT and -EINVAL for failure
 */
ssize_t imem_sys_sio_read(struct iosm_sio *ipc_sio, unsigned char __user *buf,
			  size_t size, struct sk_buff *skb);

/**
 * imem_sys_sio_write - Route the uplink buffer to CP.
 * @ipc_sio:		iosm_sio instance.
 * @buf:		Pointer to source buffer.
 * @count:		Number of data bytes to write.
 * @blocking_write:	if true wait for UL data completion.
 *
 * Return: Number of bytes read, -EINVAL and -1  for failure
 */
int imem_sys_sio_write(struct iosm_sio *ipc_sio,
		       const unsigned char __user *buf, int count,
		       bool blocking_write);

/**
 * imem_sys_sio_receive - Receive downlink characters from CP, the downlink
 *		skbuf is added at the end of the downlink or rx list.
 * @ipc_sio:    Pointer to ipc char data-struct
 * @skb:        Pointer to sk buffer
 *
 * Returns: 0 on success, -EINVAL on failure
 */
int imem_sys_sio_receive(struct iosm_sio *ipc_sio, struct sk_buff *skb);

/**
 * imem_sys_wwan_open - Open packet data online channel between network layer
 *			and CP.
 * @ipc_imem:		Imem instance.
 * @vlan_id:		VLAN tag of the VLAN device.
 *
 * Return: Channel ID on success, -1 on failure
 */
int imem_sys_wwan_open(struct iosm_imem *ipc_imem, int vlan_id);

/**
 * imem_sys_wwan_close - Close packet data online channel between network layer
 *			 and CP.
 * @ipc_imem:		Imem instance.
 * @vlan_id:		VLAN tag of the VLAN device.
 * @channel_id:		Channel ID to be closed.
 */
void imem_sys_wwan_close(struct iosm_imem *ipc_imem, int vlan_id,
			 int channel_id);

/**
 * imem_sys_wwan_transmit - Function for transfer UL data
 * @ipc_imem:		Imem instance.
 * @vlan_id:		VLAN tag of the VLAN device.
 * @channel_id:		Channel ID used
 * @skb:		Pointer to sk buffer
 *
 * Return: 0 on success, negative value on failure
 */
int imem_sys_wwan_transmit(struct iosm_imem *ipc_imem, int vlan_id,
			   int channel_id, struct sk_buff *skb);
/**
 * wwan_channel_init - Initializes WWAN channels and the channel for MUX.
 * @ipc_imem:		Pointer to iosm_imem struct.
 * @total_sessions:	Total sessions.
 * @mux_type:		Type of mux protocol.
 */
void wwan_channel_init(struct iosm_imem *ipc_imem, int total_sessions,
		       enum ipc_mux_protocol mux_type);
#endif
