#ifndef _SL_NET_DRIVE_H_
#define _SL_NET_DRIVE_H_
#include <linux/netdevice.h>
#include <linux/spinlock_types.h>
#include <linux/mutex.h>
#include "sl_nic_define.h"
#include "slnic_net_ethtool.h"

#define MAC_PORT_NUM 2
#define SL_MAC0_H_REG		(0x4000)
#define SL_MAC0_L_REG		(0x4008)
#define SL_MAC0_CONFIG_EN	(0x4048)
#define SLNIC_PALOAD_SIZE	(2016)    
#define RX_CHL_NODE			(511)
#define SW_PKT_CUR_MEM_OFFSET (8)

#define			SW_PKT_REV_OFFSET0			(0x2118)


struct rx_chunk
{
	struct hardware_to_driver_head_info info;
	char pay_load[SLNIC_PALOAD_SIZE];
};

struct slnic_netdev_rx
{
	volatile struct rx_chunk* buffer;
	unsigned int			  m_next_chunk;
	unsigned short			  m_chunk_count;
};

struct sw_rx_channel_config
{
	unsigned int node_size;
	unsigned int node_num;
};

struct netdev_data
{
	char*						m_p_poll_data;
	int							offset;
};

struct slk_net_priv
{
	struct net_device_stats		stats;
	int							status;
	struct sk_buff				*skb;
	struct net_device			*ndev;
	unsigned long				bar0_addr;
	int							port;
	struct sw_rx_channel_config stu_rx_channel_config;
	struct net_device_stats		stu;
	long						mac_reg_h;
	long						mac_reg_l;
	long						mac_reg_en;
	struct mutex				m_tx_mutex;
	//spinlock_t          tx_lock;
	struct napi_struct			m_sl_nic_napi;
	struct slnic_netdev_rx		m_rx;
	struct pci_dev				*pci_dev;
	unsigned int				regs_len;
	u32							msg_enable;
	struct fpga_hw				hw;
	bool						fc_autoneg;
	u32							itr_setting;
	u16							tx_ring_count;
	u16							rx_ring_count;
	int							b_napi_running;
	int							m_rx_enable;
	void*						buff;
	struct netdev_data			m_p_data;
};

int slnic_rx_chunk_compare(struct slnic_netdev_rx* prx);
int slnic_netdev_alloc(struct net_device **ndev_ptr, unsigned int port_num, unsigned long bar0_addr);
void slk_destory_netdev(void);
void slnic_netdev_check_link(struct net_device *ndev);
#endif
