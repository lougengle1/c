#include <linux/pci.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h> /* sl_debug() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> /* mark_bh */
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/signal.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include "slk_tools/slk_define.h"
#include "slk_tools/slk_kernel.h"

#include "sl_nic_define.h"
#include "stream_data_cdev.h"
#include "user_irq_define.h"

static int stream_move_tx_node(struct cherry_kiwi_adapter *p_driver_data)
{
	slk_tx_mem_op_list_ready(p_driver_data->m_tx_queue);

	return 0;
}

static void check_recover(struct cherry_kiwi_adapter *p_driver_data)
{
	/// ��ȡdma��ֵ

	p_driver_data->tx_dma_count = READ_REG(p_driver_data->m_mem_base0, TX_REG_COUNTER);

	/// �ж�dma��ֵ��tx queue��ֵ�Ƿ���ͬ
	while (p_driver_data->m_tx_queue->counter != p_driver_data->tx_dma_count)
	{
		if (stream_move_tx_node(p_driver_data))
		{
			break;
		}

		if (unlikely(p_driver_data->m_tx_queue->counter == 0xfffffffe))
			p_driver_data->m_tx_queue->counter = 0;
		else
			p_driver_data->m_tx_queue->counter++;
	}

}

static const struct slk_offset_t channal_offset_val[8] = {
	{MAP_CHN_0_OFFSET, MAP_CHN_0_MAPLEN, SL_NIC_RX_CHAN0_NODE_SIZE, SL_NIC_RX_CHAN0_NODE_NUM},
	{MAP_CHN_1_OFFSET, MAP_CHN_1_MAPLEN, SL_NIC_RX_CHAN1_NODE_SIZE, SL_NIC_RX_CHAN1_NODE_NUM},
	{MAP_CHN_2_OFFSET, MAP_CHN_2_MAPLEN, SL_NIC_RX_CHAN2_NODE_SIZE, SL_NIC_RX_CHAN2_NODE_NUM},
	{MAP_CHN_3_OFFSET, MAP_CHN_3_MAPLEN, SL_NIC_RX_CHAN3_NODE_SIZE, SL_NIC_RX_CHAN3_NODE_NUM},
	{MAP_CHN_4_OFFSET, MAP_CHN_4_MAPLEN, SL_NIC_RX_CHAN4_NODE_SIZE, SL_NIC_RX_CHAN4_NODE_NUM},
	{MAP_CHN_5_OFFSET, MAP_CHN_5_MAPLEN, SL_NIC_RX_CHAN5_NODE_SIZE, SL_NIC_RX_CHAN5_NODE_NUM},
	{MAP_CHN_6_OFFSET, MAP_CHN_6_MAPLEN, SL_NIC_RX_CHAN6_NODE_SIZE, SL_NIC_RX_CHAN6_NODE_NUM},
	{MAP_CHN_7_OFFSET, MAP_CHN_7_MAPLEN, SL_NIC_RX_CHAN7_NODE_SIZE, SL_NIC_RX_CHAN7_NODE_NUM}
};

static
loff_t stream_data_llseek(struct file *filp, loff_t off, int whence)
{

	return 0;
}

static
ssize_t stream_data_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
	return 0;
}

static
ssize_t stream_data_write(struct file * pfile, const char __user * ch_user, size_t count, loff_t * pos)
{
	int i_result;
	struct inode *pnode = pfile->f_inode;
	struct cherry_kiwi_adapter *p_driver_data = container_of(pnode->i_cdev, struct cherry_kiwi_adapter, m_dev_stream);

	char*						ch_msg = 0;
	int							cp_len = 0;
	unsigned long flags;
	ch_msg = kmalloc(count, GFP_KERNEL);
	if (!ch_msg)
	{
		sl_error("[stream_data_write] kmolloc error!");
		return -1;
	}

	cp_len = copy_from_user(ch_msg, ch_user, count);
	if (cp_len)
	{
		sl_error("copy data error\n");
		return -1;
	}

	switch (ch_msg[0])
	{
	case CTL_COMMUNICATION:

		spin_lock_irqsave(&p_driver_data->m_tx_lock, flags);
		check_recover(p_driver_data);
		i_result = slk_tx_mem_op_list_write_safe((char*)(ch_user + 1 + 8), *(int*)(ch_msg + 5), p_driver_data->m_tx_queue);
		if (i_result < 0)
		{
			sl_error("tx dma:%08x,tx count:%08x,tx send count:%08x",
				p_driver_data->tx_dma_count,
				p_driver_data->m_tx_queue->counter,
				p_driver_data->m_tx_queue->snd_counter
			);
		}
		spin_unlock_irqrestore(&p_driver_data->m_tx_lock, flags);
		break;
	default:
		{
			sl_debug("[stream data]  can't know msg type: %c", ch_msg[0]);
		}
	}
	kfree(ch_msg);
	
	return count;
}

static
unsigned int stream_data_poll(struct file* filep, poll_table* wait)
{
	return 0;
}

static
int	stream_data_open(struct inode *pnode, struct file* pfile)
{
	sl_debug("[super_kiwi] [stream data open]");
	pfile->private_data = 0;
	return 0;
}

static
int stream_data_release(struct inode * pnode, struct file * pfile)
{
	return 0;
}

static
long stream_data_ioctl(struct file * pfile, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct inode *pnode = pfile->f_inode;
	struct cherry_kiwi_adapter *p_driver_data = container_of(pnode->i_cdev, struct cherry_kiwi_adapter, m_dev_stream);
	struct slk_addr_t slk_addr;
	int channel_id;
	int val = SHENGLI_DRIVER_VERSION;
	switch (cmd)
	{
	case SLK_IOCTL_GET_RX_ADDR:
		{
			if (copy_from_user(&slk_addr, (void *)arg, sizeof(slk_addr)))
				return -EFAULT;
			channel_id = slk_addr.m_channel;
			if (channel_id < 0 || channel_id > 7)
				return -1;
			slk_addr.m_offset = channal_offset_val[channel_id].m_off;
			slk_addr.m_lenth = channal_offset_val[channel_id].m_len;
			slk_addr.m_node_size = channal_offset_val[channel_id].m_node_size;
			slk_addr.m_node_num = channal_offset_val[channel_id].m_node_num;
			slk_addr.m_dma_addr = p_driver_data->m_rx_channel.m_dma_chan[channel_id].m_dma_addr_start;
			if (copy_to_user((void *)arg, &slk_addr, sizeof(slk_addr)) != 0)
				ret = -EFAULT;
		}
		break;
	case SLK_IOCTL_GET_RXSEQ_ADDR:
		{
			slk_addr.m_offset = MAP_RXSEQ_OFFSET;
			slk_addr.m_lenth = MAP_RXSEQ_MAPLEN;
			if (copy_to_user((void *)arg, &slk_addr, sizeof(slk_addr)) != 0)
				ret = -EFAULT;
		}
		break;
	case SLK_IOCTL_VERSION:
	{
		ret = __put_user(val, (int*)arg);
		sl_debug("version:%d", val);
	}
	break;
	default:
		ret = -EFAULT;
		break;
	}

	return ret;
}

static
int stream_data_mmap(struct file * filp, struct vm_area_struct *vma)
{
	int							ret;
	int							que_page_size;
	void*						que_page_addr;
	int							i;
	struct inode *pnode = filp->f_inode;
	struct cherry_kiwi_adapter *p_driver_data = container_of(pnode->i_cdev, struct cherry_kiwi_adapter, m_dev_stream);

	long offset = vma->vm_pgoff << PAGE_SHIFT;

	if (offset == MAP_RXSEQ_OFFSET)
	{
		ret = remap_pfn_range(vma, vma->vm_start,
			virt_to_phys(p_driver_data->m_rx_seqs) >> PAGE_SHIFT,
			PAGE_SIZE,
			vma->vm_page_prot);

		if (ret != 0)
		{
			sl_error("mmap rxseq");
			return -EAGAIN;
		}
	}


	for (i = 0; i < 8; ++i)
	{
		if (channal_offset_val[i].m_off == offset)
		{
			sl_debug("mmap channel %d\n", i);
			que_page_size = channal_offset_val[i].m_len;
			que_page_addr  = p_driver_data->m_rx_channel.m_dma_chan[i].m_virt_addr_start;
			/// ӳ��rx����
			if((ret = remap_pfn_range(vma,vma->vm_start, 
										virt_to_phys(que_page_addr) >> PAGE_SHIFT, 
										que_page_size, 
										vma->vm_page_prot)) < 0)
			{
				sl_error("mmap queue error:%08x", ret);
				return -EAGAIN;
			}
		}
	}

	return 0;
}

struct file_operations g_fop_stream =
{
	.owner = THIS_MODULE,
	.llseek = stream_data_llseek,
	.read = stream_data_read,
	.write = stream_data_write,
	.open = stream_data_open,
	.release = stream_data_release,
	.mmap = stream_data_mmap,
	.poll = stream_data_poll,
	.unlocked_ioctl = stream_data_ioctl,
};