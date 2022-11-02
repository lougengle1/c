#ifndef WTM_DEFINE_H_20121120
#define WTM_DEFINE_H_20121120

#include "slk_tools/slk_define.h"
#include "slk_tools/slk_queue.h"
#include "slk_tools/slk_gfp.h"
#include "slk_tools/slk_struct.h"
#include "slk_tools/slk_rx_chan.h"
#include "slk_tools/slk_hw_mem_op.h"
#include "slk_tools/slk_log.h"
#include "slk_tools/slk_register.h"
#include "slk_tools/slk_function.h"
#include "slk_tools/slk_dev_status.h"
#include "slk_tools/slk_rx_chan_new.h"
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/spinlock_types.h>
#include <linux/types.h>



#define			KIWI_POLL_DRIVER_NAME		"sl_nic_driver"
#define			KIWI_POLL_DEVICE_ID			0x5201
#define			KIWI_POLL_VENDOR_ID			0x8009

#define			KIWI_POLL_DEVICE_ID_NF5		0x0210
#define			KIWI_POLL_VENDOR_ID_NF5		0x8009

#define			KIWI_POLL_DEVICE_ID_NF10	0x0003
#define			KIWI_POLL_VENDOR_ID_NF10	0x8009

#define			SL_DRV_NAME					"slnic"
#define			SLNIC_MAX_PORTS				2
#define			KIWI_REG_CHAR_MAJOR			224
#define			KIWI_REG_CHAR_MINOR			1
#define			KIWI_STREAM_CHAR_MAJOR		225
#define			KIWI_STREAM_CHAR_MINOR		1
#define			KIWI_SLNIC_CHAR_MAJOR		226
#define			KIWI_SLNIC_CHAR_MINOR		1
#define			DEFAULT_MSG_ENABLE			(NETIF_MSG_DRV|NETIF_MSG_PROBE|NETIF_MSG_LINK)
#define			CTL_COMMUNICATION			'M'
#define			DEVICE_NAME					"slnic"



#define			SL_NIC_RX_CHAN0_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN0_NODE_NUM		(512)
#define			SL_NIC_RX_CHAN1_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN1_NODE_NUM		(512)
#define			SL_NIC_RX_CHAN2_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN2_NODE_NUM		(2048)
#define			SL_NIC_RX_CHAN3_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN3_NODE_NUM		(2048)
#define			SL_NIC_RX_CHAN4_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN4_NODE_NUM		(2048)
#define			SL_NIC_RX_CHAN5_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN5_NODE_NUM		(2048)
#define			SL_NIC_RX_CHAN6_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN6_NODE_NUM		(2048)
#define			SL_NIC_RX_CHAN7_NODE_SIZE		(2048)
#define			SL_NIC_RX_CHAN7_NODE_NUM		(2048)

//#define			CHR_VL_LOG_LEN					(1*1024*1024)
		
#define			SLNIC_MAX_NUM_DEVICES			(4)

struct slk_addr_t
{
	long			m_offset;
	long			m_lenth;

	long			m_channel;
	long			m_dma_addr;

	int				m_node_size;
	int				m_node_num;
};

//struct slk_session_netaddr
//{
//	union
//	{
//		unsigned int				m_ipv4;
//		unsigned char				m_mac[6];
//		char						m_padding[8];
//	};
//};

#define MAX_CHANNEL_SESSION			16
#define MAX_CHANNEL_NUN				8

struct slk_nic_session
{
	char	 m_smac[20];						//源mac地址
	char	 m_dmac[20];						//目标mac地址
	char	 m_sip[20];							//源ip地址
	char	 m_dip[20];							//目标地址
	unsigned int	 m_sport;					//源端口
	unsigned int	 m_dport;					//目标端口
	int				 m_net_type;				//网络传输类型tcp/udp
	int				 m_mac;						//mac
};

struct slk_channel_config
{
	struct slk_nic_session		m_session_config[MAX_CHANNEL_SESSION];		//一共16个session，从0到15
	unsigned short				m_channel;									//通道号
	unsigned short				m_cpu;										//-1为不绑核,其他为绑定的cpu核
	unsigned int				m_session_num;								//session号
};

enum {
	SLK_IOCTL_GET_RX_ADDR = 100,
	SLK_IOCTL_GET_BASE0_ADDR,
	SLK_IOCTL_GET_BASE1_ADDR,
	SLK_IOCTL_GET_RXSEQ_ADDR,
	SLK_IOCTL_GET_TX_ADDR,

	SLK_IOCTL_CONFIG_CHANNEL = 150,

	SLK_IOCTL_CONTROL_RESET = 200,
	SLK_IOCTL_VERSION = 250,
};

enum em_resource_rls_ctrl
{
	em_rls_ctrl_init,
	em_pci_enable_device_mem,
	em_pci_request_regions,
	em_pci_reenable_device_val,
	em_char_cdev_add,
	em_reg_cdev_device_create,
	em_data_cdev_add,
	em_data_cdev_device_create,
	em_slnic_cdev_add,
	em_slnic_cdev_create_device,
	em_dma_alloc_coherent,
	em_slk_create_tx_mem_op_list,
	em_m_rx_seqs,
	em_slk_rx_channel_new_create,
	em_slnic_netdev_alloc,
	em_kthread_create,
	em_rls_ctrl_end,
};

struct slnic_device_info
{
	unsigned int	port;
	int				rx_enable;
	unsigned long	rx_packets;
	unsigned long	tx_packets;
	unsigned long	rx_bytes;
	unsigned long	tx_bytes;
	unsigned long	rx_errors;
	unsigned long	tx_errors;
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
};

#pragma pack(push, 1)
struct driver_to_hardware_head
{
	int				m_mode : 8;
	int				m_resv1 : 8;
	int				m_channel_id : 8;
	unsigned long	m_user_data;
	int				m_resv2 : 24;
	unsigned short	m_payload_lenth;
	unsigned long	resv3;
	unsigned long	resv4;
};

/// 硬件到驱动的头
struct hardware_to_driver_head_info
{
	unsigned int				m_mac_id : 8;
	unsigned int				m_channel_id : 8;
	unsigned short				m_chunk_count : 16;
	unsigned int				m_systemTime_s;
	unsigned int				m_systemTime_ns;
	unsigned int				m_rev1 : 8;
	unsigned int				m_ip_port : 8;
	unsigned int				payload_lenth : 16;
	unsigned int				m_fragment_flag : 8;
	unsigned int				m_fragment_offset : 16;
	unsigned int				m_rev2 : 8;
	unsigned int				m_rev3;
	unsigned long long			m_rev4;
};

#pragma pack(pop)

struct slk_offset_t
{
	long						m_off;
	long						m_len;
	int							m_node_size;
	int							m_node_num;
};

struct coher_dma
{
	dma_addr_t			m_dam_addr;
	void*				m_vis_addr;
	int					m_i_len;
};

#define SYS_CACHE_LINE_SIZE			64
struct channel_read_info
{
	unsigned short			m_read_seq;
};

struct channel_read_info_block
{
	union
	{
		struct channel_read_info m_info;
		char 					 m_payload[SYS_CACHE_LINE_SIZE];
	};
};

struct cherry_kiwi_adapter
{
	struct pci_dev*					m_dev_pci;			///< PCI设备
	enum em_resource_rls_ctrl		m_e_rls;                        ///卸载资源控制
	unsigned long					m_mem_base0;		///< PCI设备中6个寄存器的第一个寄存器
	unsigned long					m_mem_base1;		///< PCI设备中6个寄存器的第二个寄存器
	unsigned long					m_mem_base11;		///< PCI设备中6个寄存器的第二个内存操作
	unsigned long					m_mem_base2;		///< PCI设备中6个寄存器的第三个寄存器
	struct task_struct*				deal_data_task;		
	struct net_device*				ndev[SLNIC_MAX_PORTS]; ///<数据从通道0和通道1上传,走内核协议栈
	struct slk_tx_mem_op_list*		m_tx_queue;
	struct cdev						m_dev_reg;
	struct cdev						m_dev_stream;
	struct cdev						m_dev_slnic;
	struct slk_rx_channel_new		m_rx_channel;
	struct slk_rx_mmap_head*		m_rx_chan_head;		/// 头部基本信息
	uint32_t						tx_dma_count;
	int								m_debug;
	struct semaphore				m_event_sem;		///<行情信号锁
	int								m_link_status;
	struct coher_dma				m_tx_count;			///< tx计数器
	struct timer_list				m_link_timer;
	struct timer_list				m_time_stamp;
	spinlock_t						m_tx_lock;
	unsigned char					channel_status[8];  ///最低位为1代表已经占用，不许别的程序使用
	struct channel_read_info_block *m_rx_seqs;
	unsigned int					m_id;				///第几个板卡
	struct device					*sys_device;
	dev_t							m_reg_dev ;
	dev_t							m_stream_dev;
	dev_t							m_slnic_dev;
	unsigned int					m_borad_port_num;	//每张板卡支持的网口数量
};

#define MAP_BASE0_OFFSET	(0)
#define MAP_BASE0_MAPLEN	(0x8000)
#define MAP_BASE1_OFFSET	(MAP_BASE0_OFFSET + MAP_BASE0_MAPLEN)
#define MAP_BASE1_MAPLEN	(0x8000)

#define MAP_TXDAT_OFFSET	(MAP_BASE1_OFFSET + MAP_BASE1_MAPLEN)
#define MAP_TXDAT_MAPLEN	(0x8000)
#define MAP_RXSEQ_OFFSET	(MAP_TXDAT_OFFSET + MAP_TXDAT_MAPLEN)
#define MAP_RXSEQ_MAPLEN	(PAGE_SIZE)


#define MAP_CHN_0_MAPLEN	((SL_NIC_RX_CHAN0_NODE_SIZE * SL_NIC_RX_CHAN0_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define MAP_CHN_1_MAPLEN	((SL_NIC_RX_CHAN1_NODE_SIZE * SL_NIC_RX_CHAN1_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define MAP_CHN_2_MAPLEN	((SL_NIC_RX_CHAN2_NODE_SIZE * SL_NIC_RX_CHAN2_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define MAP_CHN_3_MAPLEN	((SL_NIC_RX_CHAN3_NODE_SIZE * SL_NIC_RX_CHAN3_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define MAP_CHN_4_MAPLEN	((SL_NIC_RX_CHAN4_NODE_SIZE * SL_NIC_RX_CHAN4_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define MAP_CHN_5_MAPLEN	((SL_NIC_RX_CHAN5_NODE_SIZE * SL_NIC_RX_CHAN5_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define MAP_CHN_6_MAPLEN	((SL_NIC_RX_CHAN6_NODE_SIZE * SL_NIC_RX_CHAN6_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))
#define MAP_CHN_7_MAPLEN	((SL_NIC_RX_CHAN7_NODE_SIZE * SL_NIC_RX_CHAN7_NODE_NUM /*+ 4096 */+ PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))

#define MAP_CHN_0_OFFSET	(MAP_RXSEQ_OFFSET + MAP_RXSEQ_MAPLEN)
#define MAP_CHN_1_OFFSET	(MAP_CHN_0_OFFSET + MAP_CHN_0_MAPLEN)
#define MAP_CHN_2_OFFSET	(MAP_CHN_1_OFFSET + MAP_CHN_1_MAPLEN)
#define MAP_CHN_3_OFFSET	(MAP_CHN_2_OFFSET + MAP_CHN_2_MAPLEN)
#define MAP_CHN_4_OFFSET	(MAP_CHN_3_OFFSET + MAP_CHN_3_MAPLEN)
#define MAP_CHN_5_OFFSET	(MAP_CHN_4_OFFSET + MAP_CHN_4_MAPLEN)
#define MAP_CHN_6_OFFSET	(MAP_CHN_5_OFFSET + MAP_CHN_5_MAPLEN)
#define MAP_CHN_7_OFFSET	(MAP_CHN_6_OFFSET + MAP_CHN_6_MAPLEN)

#endif