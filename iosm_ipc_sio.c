// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2020 Intel Corporation.
 */

#include <linux/poll.h>
#include <asm/ioctls.h>

#include "iosm_ipc_sio.h"

static struct mutex sio_floc;		/* Mutex Lock for sio read */
static struct mutex sio_floc_wr;	/* Mutex Lock for sio write */

/* Open a shared memory device and initialize the head of the rx skbuf list. */
static int ipc_sio_fop_open(struct inode *inode, struct file *filp)
{
	struct iosm_sio *ipc_sio =
		container_of(filp->private_data, struct iosm_sio, misc);

	struct iosm_sio_open_file *sio_op = kzalloc(sizeof(*sio_op),
						    GFP_KERNEL);
	if (!sio_op)
		return -ENOMEM;

	if (test_and_set_bit(IS_OPEN, &ipc_sio->flag)) {
		kfree(sio_op);
		return -EBUSY;
	}

	ipc_sio->channel = imem_sys_sio_open(ipc_sio->ipc_imem);

	if (!ipc_sio->channel) {
		kfree(sio_op);
		return -EIO;
	}

	mutex_lock(&sio_floc);

	inode->i_private = sio_op;
	ipc_sio->sio_fop = sio_op;
	sio_op->sio_dev = ipc_sio;

	mutex_unlock(&sio_floc);
	return 0;
}

static int ipc_sio_fop_release(struct inode *inode, struct file *filp)
{
	struct iosm_sio_open_file *sio_op = inode->i_private;

	mutex_lock(&sio_floc);

	if (sio_op->sio_dev) {
		clear_bit(IS_OPEN, &sio_op->sio_dev->flag);
		imem_sys_sio_close(sio_op->sio_dev);
		sio_op->sio_dev->sio_fop = NULL;
	}

	kfree(sio_op);

	mutex_unlock(&sio_floc);
	return 0;
}

/* Copy the data from skbuff to the user buffer */
static ssize_t ipc_sio_fop_read(struct file *filp, char __user *buf,
				size_t size, loff_t *l)
{
	struct iosm_sio_open_file *sio_op = filp->f_inode->i_private;
	struct sk_buff *skb = NULL;
	struct iosm_sio *ipc_sio;
	ssize_t read_byt;
	int ret_err;

	if (!buf) {
		ret_err = -EINVAL;
		goto err;
	}

	mutex_lock(&sio_floc);

	if (!sio_op->sio_dev) {
		ret_err = -EIO;
		goto err_free_lock;
	}

	ipc_sio = sio_op->sio_dev;

	if (!(filp->f_flags & O_NONBLOCK))
		set_bit(IS_BLOCKING, &ipc_sio->flag);

	/* only log in blocking mode to reduce flooding the log */
	if (test_bit(IS_BLOCKING, &ipc_sio->flag))
		dev_dbg(ipc_sio->dev, "sio read chid[%d] size=%zu",
			ipc_sio->channel->channel_id, size);

	/* First provide the pending skbuf to the user. */
	if (ipc_sio->rx_pending_buf) {
		skb = ipc_sio->rx_pending_buf;
		ipc_sio->rx_pending_buf = NULL;
	}

	/* check skb is available in rx_list or wait for skb in case of
	 * blocking read
	 */
	while (!skb && !(skb = skb_dequeue(&ipc_sio->rx_list))) {
		if (!test_bit(IS_BLOCKING, &ipc_sio->flag)) {
			ret_err = -EAGAIN;
			goto err_free_lock;
		}
		/* Suspend the user app and wait a certain time for data
		 * from CP.
		 */
		wait_for_completion_interruptible_timeout
		(&ipc_sio->read_sem, msecs_to_jiffies(IPC_READ_TIMEOUT));

		if (test_bit(IS_DEINIT, &ipc_sio->flag)) {
			ret_err = -EPERM;
			goto err_free_lock;
		}
	}

	read_byt = imem_sys_sio_read(ipc_sio, buf, size, skb);
	mutex_unlock(&sio_floc);
	return read_byt;

err_free_lock:
	mutex_unlock(&sio_floc);
err:
	return ret_err;
}

/* Route the user data to the shared memory layer. */
static ssize_t ipc_sio_fop_write(struct file *filp, const char __user *buf,
				 size_t size, loff_t *l)
{
	struct iosm_sio_open_file *sio_op = filp->f_inode->i_private;
	struct iosm_sio *ipc_sio;
	bool is_blocking;
	ssize_t write_byt;
	int ret_err;

	if (!buf) {
		ret_err = -EINVAL;
		goto err;
	}

	mutex_lock(&sio_floc_wr);
	if (!sio_op->sio_dev) {
		ret_err = -EIO;
		goto err_free_lock;
	}

	ipc_sio = sio_op->sio_dev;

	is_blocking = !(filp->f_flags & O_NONBLOCK);
	if (!is_blocking) {
		if (test_bit(WRITE_IN_USE, &ipc_sio->flag)) {
			ret_err = -EAGAIN;
			goto err_free_lock;
		}
	}

	write_byt =  imem_sys_sio_write(ipc_sio, buf, size, is_blocking);
	mutex_unlock(&sio_floc_wr);
	return write_byt;

err_free_lock:
	mutex_unlock(&sio_floc_wr);
err:
	return ret_err;
}

/* poll for applications using nonblocking I/O */
static __poll_t ipc_sio_fop_poll(struct file *filp, poll_table *wait)
{
	struct iosm_sio *ipc_sio =
		container_of(filp->private_data, struct iosm_sio, misc);
	__poll_t mask = 0;

	/* Just registers wait_queue hook. This doesn't really wait. */
	poll_wait(filp, &ipc_sio->poll_inq, wait);

	/* Test the fill level of the skbuf rx queue. */
	if (!skb_queue_empty(&ipc_sio->rx_list) || ipc_sio->rx_pending_buf)
		mask |= EPOLLIN | EPOLLRDNORM; /* readable */

	if (!test_bit(WRITE_IN_USE, &ipc_sio->flag))
		mask |= EPOLLOUT | EPOLLWRNORM; /* writable */

	return mask;
}

struct iosm_sio *ipc_sio_init(struct iosm_imem *ipc_imem, const char *name)
{
	static const struct file_operations fops = {
		.owner = THIS_MODULE,
		.open = ipc_sio_fop_open,
		.release = ipc_sio_fop_release,
		.read = ipc_sio_fop_read,
		.write = ipc_sio_fop_write,
		.poll = ipc_sio_fop_poll,
	};

	struct iosm_sio *ipc_sio = kzalloc(sizeof(*ipc_sio), GFP_KERNEL);

	if (!ipc_sio)
		return NULL;

	ipc_sio->dev = ipc_imem->dev;
	ipc_sio->pcie = ipc_imem->pcie;
	ipc_sio->ipc_imem = ipc_imem;

	mutex_init(&sio_floc);
	mutex_init(&sio_floc_wr);
	init_completion(&ipc_sio->read_sem);

	skb_queue_head_init(&ipc_sio->rx_list);
	init_waitqueue_head(&ipc_sio->poll_inq);

	strncpy(ipc_sio->devname, name, sizeof(ipc_sio->devname) - 1);
	ipc_sio->devname[IPC_SIO_DEVNAME_LEN - 1] = '\0';

	ipc_sio->misc.minor = MISC_DYNAMIC_MINOR;
	ipc_sio->misc.name = ipc_sio->devname;
	ipc_sio->misc.fops = &fops;
	ipc_sio->misc.mode = IPC_CHAR_DEVICE_DEFAULT_MODE;

	if (misc_register(&ipc_sio->misc) != 0) {
		kfree(ipc_sio);
		return NULL;
	}

	return ipc_sio;
}

void ipc_sio_deinit(struct iosm_sio *ipc_sio)
{
	if (ipc_sio) {
		misc_deregister(&ipc_sio->misc);

		set_bit(IS_DEINIT, &ipc_sio->flag);
		/* Applying memory barrier so that ipc_sio->flag is updated
		 * before being read
		 */
		smp_mb__after_atomic();
		if (test_bit(IS_BLOCKING, &ipc_sio->flag)) {
			complete(&ipc_sio->read_sem);
			complete(&ipc_sio->channel->ul_sem);
		}

		mutex_lock(&sio_floc);
		mutex_lock(&sio_floc_wr);

		ipc_pcie_kfree_skb(ipc_sio->pcie, ipc_sio->rx_pending_buf);
		skb_queue_purge(&ipc_sio->rx_list);

		if (ipc_sio->sio_fop)
			ipc_sio->sio_fop->sio_dev = NULL;

		mutex_unlock(&sio_floc_wr);
		mutex_unlock(&sio_floc);

		kfree(ipc_sio);
	}
}
