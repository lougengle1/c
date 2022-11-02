#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/pci.h>
#include <linux/in.h>
#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */
#include <linux/tcp.h>         /* struct tcphdr */
#include <linux/skbuff.h>
#include <linux/in6.h>
#include <asm/checksum.h>
#include <linux/notifier.h>
#include <linux/inetdevice.h>
#include <linux/etherdevice.h>
#include "sl_nic_netdev.h"
#include "sl_nic_define.h"
#include "slk_tools/slk_define.h"
#include "slk_tools/slk_rx_chan.h"
#include "user_irq_define.h"
#include <linux/byteorder/generic.h>
#include "slnic_net_ethtool.h"
#include "sl_nic_driver.h"
#include <linux/ethtool.h>
#include <linux/version.h>


static void sl_nic_set_mac_regs(struct net_device *dev)
{
	unsigned mac_h = 0;
	unsigned mac_l = 0;

	unsigned char *dev_addr = dev->dev_addr;
	struct slk_net_priv *priv = netdev_priv(dev);

	mac_l |= dev_addr[5] << 0 * 8;
	mac_l |= dev_addr[4] << 1 * 8;
	mac_l |= dev_addr[3] << 2 * 8;
	mac_l |= dev_addr[2] << 3 * 8;

	mac_h |= dev_addr[1] << 0 * 8;
	mac_h |= dev_addr[0] << 1 * 8;

	WRITE_REG(priv->bar0_addr, priv->mac_reg_h, mac_h);
	WRITE_REG(priv->bar0_addr, priv->mac_reg_l, mac_l);
	WRITE_REG(priv->bar0_addr, priv->mac_reg_en, 1);

}

static int slnic_net_open(struct net_device *dev)
{
	struct slk_net_priv *priv = netdev_priv(dev);
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	sl_nic_set_mac_regs(dev);

	priv->m_rx_enable = 1;
	//napi_enable(&priv->m_sl_nic_napi);
	netif_start_queue(dev);
	
	return 0;
}

static int slnic_net_stop(struct net_device *dev)
{
	struct slk_net_priv *priv = netdev_priv(dev);
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	priv->m_rx_enable = 0;
	netif_stop_queue(dev); /* can't transmit any more */
	//napi_disable(&priv->m_sl_nic_napi);

	return 0;
}

static int slnic_net_config(struct net_device *dev, struct ifmap *map)
{
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	if (dev->flags & IFF_UP) /* can't act on a running interface */
		return -EBUSY;

	/* Don't allow changing the I/O address */
	if (map->base_addr != dev->base_addr) {
		sl_debug(KERN_WARNING "snull: Can't change I/O address\n");
		return -EOPNOTSUPP;
	}

	/* Allow changing the IRQ */
	if (map->irq != dev->irq) {
		dev->irq = map->irq;
		/* request_irq() is delayed to open-time */
	}

	/* ignore other fields */
	return 0;
}

static int slnic_net_set_mac_address(struct net_device *dev, void *addr)
{
	int ret;
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	ret = eth_mac_addr(dev, addr);

	sl_nic_set_mac_regs(dev);

	return ret;
}


static int stream_move_tx_node(struct cherry_kiwi_adapter * p_driver_data)
{
	slk_tx_mem_op_list_ready(p_driver_data->m_tx_queue);

	return 0;
}

static void check_recover(struct cherry_kiwi_adapter * p_driver_data)
{
	/// 获取dma的值
	p_driver_data->tx_dma_count = *(volatile unsigned int*)(p_driver_data->m_tx_count.m_vis_addr);

	/// 判断dma的值和tx queue的值是否相同
	while (p_driver_data->m_tx_queue->counter != p_driver_data->tx_dma_count)
		
	{
		/// 移动节点
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

static int slnic_net_tx(struct sk_buff *skb, struct net_device *dev)
{
	int err;
	unsigned long flags;
	struct slk_net_priv *priv = netdev_priv(dev);

	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	//void *buff = kmalloc(sizeof(struct driver_to_hardware_head) + skb->len, GFP_KERNEL);
	struct driver_to_hardware_head* p_head = (struct driver_to_hardware_head*)(priv->buff);

	/*在最开始申请网络设备的时候保存通道号*/
	p_head->m_channel_id = priv->port;
	p_head->m_payload_lenth = ntohs(skb->len);

	//sl_debug("[priv->port %x] [skb->len %x] [m_pay_load_lenth %x]\n", priv->port, skb->len, p_head->m_payload_lenth);

	if (skb->len == 0)
	{
		dev_kfree_skb(skb);
		priv->stats.tx_dropped++;
	}

	memcpy(priv->buff + sizeof(struct driver_to_hardware_head), skb->data, skb->len);

	spin_lock_irqsave(&priv->m_p_slnic_data->m_tx_lock, flags);

	check_recover(priv->m_p_slnic_data);
	err = slk_tx_mem_op_list_write((char*)priv->buff, sizeof(struct driver_to_hardware_head)+skb->len, priv->m_p_slnic_data->m_tx_queue);
	if (err < 0)
	{
		priv->stats.rx_errors++;
		sl_error("tx dma:%08x,tx count:%08x,tx send count:%08x",
			priv->m_p_slnic_data->tx_dma_count,
			priv->m_p_slnic_data->m_tx_queue->counter,
			priv->m_p_slnic_data->m_tx_queue->snd_counter
		);
	}

	spin_unlock_irqrestore(&priv->m_p_slnic_data->m_tx_lock, flags);

	priv->stats.tx_packets++;
	priv->stats.tx_bytes += skb->len;

	dev_kfree_skb(skb);
	return 0; /* Our simple device can not fail */
}

static int slnic_net_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static struct net_device_stats *slnic_net_stats(struct net_device *dev)
{
	struct slk_net_priv *priv = netdev_priv(dev);

	return &priv->stats;
}

static int slnic_net_change_mtu(struct net_device *dev, int new_mtu)
{
	if ((new_mtu < 68) || (new_mtu > 1500))
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0; /* success */
}

static void slnic_net_tx_timeout(struct net_device *dev)
{
	sl_debug(KERN_ALERT "fpga_net_tx_timeout");
	netif_wake_queue(dev);                  /// 唤醒队列
	return;
}

static struct net_device_ops sl_nic_ndos = {
	.ndo_open = slnic_net_open,
	.ndo_stop = slnic_net_stop,
	.ndo_set_config = slnic_net_config,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_mac_address = slnic_net_set_mac_address,
	.ndo_start_xmit = slnic_net_tx,
	.ndo_do_ioctl = slnic_net_ioctl,
	.ndo_get_stats = slnic_net_stats,
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,10,0)
	.ndo_change_mtu = slnic_net_change_mtu,
#endif
	.ndo_tx_timeout = slnic_net_tx_timeout,
};

static int slnic_rx_chunk_inplace(struct slnic_netdev_rx* p_rx,struct slk_net_priv*p_slk_priv)
{
	int hard_counter = p_rx->buffer[p_rx->m_next_chunk].info.m_chunk_count;
	int soft_counter = p_rx->m_chunk_count;
	int mf_and_ff = p_rx->buffer[p_rx->m_next_chunk].info.m_fragment_flag;
	int payload_lenth = ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);

	//保护模式，防止硬件bug给的长度不对，导致系统奔溃
	if (p_slk_priv->m_p_data.offset + payload_lenth > PAGE_SIZE*16)
	{
		p_slk_priv->m_p_data.offset = 0;
		return 0;
		sl_error("[p_slk_priv->m_p_data.offset:%d] [payload_lenth:%d]", p_slk_priv->m_p_data.offset, payload_lenth);
	}


	if (hard_counter == soft_counter)
	{
		sl_debug("[m_fragment_flag :%d]\n",p_rx->buffer[p_rx->m_next_chunk].info.m_fragment_flag);
		/*数据来了，m_chunk_cout硬件上传是一直计数累加的*/
		//解析mm和mf的字段
		if (mf_and_ff == 3)
		{
			memcpy(p_slk_priv->m_p_data.m_p_poll_data + p_slk_priv->m_p_data.offset, p_rx->buffer[p_rx->m_next_chunk].pay_load, payload_lenth);
			p_slk_priv->m_p_data.offset = p_slk_priv->m_p_data.offset +ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);	
			return 0;
		}
		else if(mf_and_ff == 2)
		{
			memcpy(p_slk_priv->m_p_data.m_p_poll_data + p_slk_priv->m_p_data.offset, p_rx->buffer[p_rx->m_next_chunk].pay_load, payload_lenth);
			p_slk_priv->m_p_data.offset = p_slk_priv->m_p_data.offset + ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);
			return p_slk_priv->m_p_data.offset;
		}
		else if(mf_and_ff ==0)
		{
			memcpy(p_slk_priv->m_p_data.m_p_poll_data, p_rx->buffer[p_rx->m_next_chunk].pay_load, payload_lenth);
			p_slk_priv->m_p_data.offset = ntohs(p_rx->buffer[p_rx->m_next_chunk].info.payload_lenth);
			return p_slk_priv->m_p_data.offset;
		}
		else 
		{
			sl_error("mf_and_ff is error %d", mf_and_ff);
			return -1;
		}
	}
	else if (hard_counter != soft_counter)
	{
		return -1;
	}
}

int slnic_rx_chunk_compare(struct slnic_netdev_rx* p_rx)
{
	int hard_counter = p_rx->buffer[p_rx->m_next_chunk].info.m_chunk_count;
	int soft_counter = p_rx->m_chunk_count;
	if (hard_counter == soft_counter)
	{
		/*有数据到来*/
		return 1;
	}
	else if (hard_counter != soft_counter)
	{
		/*没有数据*/
		return 0;
	}
	
	return 0;
}

static int slnic_net_poll(struct napi_struct *napi, int budget)
{
	struct slk_net_priv *priv = container_of(napi, struct slk_net_priv, m_sl_nic_napi);
	struct slnic_netdev_rx *p_rx = &priv->m_rx;
	struct net_device *ndev = priv->ndev;
	//char* ptr = NULL;
	int received = 0;
	int pkt_size;
	int ptr_count = 0;
	unsigned long long timensval;

	while ((received < budget) && (priv->b_napi_running == 1) && (priv->m_rx_enable == 1))
	{
		// -1 代表没数据和包中的md有问题，0 代表要组包，组包但是计数要增加，其他代表数据的长度
		pkt_size = slnic_rx_chunk_inplace(p_rx, priv);
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

		priv->skb = netdev_alloc_skb(ndev,pkt_size+2);
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
			WRITE_REG(priv->m_p_slnic_data->m_mem_base0, SW_PKT_CUR_MEM_OFF0 + SW_PKT_CUR_MEM_OFFSET * priv->port,
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

	napi_complete(napi);
	napi_reschedule(napi);
	return received;
}

//static void slnic_net_setup_led(void)
//{
//	sl_debug(KERN_ALERT "fpga set up led! \r\n");
//}
//
//static void slnic_net_led_on(void)
//{
//	sl_debug(KERN_ALERT "fpga set led on! \r\n");
//}
//
//static void slnic_net_led_off(void)
//{
//	sl_debug(KERN_ALERT "fpga set led off! \r\n");
//}
//
//static void slnic_net_cleanup_led(void)
//{
//	sl_debug(KERN_ALERT "fpag clean up led! \r\n");
//}
//
//static int slnic_net__set_phys_id(struct net_device *netdev,
//			     enum ethtool_phys_id_state state)
//{
//	switch (state) {
//	case ETHTOOL_ID_ACTIVE:
//		slnic_net_setup_led();
//		return 2;
//
//	case ETHTOOL_ID_ON:
//		slnic_net_led_on();
//		break;
//
//	case ETHTOOL_ID_OFF:
//		slnic_net_led_off();
//		break;
//
//	case ETHTOOL_ID_INACTIVE:
//		slnic_net_cleanup_led();
//		break;
//	}
//
//	return 0;
//}
//
//int slnic_op_get_ts_info(struct net_device *dev, struct ethtool_ts_info *info)
//{
//	return 0;
//}

#if defined(ETHTOOL_GET_TS_INFO) && LINUX_VERSION_CODE < KERNEL_VERSION(3, 0, 0)
/* RedHat 2.6.x backports place get_ts_info in ethtool_ops_ext */
static struct ethtool_ops_ext exanic_ethtool_ops_ext = {
    .size                   = sizeof(struct ethtool_ops_ext),
   // .get_ts_info            = slnic_op_get_ts_info,
    .set_phys_id 	    = slnic_net__set_phys_id,
};
#define SET_ETHTOOL_OPS_EXT(ndev, ops) set_ethtool_ops_ext(ndev, ops)
#else
#define SET_ETHTOOL_OPS_EXT(ndev, ops)
#endif


static void slnic_net_init(struct net_device* dev)
{
	struct slk_net_priv* priv = netdev_priv(dev);

	int timeout = 5;
	ether_setup(dev); /* assign some of the fields */

	dev->netdev_ops = &sl_nic_ndos;
	SET_ETHTOOL_OPS_EXT(dev, &exanic_ethtool_ops_ext);
	//	dev->ethtool_ops = &fpga_devices_ethtool_ops;

	dev->watchdog_timeo = timeout;

	dev->vlan_features = dev->features;

	netif_napi_add(dev, &priv->m_sl_nic_napi, slnic_net_poll, 64);
}

void slnic_netdev_check_link(struct net_device *ndev)
{
	struct slk_net_priv *priv = netdev_priv(ndev);
	uint32_t reg;

	reg = READ_REG(priv->m_p_slnic_data->m_mem_base0, SYS_STATUS);

	if (priv->port == 0)
	{
		if (reg & 0x1)
		{
			if (!netif_carrier_ok(ndev))
			{
				netif_carrier_on(ndev);
			}
		}
		else if (netif_carrier_ok(ndev))
		{

			netif_carrier_off(ndev);
		}
	}
	else if (priv->port == 1)
	{
		if (reg & 0x2)
		{
			if (!netif_carrier_ok(ndev))
			{
				netif_carrier_on(ndev);
			}
		}
		else if (netif_carrier_ok(ndev))
		{
			netif_carrier_off(ndev);
		}
	}
}

static int slnic_get_mac_addr_regs(struct net_device *ndev,int port_num, struct cherry_kiwi_adapter *p_driver_data)
{
	uint32_t mac_h;
	uint32_t mac_l;
	
	struct slk_net_priv *priv = netdev_priv(ndev);

	mac_h = READ_REG(p_driver_data->m_mem_base0, MAC0_ADDR_H + port_num *MAC_OFFSET);
	mac_l = READ_REG(p_driver_data->m_mem_base0, MAC0_ADDR_L + port_num *MAC_OFFSET);

	//如果读到的时候空，就设置成默认值
	if ((mac_h == 0) && (mac_l == 0))
	{
		memcpy(ndev->dev_addr, "netma", 5);
		ndev->dev_addr[5] = p_driver_data->m_id +port_num;
	}
	else
	{
		ndev->dev_addr[0] = mac_h >> 8;
		ndev->dev_addr[1] = mac_h;
		ndev->dev_addr[2] = mac_l >> 24;
		ndev->dev_addr[3] = mac_l >> 16;
		ndev->dev_addr[4] = mac_l >> 8;
		ndev->dev_addr[5] = mac_l;
	}

	return 0;
}

int slnic_netdev_alloc(struct net_device **ndev_ptr,unsigned int port_num,struct cherry_kiwi_adapter *p_driver_data)
{
	int err;
	struct slk_net_priv *priv;
	struct net_device *ndev;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)
	ndev = alloc_netdev(sizeof(struct slk_net_priv), "slnic%d", 0, slnic_net_init);
#else
	
	ndev = alloc_netdev(sizeof(struct slk_net_priv), "slnic%d", slnic_net_init);
#endif
	if (!ndev)
	{
		err = -ENOMEM;
		sl_debug("alloc_netdev is failed\n");
		goto err_alloc;
	}

	priv = netdev_priv(ndev);
	priv->m_p_slnic_data = p_driver_data;
	priv->status = 0;
	priv->skb = NULL;
	priv->ndev = ndev;
	priv->bar0_addr = p_driver_data->m_mem_base0;
	
	priv->port = port_num;

	if (priv->port == 0)
	{
		priv->stu_rx_channel_config.node_num = SL_NIC_RX_CHAN0_NODE_NUM;
		priv->stu_rx_channel_config.node_size = SL_NIC_RX_CHAN0_NODE_SIZE;
	}
	else if (priv->port == 1)
	{
		priv->stu_rx_channel_config.node_num = SL_NIC_RX_CHAN1_NODE_NUM;
		priv->stu_rx_channel_config.node_size = SL_NIC_RX_CHAN1_NODE_SIZE;
	}

	priv->mac_reg_h = SL_MAC0_H_REG + (0x0100) * port_num;
	priv->mac_reg_l = SL_MAC0_L_REG + (0x0100) * port_num;
	priv->mac_reg_en = SL_MAC0_CONFIG_EN + (0x0100) * port_num;
	priv->m_rx.buffer = NULL;
	priv->m_rx.m_chunk_count = 1;
	priv->m_rx.m_next_chunk = 0;
	priv->pci_dev = p_driver_data->m_dev_pci;
	priv->regs_len = pci_resource_len(p_driver_data->m_dev_pci, 0);
	priv->msg_enable = netif_msg_init(p_driver_data->m_debug, DEFAULT_MSG_ENABLE);
	priv->tx_ring_count = SLK_FPGA_DEFAULT_TXD;
	priv->rx_ring_count = SLK_FPGA_DEFAULT_RXD;
	priv->b_napi_running =1;
	priv->m_rx_enable = 0;
	priv->m_rx.buffer = p_driver_data->m_rx_channel.m_dma_chan[priv->port].m_virt_addr_start;
	priv->buff = kmalloc(PAGE_SIZE, GFP_KERNEL);
	priv->m_p_data.m_p_poll_data = (char *)kmalloc(PAGE_SIZE*16, GFP_KERNEL);
	priv->m_p_data.offset = 0;
	*ndev_ptr = ndev;
	napi_enable(&priv->m_sl_nic_napi);


	///读取mac寄存器地址并且设置mac地址	
	slnic_get_mac_addr_regs(ndev, port_num, p_driver_data);
	slnic_net_set_ethtool_ops(ndev);

	if((err = register_netdev(ndev)))
	{
		err = -ENOMEM;
		sl_debug("register_netdev is failed \n");
		goto err_register;
	}

	slnic_netdev_check_link(ndev);

	return 1;

err_register:
	free_netdev(ndev);
err_alloc:
	return err;
}


void slk_destory_netdev(struct cherry_kiwi_adapter* p_driver_data)
{
	int i = 0;
	for (i = 0; i <p_driver_data->m_borad_port_num; i++)
	{
		struct slk_net_priv *priv = netdev_priv(p_driver_data->ndev[i]);
		priv->b_napi_running = 0;
		kfree(priv->m_p_data.m_p_poll_data);
		msleep(1000);

		kfree(priv->buff);

		napi_disable(&priv->m_sl_nic_napi);

		unregister_netdev(p_driver_data->ndev[i]);

		netif_napi_del(&priv->m_sl_nic_napi);

		free_netdev(p_driver_data->ndev[i]);
	}
}

