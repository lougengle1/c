#include <linux/pci.h>
#include <linux/module.h>
#include <linux/init.h>
#include "sl_nic_netdev.h"
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
#include <linux/delay.h>
#include <linux/kthread.h>
#include "slk_tools/slk_kernel.h"
#include "slk_tools/slk_define.h"
#include "slk_tools/slk_rx_chan_new.h"
#include "sl_nic_driver.h"
#include "user_irq_define.h"
#include "sl_nic_define.h"
#include "control_cdev.h"
#include "stream_data_cdev.h"
#include "sl_nic_dev.h"
#include <linux/etherdevice.h>

static int debug = -1;
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0=none,...,16=all)");
struct cherry_kiwi_adapter		g_driver_data;

static struct pci_device_id g_hw_pci_id[] =
{
	{ KIWI_POLL_VENDOR_ID, KIWI_POLL_DEVICE_ID,PCI_ANY_ID, PCI_ANY_ID,0,0,0 },
	{}
};

void pci_enable_irq(void)
{
	WRITE_REG(g_driver_data.m_mem_base0, SYS_IRQ, FPGA_IRQ_ENABLE);
}

static void pci_disable_irq(void)
{
	WRITE_REG(g_driver_data.m_mem_base0, SYS_IRQ, FPGA_IRQ_DISABLE);
}


static void constructor(void)
{
	g_driver_data.m_e_rls = em_rls_ctrl_init; ///卸载资源控制的初始态
	g_driver_data.m_mem_base0 = 0;
	g_driver_data.m_mem_base1 = 0;
	g_driver_data.m_mem_base11 = 0;
	g_driver_data.m_mem_base2 = 0;
	g_driver_data.m_debug = debug;
	sema_init(&g_driver_data.m_event_sem, 1);
	spin_lock_init(&g_driver_data.m_tx_lock);
}

static void unmaping_bars(void)
{
	iounmap((volatile void*)g_driver_data.m_mem_base2);
	g_driver_data.m_mem_base2 = 0;
	iounmap((volatile void*)g_driver_data.m_mem_base1);
	g_driver_data.m_mem_base1 = 0;
	iounmap((volatile void*)g_driver_data.m_mem_base11);
	g_driver_data.m_mem_base11 = 0;
	iounmap((volatile void*)g_driver_data.m_mem_base0);
	g_driver_data.m_mem_base0 = 0;
}

//static int wait_hw_init_finish(void)
//{
//	uint32_t		status_value = 0x0;
//	int				i_count = 0;
//	return 1;
//	while (1)
//	{
//		msleep(1000);
//
//		status_value = READ_REG(g_driver_data.m_mem_base0, SYS_STATUS);
//		sl_debug("read reg:%08x\n", status_value);
//
//		if ((status_value != 0xFFFFFFFF) && ((status_value & SYS_STATUS_DATA) == SYS_STATUS_DATA))
//		{
//			return 1;
//		}
//
//		i_count++;
//
//		if (i_count == 30)
//		{
//			return 0;
//		}
//	}
//
//	return 0;
//}


static void reset(void)
{

	//uint32_t		reg_value;
	WRITE_REG(g_driver_data.m_mem_base0, SYS_SOFT_RST, SYS_SOFT_RST_DISABLE_DATA);
	wmb();
	mdelay(600);
	//WRITE_REG(g_driver_data.m_mem_base0,SYS_SOFT_RST,SYS_SOFT_RST_ENABLE_DATA);
	mdelay(30);

}

static void mapping_bars(void)
{

	unsigned long		reg_addr_base2;
	unsigned long		reg_addr_base1;
	unsigned long		reg_addr_base0;

	/// bar2
	reg_addr_base2 = pci_resource_start(g_driver_data.m_dev_pci, 2);

	sl_debug("test base addr %lx\n", reg_addr_base2);

	if (reg_addr_base2)
	{
		g_driver_data.m_mem_base2 = (unsigned long)ioremap_nocache(reg_addr_base2, pci_resource_len(g_driver_data.m_dev_pci, 2));
		if (!g_driver_data.m_mem_base2)
		{
			sl_debug("ioremap bar2_nocache addr error\n");
		}

	}

	/// bar1
	reg_addr_base1 = pci_resource_start(g_driver_data.m_dev_pci, 1);

	sl_debug("test base addr %lx\n", reg_addr_base1);

	if (reg_addr_base1)
	{
		g_driver_data.m_mem_base1 = (unsigned long)ioremap_nocache(reg_addr_base1, 0x8000);
		if (!g_driver_data.m_mem_base1)
		{
			sl_debug("ioremap bar1_nocache addr error");
		}

		g_driver_data.m_mem_base11 = (unsigned long)ioremap_wc(reg_addr_base1 + 0x8000, 0x8000);
		if (!g_driver_data.m_mem_base11)
		{
			sl_debug("ioremap bar11_wc addr error");
		}
	}


	/// bar 0
	reg_addr_base0 = pci_resource_start(g_driver_data.m_dev_pci, 0);

	sl_debug("test base addr %lx\n", reg_addr_base0);

	if (reg_addr_base0)
	{
		g_driver_data.m_mem_base0 = (unsigned long)ioremap_nocache(reg_addr_base0, pci_resource_len(g_driver_data.m_dev_pci, 0));
		//sl_debug("pci_resource_len(g_driver_data.m_dev_pci, 0) is %lu\n", pci_resource_len(g_driver_data.m_dev_pci, 0));
		if (!g_driver_data.m_mem_base0)
		{
			sl_debug("ioremap_bar0 nocache addr error\n");
		}
	}
}

static void destructor(void)
{
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	if (g_driver_data.m_rx_seqs)
	{
		kfree(g_driver_data.m_rx_seqs);
		g_driver_data.m_rx_seqs = NULL;
	}

	switch (g_driver_data.m_e_rls)
	{
	case em_rls_ctrl_init:
		del_timer_sync(&g_driver_data.m_time_stamp);
		del_timer_sync(&g_driver_data.m_link_timer);

#ifdef INTERRUPT_MODE

		free_irq(g_driver_data.m_dev_pci->irq, &g_driver_data);
		msleep(500);
		sl_debug("pci_disable_msi");
		pci_disable_msi(g_driver_data.m_dev_pci);
#else

		if (IS_ERR(g_driver_data.carry_task) == 0)
		{
			kthread_stop(g_driver_data.carry_task);
			sl_debug("thread stop");
		}

#endif
		slk_destory_netdev();
	case em_slnic_netdev_alloc:
		slk_rx_channel_new_destory(&g_driver_data.m_rx_channel);
	case em_slk_rx_channel_new_create:
		if (g_driver_data.m_tx_queue)
			slk_destroy_tx_mem_op_list(g_driver_data.m_tx_queue, g_driver_data.m_dev_pci);
	case em_slk_create_tx_mem_op_list:
		dma_free_coherent(&g_driver_data.m_dev_pci->dev, PAGE_SIZE, g_driver_data.m_tx_count.m_vis_addr, g_driver_data.m_tx_count.m_dam_addr);
	case em_dma_alloc_coherent:
		cdev_del(&g_driver_data.m_dev_slnic);
	case em_slnic_cdev_add:
		cdev_del(&g_driver_data.m_dev_stream);
	case em_data_cdev_add:
		cdev_del(&g_driver_data.m_dev_reg);
	case em_char_cdev_add:
	//case em_pci_wait_hw_init_finish:
	case em_pci_reenable_device_val:
		unmaping_bars();
		pci_clear_master(g_driver_data.m_dev_pci);
		pci_release_regions(g_driver_data.m_dev_pci);
	case em_pci_request_regions:
		pci_disable_device(g_driver_data.m_dev_pci);
	case em_pci_enable_device_mem:
	//	slk_destory_default_log();
	//case em_slk_create_default_log_val:
	default:
		sl_debug("cherry_destructor is endding");
	}
}


void slnic_netdev_rx_handler(struct net_device *ndev)
{
	struct slk_net_priv *priv = netdev_priv(ndev);
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	//pci_disable_irq();
	napi_schedule(&priv->m_sl_nic_napi);
	//struct slnic_netdev_rx *p_rx = &priv->m_rx;
	////p_rx->buffer = g_driver_data.m_rx_channel.m_dma_chan[priv->port].m_virt_addr_start;

	//sl_debug("[port:%d][soft_count:%d] [p_rx->m_next_count %d] [hard_count:%d] [priv->enable:%d]\n", priv->port,
	//	p_rx->m_chunk_count, p_rx->m_next_chunk, 
	//	p_rx->buffer[p_rx->m_next_chunk].info.m_chunk_count, priv->m_rx_enable);

	//if (priv->m_rx_enable == 1)
	//{
	//	if (slnic_rx_chunk_compare(p_rx))
	//	{
	//		//pci_disable_irq();
	//		napi_schedule(&priv->m_sl_nic_napi);
	//	}
	//}
	//else if(priv->m_rx_enable == 0)
	//{
	//	sl_debug("slnic ifconfig not\n");
	//}
}

static irqreturn_t poll_interrupt(int irq, void *dev_id/*, struct pt_regs *regs*/)
{
	int i = 0;
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	struct cherry_kiwi_adapter *p_data = (struct cherry_kiwi_adapter*)(dev_id);
	pci_disable_irq();
	for (i = 0; i < MAC_PORT_NUM; ++i)
	{
		if (p_data->ndev[i])
		{
			//printk("1111111111111\n");
			slnic_netdev_rx_handler(p_data->ndev[i]);
		}

	}

	return IRQ_HANDLED;
}

static void slnic_set_time(void)
{
	struct timespec ts;
	getnstimeofday(&ts);
	///秒和纳秒数配置下去
	WRITE_REG(g_driver_data.m_mem_base0, SYS_TIME_HMS, ts.tv_sec);
	WRITE_REG(g_driver_data.m_mem_base0, SYS_TIME_NS, ts.tv_nsec);
	WRITE_REG(g_driver_data.m_mem_base0, SYS_TIME_CTRL, 0x40000001);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
static void slnic_link_timer_callback(unsigned long data)
{
	int i;
	for (i = 0; i < SLNIC_MAX_PORTS; ++i)
		if (g_driver_data.ndev[i])
			slnic_netdev_check_link(g_driver_data.ndev[i]);

	mod_timer(&g_driver_data.m_link_timer, jiffies + HZ);
}

static void slnic_rectify_hwtime_callback(unsigned long data)
{
	slnic_set_time();
	mod_timer(&g_driver_data.m_time_stamp, jiffies + HZ*3600);
}


#else
static void slnic_link_timer_callback(struct timer_list*t)
{
	int i;
	for (i = 0; i < SLNIC_MAX_PORTS; ++i)
		if (g_driver_data.ndev[i])
			slnic_netdev_check_link(g_driver_data.ndev[i]);

	mod_timer(&g_driver_data.m_link_timer, jiffies + HZ*3600);
}

static void slnic_rectify_hwtime_callback(struct timer_list*t)
{
	slnic_set_time();
	mod_timer(&g_driver_data.m_time_stamp, jiffies + HZ*3600);
}

#endif

static int pci_set_payload(struct pci_dev *dev)
{
	int					pos, ppos;
	u16					psz;
	u16					dctl, dsz;
	u16					rsz;
	uint32_t			dev_cap2;
	struct pci_dev*		parent;


	parent = dev->bus->self;
	pos = pci_find_capability(dev, PCI_CAP_ID_EXP);
	if (!pos)
		return 0;

	pci_read_config_word(dev, pos + PCI_EXP_DEVCTL, &dctl);
	dsz = (dctl & PCI_EXP_DEVCTL_PAYLOAD) >> 5;
	rsz = (dctl & PCI_EXP_DEVCTL_READRQ) >> 12;

	sl_debug("dsz: %08x, dread:%08x", dsz, rsz);

	psz = 0x4;
	pci_write_config_word(dev, pos + PCI_EXP_DEVCTL,
		(dctl & ~PCI_EXP_DEVCTL_PAYLOAD) +
		(psz << 5));

	pci_read_config_word(dev, pos + PCI_EXP_DEVCTL, &dctl);

	pci_write_config_word(dev, pos + PCI_EXP_DEVCTL,
		(dctl & ~PCI_EXP_DEVCTL_READRQ) +
		(psz << 12));

	pci_read_config_dword(dev, pos + PCI_EXP_DEVCAP2, &dev_cap2);

	sl_warning("DEV PCI_EXP_DEVCAP2:%08x", dev_cap2);

	///*********************parent************************
	parent = dev->bus->self;

	/* Read Parent MaxPayload setting */
	ppos = pci_find_capability(parent, PCI_CAP_ID_EXP);
	if (!ppos)
		return 0;

	/* If parent payload > device max payload -> error
	* If parent payload > device payload -> set speed
	* If parent payload <= device payload -> do nothing
	*/
	pci_read_config_word(dev, ppos + PCI_EXP_DEVCTL, &dctl);
	dsz = (dctl & PCI_EXP_DEVCTL_PAYLOAD) >> 5;
	rsz = (dctl & PCI_EXP_DEVCTL_READRQ) >> 12;

	sl_debug("dsz: %08x, dread:%08x", dsz, rsz);

	psz = 0x4;
	pci_write_config_word(dev, ppos + PCI_EXP_DEVCTL,
		(dctl & ~PCI_EXP_DEVCTL_PAYLOAD) +
		(psz << 5));

	pci_read_config_word(dev, ppos + PCI_EXP_DEVCTL, &dctl);

	pci_write_config_word(dev, ppos + PCI_EXP_DEVCTL,
		(dctl & ~PCI_EXP_DEVCTL_READRQ) +
		(psz << 12));

	pci_read_config_dword(parent, ppos + PCI_EXP_DEVCAP2, &dev_cap2);

	sl_warning("HOST PCI_EXP_DEVCAP2:%08x", dev_cap2);


	return 0;
}

static int slnic_rx_chunk_task_inplace(struct slnic_netdev_rx* p_rx, struct slk_net_priv*p_slk_priv)
{
	int hard_counter = p_rx->buffer[p_rx->m_next_chunk].info.m_chunk_count;
	int soft_counter = p_rx->m_chunk_count;
	int mf_and_ff = p_rx->buffer[p_rx->m_next_chunk].info.m_fragment_flag;
	int payload_lenth = ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);

	//保护模式，防止硬件bug给的长度不对，导致系统奔溃
	if (p_slk_priv->m_p_data.offset + payload_lenth > PAGE_SIZE * 16)
	{
		p_slk_priv->m_p_data.offset = 0;
		return 0;
		sl_error("[p_slk_priv->m_p_data.offset:%d] [payload_lenth:%d]", p_slk_priv->m_p_data.offset, payload_lenth);
	}


	if (hard_counter == soft_counter)
	{
		sl_debug("[m_fragment_flag :%d]\n", p_rx->buffer[p_rx->m_next_chunk].info.m_fragment_flag);
		/*数据来了，m_chunk_cout硬件上传是一直计数累加的*/
		//解析mm和mf的字段
		if (mf_and_ff == 3)
		{
			memcpy(p_slk_priv->m_p_data.m_p_poll_data + p_slk_priv->m_p_data.offset, p_rx->buffer[p_rx->m_next_chunk].pay_load, payload_lenth);
			p_slk_priv->m_p_data.offset = p_slk_priv->m_p_data.offset + ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);
			return 0;
		}
		else if (mf_and_ff == 2)
		{
			memcpy(p_slk_priv->m_p_data.m_p_poll_data + p_slk_priv->m_p_data.offset, p_rx->buffer[p_rx->m_next_chunk].pay_load, payload_lenth);
			p_slk_priv->m_p_data.offset = p_slk_priv->m_p_data.offset + ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);
			return p_slk_priv->m_p_data.offset;
		}
		else if (mf_and_ff == 0)
		{
			memcpy(p_slk_priv->m_p_data.m_p_poll_data, p_rx->buffer[p_rx->m_next_chunk].pay_load, payload_lenth);
			p_slk_priv->m_p_data.offset = ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);
			return p_slk_priv->m_p_data.offset;
		}
		else
		{
			sl_error("mf_and_ff is error %d", mf_and_ff);
		}
	}
	else if (hard_counter != soft_counter)
	{
		return -1;
	}

}

static int slnic_task_poll(struct net_device *ndev)
{
	struct slk_net_priv *priv = netdev_priv(ndev);
	struct slnic_netdev_rx *p_rx = &priv->m_rx;
	//char* ptr = NULL;
	int received = 0;
	int pkt_size;
	int ptr_count = 0;
	unsigned long long timensval;
	int budget = 5;


	while ((received < budget) && (priv->b_napi_running == 1) && (priv->m_rx_enable == 1))
	{
		// -1 代表没数据，0 代表要组包，组包但是计数要增加，其他代表数据的长度
		pkt_size = slnic_rx_chunk_task_inplace(p_rx, priv);
		if (pkt_size == -1)
		{
			break;
		}
		else if (pkt_size == 0)
		{
			goto add_count;
		}

		//如果有数据上传，把offset 置为0
		sl_debug("priv->m_p_data.offset is %d\n", priv->m_p_data.offset);
		//sl_debug("received is %d\n", received);
		//for (ptr_count = 0; ptr_count < priv->m_p_data.offset; ptr_count++)
		//{
		//	printk(" 0x%x", priv->m_p_data.m_p_poll_data[ptr_count]);
		//	if ((ptr_count + 1) % 8 == 0)
		//		printk("\n");
		//}

		priv->skb = netdev_alloc_skb(ndev, pkt_size + 2);
		if (NULL == priv->skb)
		{
			sl_error("netdev_alloc_skb failed\n");
			break;
		}

		skb_reserve(priv->skb, NET_IP_ALIGN);
		timensval = (p_rx->buffer[p_rx->m_next_chunk].info.m_systemTime_ns) + ((p_rx->buffer[p_rx->m_next_chunk].info.m_systemTime_s)) * 1000000000ull;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
		priv->skb->tstamp = timensval;
#else
		priv->skb->tstamp.tv64 = timensval;
#endif

		memcpy(skb_put(priv->skb, priv->m_p_data.offset), priv->m_p_data.m_p_poll_data, priv->m_p_data.offset);
		priv->skb->protocol = eth_type_trans(priv->skb, ndev);
		netif_receive_skb(priv->skb);
		memset(priv->m_p_data.m_p_poll_data, 0, sizeof(priv->m_p_data.m_p_poll_data));
		priv->m_p_data.offset = 0;
		priv->skb = NULL;
		priv->stats.rx_packets++;
		priv->stats.rx_bytes += pkt_size;

	add_count:
		p_rx->m_next_chunk = (p_rx->m_next_chunk + 1) & (priv->stu_rx_channel_config.node_num - 1);
		p_rx->m_chunk_count = (p_rx->m_chunk_count + 1) & 0xffff;

		if (((p_rx->m_chunk_count - 1) & (priv->stu_rx_channel_config.node_num - 1)) == (priv->stu_rx_channel_config.node_num >> 1)
			|| ((p_rx->m_chunk_count) & (priv->stu_rx_channel_config.node_num - 1)) == (priv->stu_rx_channel_config.node_num - 1))
		{
			WRITE_REG(g_driver_data.m_mem_base0, SW_PKT_CUR_MEM_OFF0 + SW_PKT_CUR_MEM_OFFSET * priv->port,
				(p_rx->m_next_chunk - 1)&(priv->stu_rx_channel_config.node_num - 1));
		}

		received++;

		//test
		//sl_debug("pkt_size is %d\n", pkt_size);
		//sl_debug("received is %d\n", received);
		//for (ptr_count = 0; ptr_count < pkt_size; ptr_count++)
		//{
		//	printk(" 0x%x", ptr[ptr_count]);
		//	if ((ptr_count + 1) % 8 == 0)
		//		printk("\n");
		//}
		//
		//因为上面ptr的数据拷贝到skb_buff,这个驱动收到的包已经可以回收了
		//发一个，回收一个

	}

	return received;

}


static int slnic_carry_task(void* __unused)
{
	int index = 0;
	int i;
	//轮询
	while (!kthread_should_stop())
	{

		for (i = 0; i < MAC_PORT_NUM; ++i)
		{
			if (g_driver_data.ndev[i])
			{
				//printk("1111111111111\n");
				slnic_task_poll(g_driver_data.ndev[i]);
			}

		}

		//获取数据
		
		msleep(10);
	}
	return 0;
}


static int super_kiwi_carry(void)
{
	g_driver_data.carry_task = kthread_create(slnic_carry_task, NULL, "slnic_task");
	if (IS_ERR(g_driver_data.carry_task))
	{
		sl_error("create super kiwi carry thread error !\n");
		return -1;
	}

	wake_up_process(g_driver_data.carry_task);

	return 0;
}


static void sl_pci_remov(struct pci_dev* dev)
{
	destructor();
}


static int sl_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int						result;
	unsigned int			value;
	unsigned int			port_num;
	dev_t					reg_dev = 0;
	dev_t					stream_dev = 0;
	dev_t					slnic_dev = 0;
	int						max_payload = 0;
	int						channel;

	g_driver_data.m_dev_pci = pdev;

	///构造
	constructor();

	result = pci_enable_device(pdev);
	if (result)
	{
		sl_error("pci_enable_device_mem failed: %d\n", result);
		g_driver_data.m_e_rls = em_pci_enable_device_mem;
		goto out;
	}

	result = pci_request_regions(pdev, SL_DRV_NAME);
	if (result)
	{
		sl_error("pci_request_regions failed: %d\n", result);
		g_driver_data.m_e_rls = em_pci_request_regions;
		goto out;
	}

	pci_set_master(pdev);

	///重启设备
	{
		/*将pci寄存器映射到内存空间*/
		mapping_bars();

		value = READ_REG(g_driver_data.m_mem_base0, SYS_STATUS);
		sl_debug("read status reg:%08x", value);

		pci_save_state(pdev);
		reset();
		pci_restore_state(pdev);

		result = pci_reenable_device(pdev);
		if (result)
		{
			g_driver_data.m_e_rls = em_pci_reenable_device_val;
			sl_error("pci reenale device error\n");
			goto out;
		}

		pci_set_master(pdev);
		mapping_bars();
	}

	///配置时间
	slnic_set_time();

	///设置字符设备
	reg_dev = MKDEV(KIWI_REG_CHAR_MAJOR, KIWI_REG_CHAR_MINOR);

	cdev_init(&g_driver_data.m_dev_reg, &g_fop_control);

	g_driver_data.m_dev_reg.owner = THIS_MODULE;
	g_driver_data.m_dev_reg.ops = &g_fop_control;

	result = cdev_add(&g_driver_data.m_dev_reg, reg_dev, 1);
	if (result)
	{
		sl_error("Error %d addint log\n", result);
		g_driver_data.m_e_rls = em_char_cdev_add;
		goto out;
	}

	/// 数据流处理
	stream_dev = MKDEV(KIWI_STREAM_CHAR_MAJOR, KIWI_STREAM_CHAR_MINOR);
	cdev_init(&g_driver_data.m_dev_stream, &g_fop_stream);
	g_driver_data.m_dev_stream.owner = THIS_MODULE;
	g_driver_data.m_dev_stream.ops = &g_fop_stream;

	result = cdev_add(&g_driver_data.m_dev_stream, stream_dev, 1);
	if (result)
	{
		sl_error("Error %d addint quote\n", result);
		g_driver_data.m_e_rls = em_data_cdev_add;
		goto out;
	}

	///slnic_char
	slnic_dev = MKDEV(KIWI_SLNIC_CHAR_MAJOR, KIWI_SLNIC_CHAR_MINOR);
	cdev_init(&g_driver_data.m_dev_slnic, &g_fop_slnic);
	g_driver_data.m_dev_slnic.owner = THIS_MODULE;
	g_driver_data.m_dev_slnic.ops = &g_fop_slnic;

	result = cdev_add(&g_driver_data.m_dev_slnic, slnic_dev, 1);
	if (result)
	{
		sl_error("Error %d addint quote\n",result);
		g_driver_data.m_e_rls = em_slnic_cdev_add;
		goto out;
	}

	//memory方式
	g_driver_data.m_tx_count.m_vis_addr = dma_alloc_coherent(&(pdev->dev), PAGE_SIZE, &g_driver_data.m_tx_count.m_dam_addr, GFP_KERNEL);
	if (!g_driver_data.m_tx_count.m_vis_addr)
	{
		g_driver_data.m_e_rls = em_dma_alloc_coherent;
		sl_error("dma_alloc_coherent\n");
		goto out;
	}

	*(volatile unsigned int*)(g_driver_data.m_tx_count.m_vis_addr) = 0;

	WRITE_REG(g_driver_data.m_mem_base0, SYS_TX_REGISTER, g_driver_data.m_tx_count.m_dam_addr);

	
	g_driver_data.m_tx_queue = slk_create_tx_mem_op_list(g_driver_data.m_mem_base11, g_driver_data.m_mem_base1, 16, g_driver_data.m_mem_base0, g_driver_data.m_dev_pci);
	if (!g_driver_data.m_tx_queue)
	{
		g_driver_data.m_e_rls = em_slk_create_tx_mem_op_list;
		sl_error("slk_create_tx_mem_op_list failed\n");
		goto out;
	}

	g_driver_data.m_rx_seqs = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!g_driver_data.m_rx_seqs)
	{
		g_driver_data.m_e_rls = em_slk_rx_channel_new_create;
		sl_error("m_rx_seqs kmalloc");
		goto out;
	}

	for (channel = 0; channel < MAX_CHANNEL_NUM; ++channel)
	{
		g_driver_data.m_rx_seqs[channel].m_info.m_read_seq = 0;
	}

	init_channel_new_node_param(g_driver_data.m_rx_channel, 0, SL_NIC_RX_CHAN0_NODE_SIZE, SL_NIC_RX_CHAN0_NODE_NUM);
	init_channel_new_node_param(g_driver_data.m_rx_channel, 1, SL_NIC_RX_CHAN1_NODE_SIZE, SL_NIC_RX_CHAN1_NODE_NUM);
	init_channel_new_node_param(g_driver_data.m_rx_channel, 2, SL_NIC_RX_CHAN2_NODE_SIZE, SL_NIC_RX_CHAN2_NODE_NUM);
	init_channel_new_node_param(g_driver_data.m_rx_channel, 3, SL_NIC_RX_CHAN3_NODE_SIZE, SL_NIC_RX_CHAN3_NODE_NUM);
	init_channel_new_node_param(g_driver_data.m_rx_channel, 4, SL_NIC_RX_CHAN4_NODE_SIZE, SL_NIC_RX_CHAN4_NODE_NUM);
	init_channel_new_node_param(g_driver_data.m_rx_channel, 5, SL_NIC_RX_CHAN5_NODE_SIZE, SL_NIC_RX_CHAN5_NODE_NUM);
	init_channel_new_node_param(g_driver_data.m_rx_channel, 6, SL_NIC_RX_CHAN6_NODE_SIZE, SL_NIC_RX_CHAN6_NODE_NUM);
	init_channel_new_node_param(g_driver_data.m_rx_channel, 7, SL_NIC_RX_CHAN7_NODE_SIZE, SL_NIC_RX_CHAN7_NODE_NUM);

	init_channel_new_param(g_driver_data.m_rx_channel,
		g_driver_data.m_dev_pci,
		g_driver_data.m_mem_base0,
		8);

	if (slk_rx_channel_new_create(&g_driver_data.m_rx_channel))
	{
		g_driver_data.m_e_rls = em_slk_rx_channel_new_create;
		sl_error("create rx mmap error!");
		goto out;
	}

	///网络设备的申请和注释
	for (port_num = 0; port_num < SLNIC_MAX_PORTS; port_num++)
	{
		result = slnic_netdev_alloc(&g_driver_data.ndev[port_num],port_num,g_driver_data.m_mem_base0);
		if (!result)
		{
			g_driver_data.m_e_rls = em_slnic_netdev_alloc;
			sl_error("slnic_netdev_alloc is failed\n");
			goto out;
		}
	}

	///Set up timer to poll link status 
	SLK_CLCOK_SETUP(&g_driver_data.m_link_timer, slnic_link_timer_callback, (unsigned long)(&g_driver_data));
	mod_timer(&g_driver_data.m_link_timer, jiffies + HZ);

	/// 硬件时间戳矫正
	SLK_CLCOK_SETUP(&g_driver_data.m_time_stamp, slnic_rectify_hwtime_callback, (unsigned long)(&g_driver_data));
	mod_timer(&g_driver_data.m_time_stamp, jiffies + HZ*3600);
	

#ifdef INTERRUPT_MODE
	result = pci_enable_msi(pdev);
	if (result)
	{
		sl_error("pci_enable_msi error %d\n", result);
	}

	result = request_irq(pdev->irq, poll_interrupt, IRQF_SHARED, KIWI_POLL_DRIVER_NAME, &g_driver_data);
	if (result)
	{
		sl_error("short: can't get assigned irq %i\n", pdev->irq);
	}

#else
	/// 启动搬运线程
	result = super_kiwi_carry();
	if (result < 0)
	{
		g_driver_data.m_e_rls = em_slnic_carry;
		goto out;
	}
	
#endif

#ifndef SLK_QUEM_EVN
	pci_set_payload(pdev);

	/// 设置pcie的最大值
	max_payload = read_pcie_parent_maxpayload(pdev);
	if (max_payload == 0 || max_payload == 1)
	{
		WRITE_REG(g_driver_data.m_mem_base0, REG_PCIE_CTRL_ADDR, max_payload);
	}
#endif

	

	pci_enable_irq();
	sl_debug(KERN_ALERT "FPGA DEVICE NUME NODE:%d,CURRENT CPU NODE ID:%d,CURRENT CPU ID:%d\n", (&g_driver_data.m_dev_pci->dev)->numa_node, numa_node_id(), get_cpu());
	return 0;

out:
	sl_pci_remov(pdev);
	return result;
}


static struct pci_driver g_hw_pci =
{
	.name = KIWI_POLL_DRIVER_NAME,
	.id_table = g_hw_pci_id,
	.probe = sl_pci_probe,
	.remove = sl_pci_remov,
};


static int __init sl_nic_init_module(void)
{
	sl_debug("net dev in");
	return pci_register_driver(&g_hw_pci);
}

static void __exit sl_nic_exit_module(void)
{
	sl_debug("net dev out");
	pci_unregister_driver(&g_hw_pci);
}


module_init(sl_nic_init_module);
module_exit(sl_nic_exit_module);

MODULE_LICENSE("GPL");
