#ifndef SLK_TYPE_H_20200908
#define SLK_TYPE_H_20200908

#define  PCI_PM_CTRL_STATE_MASK   0x0003
#define  PCI_BRIDGE_CONTROL	      0x3e
#define  PCI_BRIDGE_CTL_BUS_RESET 0x40
#define  PCI_CAP_ID_EXP		      0x10	
#define PCI_EXP_DEVCTL		      8
#define  PCI_EXP_DEVCTL_PAYLOAD   0x00e0
#define O_RDWR		00000002

struct slk_list_head {
	struct slk_list_head *next, *prev;
};

#ifdef		SLK_CHERRY

#define		FPGA_RX_RING_LEN		2048	/// 接收队列的长度
#define		FPGA_RX_DMA_LEN			1024	/// 与硬件交互的dma队列长度

#else

#ifdef      SLK_CHERRY_STOCK
#define		FPGA_RX_RING_LEN		8192	/// 接收队列的长度
#define		FPGA_RX_DMA_LEN			4096	/// 与硬件交互的dma队列长度
#else
#define		FPGA_RX_RING_LEN		2048	/// 接收队列的长度
#define		FPGA_RX_DMA_LEN			1024	/// 与硬件交互的dma队列长度
#endif

#endif

#ifdef		SLK_MANGO
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_CHERRY
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_CHERRY_STOCK
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif


#ifdef		SLK_WATERMELON
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_WAXBERRY
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_COCO
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_GUAVA
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_KIWI
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif


#ifdef		SLK_KGR
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_HDH
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_SUPER_KIWI
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_MULTI_KIWI
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_SAAP_ONLOAD
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_ONLOAD_KIWI
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_DELAY
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_MINI_KGR
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_TCPDUMP
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_BOARD_CHECK
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_PERSIMMION
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#endif

#ifdef		SLK_SME_ENGINE
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000
#endif

#ifdef		SLK_SME_DDS
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000
#endif

#ifdef		SLK_SME_SDC
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000
#endif

#ifdef		SLK_SME_STF
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000
#endif

#ifdef		SLK_SME_MRISK
// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000
#endif


#ifdef		SLK_CHERRY_ONE_AND_ONLOAD

#define		FPGA_RX_RING_LEN		2048	/// 接收队列的长度
#define		FPGA_RX_DMA_LEN			1024	/// 与硬件交互的dma队列长度


// TX
#define		TX_PCIE_CMD				0x2008 //0xa800
#define		TX_CMD_BELENGTH			0x2010 //0xa808
#define		TX_ADDR_U				0x2018 //0xa810
#define		TX_ADDR_L				0x2020 //0xa818
// RX
#define		RX_PCIE_CMD				0x2008 //0xa800
#define		RX_ADDR_U				0x2028 //0xa820
#define		RX_ADDR_L				0x2030 //0xa828
// tx
#define		TX_PCIE_CMD_DATA1		0x80000000
#define		TX_PCIE_CMD_DATA2		0x00000000
// rx
#define		RX_PCIE_CMD_DATA1		0x40000000
#define		RX_PCIE_CMD_DATA2		0x00000000

#ifdef		SLK_MANGO
#define		MODULE_NAME					"mango"
#endif

#ifdef		SLK_CHERRY
#define		MODULE_NAME					"cherry"
#endif

#ifdef		SLK_CHERRY_STOCK
#define		MODULE_NAME					"cherry_stock"
#endif

#ifdef		SLK_WATERMELON
#define		MODULE_NAME					"watermelon"
#endif

#ifdef		SLK_WAXBERRY
#define		MODULE_NAME					"waxberry"
#endif

#ifdef		SLK_COCO
#define		MODULE_NAME					"coco"
#define		SLK_COMBIN_LIST_COUNT		(1)									/// 1个通道
#endif

#ifdef		SLK_KIWI
#define		MODULE_NAME					"kiwi"
#endif

#ifdef		SLK_GUAVA		
#define		MODULE_NAME					"guava"
#endif

#ifdef		SLK_KGR
#define		MODULE_NAME					"kangaroo"
#endif

#ifdef		SLK_HDH
#define		MODULE_NAME					"hdh"
#endif

#ifdef		SLK_SUPER_KIWI
#define		MODULE_NAME					"super_kiwi"
#endif

#ifdef		SLK_DELAY
#define		MODULE_NAME					"delay"
#endif

#ifdef		SLK_ONLOAD_KIWI
#define		MODULE_NAME					"onload_kiwi"					/// 这里是onload 的driver
#define		SLK_COMBIN_LIST_COUNT		(65536 + 256)						/// 组合列表的数量，主要用来与应用层交互用的最大值,包括65536个session和256个server
#endif

#ifdef		SLK_MULTI_KIWI
#define		MODULE_NAME					"multi_kiwi"
#define		SLK_COMBIN_LIST_COUNT		(128+32)
#endif

#ifdef		SLK_SAAP_ONLOAD
#define		MODULE_NAME					"saap_onload"
#define		SLK_COMBIN_LIST_COUNT		(128+32)
#endif

#ifdef		SLK_MINI_KGR
#define		MODULE_NAME					"mini_kgr"
#define		SLK_COMBIN_LIST_COUNT		(7)									/// 六个通道
#endif

#ifdef		SLK_TCPDUMP
#define		MODULE_NAME					"hd_tcpdump"
#define		SLK_COMBIN_LIST_COUNT		(4)									/// 六个通道
#endif

#ifdef		SLK_BOARD_CHECK
#define		MODULE_NAME					"board_check"
#define		SLK_COMBIN_LIST_COUNT		(4)									/// 六个通道
#endif

#ifdef		SLK_PCIE_CFG
#define		MODULE_NAME					"pcie_config"
#endif

#ifdef		SLK_NFQ_HOOK
#define		MODULE_NAME					"nfq_hook"
#endif

#ifdef		 SLK_PERSIMMION
#define		 MODULE_NAME				"persimmon"
#endif

#ifdef		 SLK_SME_STF
#define		 MODULE_NAME				"sme_stf"
#endif

#ifdef		 SLK_SME_ENGINE
#define		 MODULE_NAME				"sme_engine"
#endif

#ifdef		 SLK_SME_DDS
#define		 MODULE_NAME				"sme_dds"
#endif

#ifdef		 SLK_SME_SDC
#define		 MODULE_NAME				"sme_sdc"
#endif

#ifdef		 SLK_SME_MRISK
#define		 MODULE_NAME				"sme_mrisk"
#endif

#ifdef		SLK_CHERRY_ONE_AND_ONLOAD
#define		MODULE_NAME					"cherry_and_onload"
#define		SLK_COMBIN_LIST_COUNT		(128+32)
#endif


#endif

#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef uint16_t
#define uint16_t unsigned short
#endif

#ifndef u32
#define  u32 unsigned int
#endif

#ifndef uint32_t
#define uint32_t unsigned int
#endif

#ifndef u64
#define u64 unsigned long long
#endif

#ifndef uint64_t
#define uint64_t unsigned long long
#endif

#ifndef bool
#define bool _Bool
#endif

typedef void* SLK_PTR_DAM_ADDR_T;
typedef void* SLK_PTR_PCI_DEV;
typedef void* SLK_SPINLOCK_T;
typedef void* SLK_ATOMIC_T;
typedef void* SLK_DEVICE;
typedef void* SLK_TIMEVAL;
typedef void* SLK_WAIT_QUEUE_HEAD_T;
typedef void* SLK_SEMAPHORE;
typedef void* SLK_FILE;
typedef void* SLK_TIMEX;
typedef void* SLK_PTR_TASK_STRUCT;
typedef void* SLK_SK_BUFF;
typedef void* SLK_TCPHDR;
typedef void* SLK_NET_DEVICE;
typedef void* SLK_MM_SEGMENT_T;
typedef void* SLK_OP_FILES;
typedef void* SLK_CDEV;
typedef void* SLK_CLASS;
typedef void* SLK_DEV_T;

#endif

