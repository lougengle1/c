#pragma once

#include <stdint.h>

#define MAX_CHANNEL		(8)
#define CHN_MAX_NODE	(8)
#define CHN_NODE_SIZE	(2048)

#define STREAM_DEVICE1		"/dev/slnic/stream1"
#define REG_CTL_DEVICE1		"/dev/slnic/register1"
#define NIC_CTL_DEVICE1		"/dev/slnic/slnic1"

#define STREAM_DEVICE2		"/dev/slnic/stream2"
#define REG_CTL_DEVICE2		"/dev/slnic/register2"
#define NIC_CTL_DEVICE2		"/dev/slnic/slnic2"

# define unlikely(t)  __builtin_expect((t), 0)
# define likely(t)  __builtin_expect((t), 1)

struct slk_addr_t
{
	long            m_offset;
	long            m_lenth;

	long            m_channel;
	long            m_dma_addr;

	int				m_node_size;
	int				m_node_num;
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

#define SLK_SYS_ID						0x0010
#define SLK_SYS_SEQU_NUM				0x0018
#define SLK_SYS_UINFO1					0x0020

#define SLK_TRX_QUE_DONE_REG_ADDR       0x2008
#define SLK_TX_QUE_BASE_VALUE           0x80000000
#define SLK_RX_QUE_BASE_VALUE           0x40000000

#define SLK_RX_NEW_LADDR_REGISTER0      0x2068
#define SLK_RX_NEW_LADDR_REGISTER1      0x2088
#define SLK_RX_NEW_LADDR_REGISTER2      0x20a8
#define SLK_RX_NEW_LADDR_REGISTER3      0x20c8
#define SLK_RX_NEW_LADDR_REGISTER4      0x20e8
#define SLK_RX_NEW_LADDR_REGISTER5      0x2108
#define SLK_RX_NEW_LADDR_REGISTER6      0x2128
#define SLK_RX_NEW_LADDR_REGISTER7      0x2148

#define MAX_TX_NODE						16
#define TMO_TX_REG_ADDR					0x2010
#define	TX_REG_COUNTER					0x2040 //0x0030

#define SLK_BAR0_SIZE					(1024 * 1024)//0x10000

#define SLK_REG_IP_CHECKSUM_EN			0x0468
#define SLK_REG_TCP_CHECKSUM_EN			0x0470
#define SLK_BOARD_MAC0_IP				0x0500

#define SLK_MAC0_ADDR_H					0x4000
#define SLK_MAC0_ADDR_L					0x4008
#define SLK_MAC0_CFG_EN					0x4048

#define SLK_MAC1_ADDR_H					0x4100
#define SLK_MAC1_ADDR_L					0x4108
#define SLK_MAC1_CFG_EN					0x4148

#define SLK_MAC0_RX_FRAME_CTL			0x4010
#define SLK_SESSION0_SIP				0x5000
#define SLK_SESSION0_SPORT				0x5008
#define SLK_SESSION0_DIP				0x5010
#define SLK_SESSION0_DPORT				0x5018
#define SLK_SESSION0_EN_CTL				0x5020

#define ch0_subpkt_num					0x6000			//指数
#define ch0_subpkt_timeout				0x6008			
#define	ch1_subpkt_num					0x6100			//成交
#define ch1_subpkt_timeout				0x6108
#define	ch2_subpkt_num					0x6200			//期权
#define ch2_subpkt_timeout				0x6208
#define	ch3_subpkt_num					0x6300			//建树
#define ch3_subpkt_timeout				0x6308

///new rx

#define HW_REV_PKT_CNT					0x2008

#define HW_SEND_PKT_CNT0				0X2010
#define HW_SEND_PKT_CNT1				0X2018
#define HW_SEND_PKT_CNT2				0X2020
#define HW_SEND_PKT_CNT3				0X2028
#define HW_SEND_PKT_CNT4				0X2030
#define HW_SEND_PKT_CNT5				0X2038
#define HW_SEND_PKT_CNT6				0X2040
#define HW_SEND_PKT_CNT7				0X2048

#define HW_REV_SWADDR					0x2050

#define HW_SEND_SW_OFF_ADDR0			0x2058
#define HW_SEND_SW_OFF_ADDR1			0x2060
#define HW_SEND_SW_OFF_ADDR2			0x2068
#define HW_SEND_SW_OFF_ADDR3			0x2070
#define HW_SEND_SW_OFF_ADDR4			0x2078
#define HW_SEND_SW_OFF_ADDR5			0x2080
#define HW_SEND_SW_OFF_ADDR6			0x2088
#define HW_SEND_SW_OFF_ADDR7			0x2090

#define HW_PKT_MEMSIZE0					0x2098
#define HW_PKT_MEMSIZE1					0x20a0
#define HW_PKT_MEMSIZE2					0x20a8
#define HW_PKT_MEMSIZE3					0x20b0
#define HW_PKT_MEMSIZE4					0x20b8
#define HW_PKT_MEMSIZE5					0x20c0
#define HW_PKT_MEMSIZE6					0x20c8
#define HW_PKT_MEMSIZE7					0x20d0

#define HW_PKT_MEMCNT0					0x20d8
#define HW_PKT_MEMCNT1					0x20e0
#define HW_PKT_MEMCNT2					0x20e8
#define HW_PKT_MEMCNT3					0x20f0
#define HW_PKT_MEMCNT4					0x20f8
#define HW_PKT_MEMCNT5					0x2100
#define HW_PKT_MEMCNT6					0x2108
#define HW_PKT_MEMCNT7					0x2110

#define SW_PKT_CUR_MEM_OFF0				0x2118
#define SW_PKT_CUR_MEM_OFF1				0x2120
#define SW_PKT_CUR_MEM_OFF2				0x2128
#define SW_PKT_CUR_MEM_OFF3				0x2130
#define SW_PKT_CUR_MEM_OFF4				0x2138
#define SW_PKT_CUR_MEM_OFF5				0x2140
#define SW_PKT_CUR_MEM_OFF6				0x2148
#define SW_PKT_CUR_MEM_OFF7				0x2150

#define SW_PKT_START_ADDR0				0x2158
#define SW_PKT_START_ADDR1				0x2160
#define SW_PKT_START_ADDR2				0x2168
#define SW_PKT_START_ADDR3				0x2170
#define SW_PKT_START_ADDR4				0x2178
#define SW_PKT_START_ADDR5				0x2180
#define SW_PKT_START_ADDR6				0x2188
#define SW_PKT_START_ADDR7				0x2190

#define SESSION_EN_CTL_EN_BIT			(15)
#define SESSION_EN_CTL_CH_BIT			(12)

//#define MAC0_SESSION_0_SIP				0x5000
//#define MAC0_SESSION_0_SPORT			0x5008
//#define MAC0_SESSION_0_DIP				0x5010
//#define MAC0_SESSION_0_DPORT			0x5018
//#define MAC0_SESSION_0_CTRL				0x5020
//....
//#define MAC0_SESSION_15_SIP				0x53c0
//
//#define MAC1_SESSION_0_SIP				0X5400
//....
//#define MAC1_SESSION_15_SIP				0X57C0

#define CHL2_SESSION0_SIP				   0x6800
#define CHL3_SESSION0_SIP				   0x6c00
#define CHL4_SESSION0_SIP				   0x7000	
#define CHL5_SESSION0_SIP				   0x7400	
#define CHL6_SESSION0_SIP				   0x7800	
#define CHL7_SESSION0_SIP				   0x7c00	

