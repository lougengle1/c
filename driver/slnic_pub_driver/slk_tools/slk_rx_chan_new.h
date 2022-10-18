#ifndef _SLK_RX_CHANN_NEW_H_
#define _SLK_RX_CHANN_NEW_H_

//参考 http://10.1.59.14/svn/waxberry_china/EFH_porch/trunk/common/EFH_porch项目寄存器说明.docx

#define MAX_CHANNEL_NUM			(8)

#define HW_REV_PKT_CNT			0x2008				//硬件收包个数

#define HW_SEND_PKT_CNT0		0X2010				//硬件通道0发包个数
#define HW_SEND_PKT_CNT1		0X2018				//硬件通道1发包个数
#define HW_SEND_PKT_CNT2		0X2020
#define HW_SEND_PKT_CNT3		0X2028
#define HW_SEND_PKT_CNT4		0X2030
#define HW_SEND_PKT_CNT5		0X2038
#define HW_SEND_PKT_CNT6		0X2040
#define HW_SEND_PKT_CNT7		0X2048

#define HW_REV_SWADDR			0x2050				//Root端发包计数器地址

#define HW_SEND_SW_OFF_ADDR0	0x2058				//通道0硬件发包循环队列地址，在root端
#define HW_SEND_SW_OFF_ADDR1	0x2060				//通道1硬件发包循环队列地址，在root端
#define HW_SEND_SW_OFF_ADDR2	0x2068
#define HW_SEND_SW_OFF_ADDR3	0x2070
#define HW_SEND_SW_OFF_ADDR4	0x2078
#define HW_SEND_SW_OFF_ADDR5	0x2080
#define HW_SEND_SW_OFF_ADDR6	0x2088
#define HW_SEND_SW_OFF_ADDR7	0x2090

#define HW_PKT_MEMSIZE0			0x2098				//驱动接收通道0包存储单元大小，单位byte
#define HW_PKT_MEMSIZE1			0x20a0
#define HW_PKT_MEMSIZE2			0x20a8
#define HW_PKT_MEMSIZE3			0x20b0
#define HW_PKT_MEMSIZE4			0x20b8
#define HW_PKT_MEMSIZE5			0x20c0
#define HW_PKT_MEMSIZE6			0x20c8
#define HW_PKT_MEMSIZE7			0x20d0

#define HW_PKT_MEMCNT0			0x20d8				//驱动接收通道0包存储单元总个数-1
#define HW_PKT_MEMCNT1			0x20e0
#define HW_PKT_MEMCNT2			0x20e8
#define HW_PKT_MEMCNT3			0x20f0
#define HW_PKT_MEMCNT4			0x20f8
#define HW_PKT_MEMCNT5			0x2100
#define HW_PKT_MEMCNT6			0x2108
#define HW_PKT_MEMCNT7			0x2110

#define SW_PKT_CUR_MEM_OFF0		0x2118				//驱动接收通道0包存储单元回收个数
#define SW_PKT_CUR_MEM_OFF1		0x2120
#define SW_PKT_CUR_MEM_OFF2		0x2128
#define SW_PKT_CUR_MEM_OFF3		0x2130
#define SW_PKT_CUR_MEM_OFF4		0x2138
#define SW_PKT_CUR_MEM_OFF5		0x2140
#define SW_PKT_CUR_MEM_OFF6		0x2148
#define SW_PKT_CUR_MEM_OFF7		0x2150

#define SW_PKT_START_ADDR0		0x2158				//驱动接收通道0第一个包存储单元地址
#define SW_PKT_START_ADDR1		0x2160
#define SW_PKT_START_ADDR2		0x2168
#define SW_PKT_START_ADDR3		0x2170
#define SW_PKT_START_ADDR4		0x2178
#define SW_PKT_START_ADDR5		0x2180
#define SW_PKT_START_ADDR6		0x2188
#define SW_PKT_START_ADDR7		0x2190

struct slk_rx_channel_node
{
	int		 m_node_size;
	int		 m_node_num;

	long	 m_dma_addr_start;
	void	*m_virt_addr_start;
};

struct slk_rx_channel_new
{
	int							 m_used_channel_num;
	void						*m_dev;
	long						 m_bar0;
	long						 m_hw_off_dma_addr;
	void						*m_hw_off_virt_addr;
	struct slk_rx_channel_node	 m_dma_chan[MAX_CHANNEL_NUM];
};


#define init_channel_new_node_param(_CHAN, _NODE, _NODE_SIZE, _NODE_NUM) { \
(_CHAN).m_dma_chan[_NODE].m_node_size = (_NODE_SIZE); \
(_CHAN).m_dma_chan[_NODE].m_node_num = (_NODE_NUM); \
}

#define init_channel_new_param(_CHAN, _DEV, _BAR0, _CHANNEL_NUM)		{ \
(_CHAN).m_used_channel_num = (_CHANNEL_NUM); \
(_CHAN).m_dev = (_DEV);	\
(_CHAN).m_bar0 = (_BAR0); \
}

int slk_rx_channel_alloc_dma(struct slk_rx_channel_new *chan);
int slk_rx_channel_hw_config(struct slk_rx_channel_new *chan);
int slk_rx_channel_new_create(struct slk_rx_channel_new *chan);
int slk_rx_channel_new_destory(struct slk_rx_channel_new *chan);

#endif