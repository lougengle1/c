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

static int debug = -1;
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0=none,...,16=all)");

static struct cherry_kiwi_adapter		g_driver_data[SLNIC_MAX_NUM_DEVICES];
static unsigned 						slnic_count = 0;
static struct class 					*slnic_class;



static struct pci_device_id g_hw_pci_id[] =
{
	{ KIWI_POLL_VENDOR_ID_NF5, KIWI_POLL_DEVICE_ID_NF5,PCI_ANY_ID, PCI_ANY_ID,0,0,0 },
	{ KIWI_POLL_VENDOR_ID_NF10, KIWI_POLL_DEVICE_ID_NF10,PCI_ANY_ID, PCI_ANY_ID,0,0,0 },
	{}
};

void pci_enable_irq(struct cherry_kiwi_adapter *p_driver_data)
{
	WRITE_REG(p_driver_data->m_mem_base0, SYS_IRQ, FPGA_IRQ_ENABLE);
}

static void pci_disable_irq(struct cherry_kiwi_adapter *p_driver_data)
{
	WRITE_REG(p_driver_data->m_mem_base0, SYS_IRQ, FPGA_IRQ_DISABLE);
}


static void constructor(struct cherry_kiwi_adapter * p_driver_data)
{
	p_driver_data->m_e_rls = em_rls_ctrl_init; ///ж����Դ���Ƶĳ�ʼ̬
	p_driver_data->m_mem_base0 = 0;
	p_driver_data->m_mem_base1 = 0;
	p_driver_data->m_mem_base11 = 0;
	p_driver_data->m_mem_base2 = 0;
	p_driver_data->m_debug = debug;
	sema_init(&p_driver_data->m_event_sem, 1);
	spin_lock_init(&p_driver_data->m_tx_lock);
	p_driver_data->m_reg_dev = 0;
	p_driver_data->m_stream_dev = 0;
	p_driver_data->m_slnic_dev = 0;
}

static void unmaping_bars(struct cherry_kiwi_adapter * p_driver_data)
{
	iounmap((volatile void*)p_driver_data->m_mem_base2);
	p_driver_data->m_mem_base2 = 0;
	iounmap((volatile void*)p_driver_data->m_mem_base1);
	p_driver_data->m_mem_base1 = 0;
	iounmap((volatile void*)p_driver_data->m_mem_base11);
	p_driver_data->m_mem_base11 = 0;
	iounmap((volatile void*)p_driver_data->m_mem_base0);
	p_driver_data->m_mem_base0 = 0;
}


static void reset(struct cherry_kiwi_adapter * p_driver_data)
{


	WRITE_REG(p_driver_data->m_mem_base0, SYS_SOFT_RST, SYS_SOFT_RST_DISABLE_DATA);
	wmb();
	mdelay(600);

	mdelay(30);

}

static void mapping_bars(struct cherry_kiwi_adapter *p_driver_data)
{

	unsigned long		reg_addr_base2;
	unsigned long		reg_addr_base1;
	unsigned long		reg_addr_base0;

	/// bar2
	reg_addr_base2 = pci_resource_start(p_driver_data->m_dev_pci, 2);

	sl_debug("test base addr %lx\n", reg_addr_base2);

	if (reg_addr_base2)
	{
		p_driver_data->m_mem_base2 = (unsigned long)ioremap_nocache(reg_addr_base2, pci_resource_len(p_driver_data->m_dev_pci, 2));
		if (!p_driver_data->m_mem_base2)
		{
			sl_debug("ioremap bar2_nocache addr error\n");
		}

	}

	/// bar1
	reg_addr_base1 = pci_resource_start(p_driver_data->m_dev_pci, 1);

	sl_debug("test base addr %lx\n", reg_addr_base1);

	if (reg_addr_base1)
	{
		p_driver_data->m_mem_base1 = (unsigned long)ioremap_nocache(reg_addr_base1, 0x8000);
		if (!p_driver_data->m_mem_base1)
		{
			sl_debug("ioremap bar1_nocache addr error");
		}

		p_driver_data->m_mem_base11 = (unsigned long)ioremap_wc(reg_addr_base1 + 0x8000, 0x8000);
		if (!p_driver_data->m_mem_base11)
		{
			sl_debug("ioremap bar11_wc addr error");
		}
	}


	/// bar 0
	reg_addr_base0 = pci_resource_start(p_driver_data->m_dev_pci, 0);

	sl_debug("test base addr %lx\n", reg_addr_base0);

	if (reg_addr_base0)
	{
		p_driver_data->m_mem_base0 = (unsigned long)ioremap_nocache(reg_addr_base0, pci_resource_len(p_driver_data->m_dev_pci, 0));
		
		if (!p_driver_data->m_mem_base0)
		{
			sl_debug("ioremap_bar0 nocache addr error\n");
		}
	}
}

static void destructor(struct cherry_kiwi_adapter *p_driver_data)
{
	

	switch (p_driver_data->m_e_rls)
	{
	case em_rls_ctrl_init:
		del_timer_sync(&p_driver_data->m_time_stamp);
		del_timer_sync(&p_driver_data->m_link_timer);
		free_irq(p_driver_data->m_dev_pci->irq, p_driver_data);
		msleep(500);
		sl_debug("pci_disable_msi");
		pci_disable_msi(p_driver_data->m_dev_pci);
		slk_destory_netdev(p_driver_data);
	case em_slnic_netdev_alloc:
		slk_rx_channel_new_destory(&p_driver_data->m_rx_channel);
	case em_slk_rx_channel_new_create:
		if (p_driver_data->m_rx_seqs)
		{
			kfree(p_driver_data->m_rx_seqs);
			p_driver_data->m_rx_seqs = NULL;
		}
	case em_m_rx_seqs:
	if (p_driver_data->m_tx_queue)
		slk_destroy_tx_mem_op_list(p_driver_data->m_tx_queue, p_driver_data->m_dev_pci);
	case em_slk_create_tx_mem_op_list:
		dma_free_coherent(&p_driver_data->m_dev_pci->dev, PAGE_SIZE, p_driver_data->m_tx_count.m_vis_addr, p_driver_data->m_tx_count.m_dam_addr);
	case em_dma_alloc_coherent:
		device_destroy(slnic_class, p_driver_data->m_slnic_dev);
	case em_slnic_cdev_create_device:
		cdev_del(&p_driver_data->m_dev_slnic);
	case em_slnic_cdev_add:
		device_destroy(slnic_class, p_driver_data->m_stream_dev);
	case em_data_cdev_device_create:
		cdev_del(&p_driver_data->m_dev_stream);
	case em_data_cdev_add:
		device_destroy(slnic_class, p_driver_data->m_reg_dev);
	case em_reg_cdev_device_create:
		cdev_del(&p_driver_data->m_dev_reg);
	case em_char_cdev_add:
	case em_pci_reenable_device_val:
		unmaping_bars(p_driver_data);
		pci_clear_master(p_driver_data->m_dev_pci);
		pci_release_regions(p_driver_data->m_dev_pci);
	case em_pci_request_regions:
		pci_disable_device(p_driver_data->m_dev_pci);
	case em_pci_enable_device_mem:
	default:
		sl_debug("cherry_destructor is endding");
	}
}


void slnic_netdev_rx_handler(struct net_device *ndev)
{
	struct slk_net_priv *priv = netdev_priv(ndev);
	napi_schedule(&priv->m_sl_nic_napi);
}

static irqreturn_t poll_interrupt(int irq, void *dev_id/*, struct pt_regs *regs*/)
{
	int i = 0;
	struct cherry_kiwi_adapter *p_driver_data = (struct cherry_kiwi_adapter*)(dev_id);
	pci_disable_irq(p_driver_data);
	for (i = 0; i < MAC_PORT_NUM; ++i)
	{
		if (p_driver_data->ndev[i])
		{
			slnic_netdev_rx_handler(p_driver_data->ndev[i]);
		}

	}

	return IRQ_HANDLED;
}

static void slnic_set_time(struct cherry_kiwi_adapter *p_driver_data)
{
	struct timespec ts;
	getnstimeofday(&ts);
	///���������������ȥ
	WRITE_REG(p_driver_data->m_mem_base0, SYS_TIME_HMS, ts.tv_sec);
	WRITE_REG(p_driver_data->m_mem_base0, SYS_TIME_NS, ts.tv_nsec);
	WRITE_REG(p_driver_data->m_mem_base0, SYS_TIME_CTRL, 0x40000001);
}

static void slnic_set_time_rest(int slnic_count)
{
	struct timespec ts;
	getnstimeofday(&ts);
	int i = 0;
	for (i = 0;  i < slnic_count;  i++)
	{
		struct cherry_kiwi_adapter *p_driver_data = g_driver_data + i;
		///���������������ȥ
		WRITE_REG(p_driver_data->m_mem_base0, SYS_TIME_HMS, ts.tv_sec);
		WRITE_REG(p_driver_data->m_mem_base0, SYS_TIME_NS, ts.tv_nsec);
		WRITE_REG(p_driver_data->m_mem_base0, SYS_TIME_CTRL, 0x40000001);
	}

	
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
static void slnic_link_timer_callback(unsigned long data)
{
	int i;
	struct cherry_kiwi_adapter * p_driver_data = (struct cherry_kiwi_adapter*)(data);
	for (i = 0; i < SLNIC_MAX_PORTS; ++i)
		if (p_driver_data->ndev[i])
			slnic_netdev_check_link(p_driver_data->ndev[i]);

	mod_timer(&p_driver_data->m_link_timer, jiffies + HZ);
}

static void slnic_rectify_hwtime_callback(unsigned long data)
{
	struct cherry_kiwi_adapter * p_driver_data = (struct cherry_kiwi_adapter*)(data);
	slnic_set_time(p_driver_data);
	mod_timer(&p_driver_data->m_time_stamp, jiffies + HZ*3600);
}


#else
static void slnic_link_timer_callback(struct timer_list*t)
{
	int i;

	struct cherry_kiwi_adapter * p_driver_data = container_of(t, struct cherry_kiwi_adapter, m_link_timer);

	for (i = 0; i < SLNIC_MAX_PORTS; ++i)
		if (p_driver_data->ndev[i])
			slnic_netdev_check_link(p_driver_data->ndev[i]);

	mod_timer(&p_driver_data->m_link_timer, jiffies + HZ*3600);
}

static void slnic_rectify_hwtime_callback(struct timer_list*t)
{
	struct cherry_kiwi_adapter * p_driver_data = container_of(t, struct cherry_kiwi_adapter, m_time_stamp);
	slnic_set_time(p_driver_data);
	mod_timer(&p_driver_data->m_time_stamp, jiffies + HZ*3600);
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


static void sl_pci_remov(struct pci_dev* dev)
{
	struct cherry_kiwi_adapter *p_driver_data = pci_get_drvdata(dev);
	destructor(p_driver_data);
}


static int sl_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int						result;
	unsigned int			value;
	unsigned int			port_num;
	int						max_payload = 0;
	int						channel;

	static int register_minor = 1;
	static int stream_minor = 1;
	static int slnic_minor = 1;

	struct cherry_kiwi_adapter *p_driver_data = g_driver_data+slnic_count;
	memset(p_driver_data, 0, sizeof(struct cherry_kiwi_adapter));
	p_driver_data->m_id = slnic_count++;
	p_driver_data->m_dev_pci = pdev;
	p_driver_data->m_borad_port_num = SLNIC_MAX_PORTS;

	///初始化变量
	constructor(p_driver_data);

	result = pci_enable_device(pdev);
	if (result)
	{
		sl_error("pci_enable_device_mem failed: %d\n", result);
		p_driver_data->m_e_rls = em_pci_enable_device_mem;
		goto out;
	}

	result = pci_request_regions(pdev, SL_DRV_NAME);
	if (result)
	{
		sl_error("pci_request_regions failed: %d\n", result);
		p_driver_data->m_e_rls = em_pci_request_regions;
		goto out;
	}

	pci_set_drvdata(pdev, p_driver_data);
	pci_set_master(pdev);

	///�����豸
	{
		/*��pci�Ĵ���ӳ�䵽�ڴ�ռ�*/
		mapping_bars(p_driver_data);

		value = READ_REG(p_driver_data->m_mem_base0, SYS_STATUS);
		sl_debug("read status reg:%08x", value);

		pci_save_state(pdev);
		reset(p_driver_data);
		pci_restore_state(pdev);

		result = pci_reenable_device(pdev);
		if (result)
		{
			p_driver_data->m_e_rls = em_pci_reenable_device_val;
			sl_error("pci reenale device error\n");
			goto out;
		}

		pci_set_master(pdev);
		mapping_bars(p_driver_data);
	}

	//�����ַ��豸
	p_driver_data->m_reg_dev = MKDEV(KIWI_REG_CHAR_MAJOR, register_minor);
	cdev_init(&p_driver_data->m_dev_reg, &g_fop_control);

	p_driver_data->m_dev_reg.owner = THIS_MODULE;
	p_driver_data->m_dev_reg.ops = &g_fop_control;

	result = cdev_add(&p_driver_data->m_dev_reg, p_driver_data->m_reg_dev, 1);
	if (result)
	{
		sl_error("Error %d addint log\n", result);
		p_driver_data->m_e_rls = em_char_cdev_add;
		goto out;
	}

	p_driver_data->sys_device = device_create(slnic_class,&(pdev->dev), p_driver_data->m_reg_dev, NULL, "slnic/register%d", register_minor);
	if (IS_ERR(p_driver_data->sys_device))
	{
		sl_error("register device_create error [register_minor:%d]\n", register_minor);
		p_driver_data->m_e_rls = em_reg_cdev_device_create;
		goto out;
	}
	register_minor++;


	// ����������
	p_driver_data->m_stream_dev = MKDEV(KIWI_STREAM_CHAR_MAJOR, stream_minor);
	cdev_init(&p_driver_data->m_dev_stream, &g_fop_stream);
	p_driver_data->m_dev_stream.owner = THIS_MODULE;
	p_driver_data->m_dev_stream.ops = &g_fop_stream;

	result = cdev_add(&p_driver_data->m_dev_stream, p_driver_data->m_stream_dev, 1);
	if (result)
	{
		sl_error("Error %d addint quote\n", result);
		p_driver_data->m_e_rls = em_data_cdev_device_create;
		goto out;
	}

	p_driver_data->sys_device = device_create(slnic_class, &(pdev->dev), p_driver_data->m_stream_dev, NULL, "slnic/stream%d", stream_minor);
	if (IS_ERR(p_driver_data->sys_device))
	{
		sl_error("register device_create error [stream_minor:%d]\n", stream_minor);
		p_driver_data->m_e_rls = em_data_cdev_add;
		goto out;
	}
	stream_minor++;

	//slnic_char
	p_driver_data->m_slnic_dev = MKDEV(KIWI_SLNIC_CHAR_MAJOR, slnic_minor);
	cdev_init(&p_driver_data->m_dev_slnic, &g_fop_slnic);
	p_driver_data->m_dev_slnic.owner = THIS_MODULE;
	p_driver_data->m_dev_slnic.ops = &g_fop_slnic;

	result = cdev_add(&p_driver_data->m_dev_slnic, p_driver_data->m_slnic_dev, 1);
	if (result)
	{
		sl_error("Error %d addint quote\n",result);
		p_driver_data->m_e_rls = em_slnic_cdev_add;
		goto out;
	}

	p_driver_data->sys_device = device_create(slnic_class, &(pdev->dev), p_driver_data->m_slnic_dev, NULL, "slnic/slnic%d", slnic_minor);
	if (IS_ERR(p_driver_data->sys_device))
	{
		sl_error("register device_create error [slnic_minor:%d]\n", slnic_minor);
		p_driver_data->m_e_rls = em_slnic_cdev_add;
		goto out;
	}
	slnic_minor++;

//	memory��ʽ
	p_driver_data->m_tx_count.m_vis_addr = dma_alloc_coherent(&(pdev->dev), PAGE_SIZE, &p_driver_data->m_tx_count.m_dam_addr, GFP_KERNEL);
	if (!p_driver_data->m_tx_count.m_vis_addr)
	{
		p_driver_data->m_e_rls = em_dma_alloc_coherent;
		sl_error("dma_alloc_coherent\n");
		goto out;
	}

	*(volatile unsigned int*)(p_driver_data->m_tx_count.m_vis_addr) = 0;

	WRITE_REG(p_driver_data->m_mem_base0, SYS_TX_REGISTER, p_driver_data->m_tx_count.m_dam_addr);

	p_driver_data->m_tx_queue = slk_create_tx_mem_op_list(p_driver_data->m_mem_base11, p_driver_data->m_mem_base1, 16, p_driver_data->m_mem_base0, p_driver_data->m_dev_pci);
	if (!p_driver_data->m_tx_queue)
	{
		p_driver_data->m_e_rls = em_slk_create_tx_mem_op_list;
		sl_error("slk_create_tx_mem_op_list failed\n");
		goto out;
	}

	p_driver_data->m_rx_seqs = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!p_driver_data->m_rx_seqs)
	{
		p_driver_data->m_e_rls = em_m_rx_seqs;
		sl_error("m_rx_seqs kmalloc");
		goto out;
	}

	for (channel = 0; channel < MAX_CHANNEL_NUM; ++channel)
	{
		p_driver_data->m_rx_seqs[channel].m_info.m_read_seq = 0;
	}

	init_channel_new_node_param(p_driver_data->m_rx_channel, 0, SL_NIC_RX_CHAN0_NODE_SIZE, SL_NIC_RX_CHAN0_NODE_NUM);
	init_channel_new_node_param(p_driver_data->m_rx_channel, 1, SL_NIC_RX_CHAN1_NODE_SIZE, SL_NIC_RX_CHAN1_NODE_NUM);
	init_channel_new_node_param(p_driver_data->m_rx_channel, 2, SL_NIC_RX_CHAN2_NODE_SIZE, SL_NIC_RX_CHAN2_NODE_NUM);
	init_channel_new_node_param(p_driver_data->m_rx_channel, 3, SL_NIC_RX_CHAN3_NODE_SIZE, SL_NIC_RX_CHAN3_NODE_NUM);
	init_channel_new_node_param(p_driver_data->m_rx_channel, 4, SL_NIC_RX_CHAN4_NODE_SIZE, SL_NIC_RX_CHAN4_NODE_NUM);
	init_channel_new_node_param(p_driver_data->m_rx_channel, 5, SL_NIC_RX_CHAN5_NODE_SIZE, SL_NIC_RX_CHAN5_NODE_NUM);
	init_channel_new_node_param(p_driver_data->m_rx_channel, 6, SL_NIC_RX_CHAN6_NODE_SIZE, SL_NIC_RX_CHAN6_NODE_NUM);
	init_channel_new_node_param(p_driver_data->m_rx_channel, 7, SL_NIC_RX_CHAN7_NODE_SIZE, SL_NIC_RX_CHAN7_NODE_NUM);

	init_channel_new_param(p_driver_data->m_rx_channel,
		p_driver_data->m_dev_pci,
		p_driver_data->m_mem_base0,
		8);

	if (slk_rx_channel_new_create(&p_driver_data->m_rx_channel))
	{
		p_driver_data->m_e_rls = em_slk_rx_channel_new_create;
		sl_error("create rx mmap error!");
		goto out;
	}

	//�����豸�������ע��
	for (port_num=0 ; port_num < p_driver_data->m_borad_port_num; port_num++)
	{
		result = slnic_netdev_alloc(&p_driver_data->ndev[port_num],port_num, p_driver_data);
		if (!result)
		{
			p_driver_data->m_e_rls = em_slnic_netdev_alloc;
			sl_error("slnic_netdev_alloc is failed\n");
			goto out;
		}
	}

	//Set up timer to poll link status
	SLK_CLCOK_SETUP(&p_driver_data->m_link_timer, slnic_link_timer_callback, (unsigned long)(p_driver_data));
	mod_timer(&p_driver_data->m_link_timer, jiffies + HZ);

	 //Ӳ��ʱ�������
	SLK_CLCOK_SETUP(&p_driver_data->m_time_stamp, slnic_rectify_hwtime_callback, (unsigned long)(p_driver_data));
	mod_timer(&p_driver_data->m_time_stamp, jiffies + HZ*3600);
	
	result = pci_enable_msi(pdev);
	if (result)
	{
		sl_error("pci_enable_msi error %d\n", result);
	}

	result = request_irq(pdev->irq, poll_interrupt, IRQF_SHARED, KIWI_POLL_DRIVER_NAME, p_driver_data);
	if (result)
	{
		sl_error("short: can't get assigned irq %i\n", pdev->irq);
	}

#ifndef SLK_QUEM_EVN
	pci_set_payload(pdev);

	 //����pcie�����ֵ
	max_payload = read_pcie_parent_maxpayload(pdev);
	if (max_payload == 0 || max_payload == 1)
	{
		WRITE_REG(p_driver_data->m_mem_base0, REG_PCIE_CTRL_ADDR, max_payload);
	}
#endif

	///����ʱ��
	

	pci_enable_irq(p_driver_data);
	sl_debug("board_num%d FPGA DEVICE NUME NODE:%d,CURRENT CPU NODE ID:%d,CURRENT CPU ID:%d\n", slnic_count,(&p_driver_data->m_dev_pci->dev)->numa_node, numa_node_id(), get_cpu());
	slnic_count++;

	
	slnic_set_time_rest(slnic_count);
	return result;

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

	//�����豸��
	slnic_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(slnic_class)) {
		sl_error("slnic_class class_create [slnic_count:%d]\n", slnic_count);
		return -1;
	}

	return pci_register_driver(&g_hw_pci);
}

static void __exit sl_nic_exit_module(void)
{
	sl_debug("net dev out");
	pci_unregister_driver(&g_hw_pci);
	class_destroy(slnic_class);
}


module_init(sl_nic_init_module);
module_exit(sl_nic_exit_module);

MODULE_LICENSE("GPL");
