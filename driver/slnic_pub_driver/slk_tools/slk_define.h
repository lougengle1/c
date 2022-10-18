/*!****************************************************************************
	@file   	slk_define.h
	@date		2014/08/29   00:00
	@author 	liu.sheng
	
	@brief		文件定义了驱动中常用到的宏，另外对所有驱动项目通用宏定义做了管理.

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_DEFINE_H_20120605
#define SLK_DEFINE_H_20120605

#include "slk_lib.h"

/// 获得64位数据的高低位地址
#ifdef PCI_ADDR_64

/// 写寄存器
#define WRITE_REG(a, reg, val) \
	(*((volatile uint64_t*)(a + reg))) = (uint64_t)(val)

#define WRITE_RAW_REG(reg, val) \
	(*((volatile uint64_t*)(reg))) = (uint64_t)(val)

/// 读寄存器
#define READ_REG(a, reg) \
	(uint64_t)(*((volatile uint64_t*)(a + reg)))

#define READ_RAW_REG(reg) \
	(uint64_t)(*((volatile uint64_t*)(reg)))

#define FPGA_LODWORD(_qw)   (_qw) 
#define FPGA_HIDWORD(_qw)   (_qw)
#else
/// 写寄存器
#define WRITE_REG(a, reg, val) \
	(*((volatile uint32_t*)(a + reg))) = (uint32_t)(val)

#define WRITE_RAW_REG(reg, val) \
	(*((volatile uint32_t*)(reg))) = (uint32_t)(val)

/// 读寄存器
#define READ_REG(a, reg) \
	(uint32_t)(*((volatile uint32_t*)(a + reg)))

#define READ_RAW_REG(reg) \
	(uint32_t)(*((volatile uint32_t*)(reg)))

#define FPGA_LODWORD(_qw)    ((u32)(_qw))
#define FPGA_HIDWORD(_qw)    ((u32)(((_qw) >> 32) & 0xffffffff))

#endif

/// mac地址打印
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]

/// 消息日志时间格式标准打印
#define SLK_LOG_TIME_FMT "%d-%02d-%02d %02d:%02d:%02d.%06d"

/// 日志16进制格式标准打印
#define SLK_LOG_LINE_FMT "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"

/// ip地址打印
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

#define NIPQUAD_FMT "%u.%u.%u.%u"

#define SHENGLI_NEW_VENDOR		0x8009
#define SHENGLI_OLD_VENDOR		0x18df

#define SLK_RX_QUE_MAX_COUNT	1024
#define SLK_TX_QUE_MAX_COUNT	16

#define SLK_RX_MAX_CHAN_COUNT	8
#define SLK_RX_SIXTN_CHAN_COUNT	(16)
			

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

/// 错误日志
#define		sl_error(format, arg...)      slk_error_log(format, ##arg) 
#define		sl_error_ex(format, arg...)   slk_error_log_ex(format " file=%s, line=%d\n", ##arg, __FILE__, __LINE__) 

/// 警告日志
#define		sl_warning(format, arg...)    slk_warning_log(format, ##arg)
#define		sl_warning_ex(format, arg...) slk_warning_log_ex(format " file=%s, line=%d\n", ##arg, __FILE__, __LINE__)

/// debug日志
#define		sl_debug(format, arg...)    slk_debug_log(format, ##arg)
#define		sl_debug_ex(format, arg...) slk_debug_log_ex(format " file=%s, line=%d\n", ##arg, __FILE__, __LINE__)

/// data日志，标准双向数据打印格式
#define		sl_data(head,msg,len,seq)	slk_print_bin_ex(head,msg,len,seq)

/// data日志，标准双向数据打印格式
#define		sl_data_ex(head,msg,len,seq)	slk_print_default_ex(head,msg,len,seq)

/// 临时日志
#define		sl_local(head,msg,len,seq)	slk_print_local_ex(head,msg,len,seq)

#define     sl_session_data_ex(head,msg,len,seq)   slk_print_session_data_ex(head,msg,len,seq)

#define			sl_cur_cpu	task_cpu(current)

/// 驱动传输消息相关定义
#define			SLK_MSG_TYPE_DATA				'M'
#define			SLK_MSG_TYPE_QUE_ATTACH			'Q'

#define			SLK_QUE_ATT_STYPE_BIND			'B'
#define			SLK_QUE_ATT_STYPE_UNBIND		'U'

///写指定寄存器
#define			SLK_WRITE_REG					'W'
///读指定寄存器
#define			SLK_READ_REG					'R'

#define			SLK_HIGH_READ_REG				'r'
#define			SLK_HIGH_WRITE_REG				'w'
///读取bar0地址空间长度
#define			SLK_READ_BAR_LEN				'L'

/// SLK IOCTL相关的命令
#define			SLK_IOCTL_CMD_MV_NEXT			(100)

#define			SLK_TX_MEM_WT_BASE				0x0
#define			SLK_TX_ADDR(bar,i)				((void*)( bar + SLK_TX_MEM_WT_BASE + i*2048 ))

#define			SLK_RX_QUE_HMEM_MAX_COUNT		(100)

/// 计时相关的函数
#define			SLK_TIME_REG1_ADDR			0x8e10
#define			SLK_TIME_REG2_ADDR			0x8e18
#define			SLK_TIME_REG3_ADDR			0x8e20
#define			SLK_TIME_REG4_ADDR			0x8e28
#define			SLK_TIME_REG5_ADDR			0x8e30
#define			SLK_ENABLE_TIME_REG_ADDR	0x8e00
#define			SLK_REPORT_TIME_REG_ADDR	0x8e08

#define			SLK_TIME_POINT1(a)			WRITE_REG(a,SLK_TIME_REG1_ADDR,1)
#define			SLK_TIME_POINT2(a)			WRITE_REG(a,SLK_TIME_REG2_ADDR,1)
#define			SLK_TIME_POINT3(a)			WRITE_REG(a,SLK_TIME_REG3_ADDR,1)
#define			SLK_TIME_POINT4(a)			WRITE_REG(a,SLK_TIME_REG4_ADDR,1)
#define			SLK_TIME_POINT5(a)			WRITE_REG(a,SLK_TIME_REG5_ADDR,1)

#define			SLK_ENABLE_TIME(a,v)		WRITE_REG(a,SLK_ENABLE_TIME_REG_ADDR,v)
#define			SLK_REPORT_TIME(a)			WRITE_REG(a,SLK_REPORT_TIME_REG_ADDR,1)

#ifdef			SLK_QUEM_EVN
 #define QUEM_LOAD(a)	WRITE_REG(a,0x10000,1)
 #define QUEM_UNLOAD(a)	WRITE_REG(a,0x10000,0)

#else
 #define QUEM_LOAD(a)
 #define QUEM_UNLOAD(a)

#endif


#define		SLK_SW2HW(driv,len,pcie)				do	{ \
														int			i=0; \
														long long	ll_driv		= (long long)(driv); \
														char*		ch_driv_end = (char*)&ll_driv; \
														char		ch_pcie_beg[sizeof(long long)]={0}; \
														for( i=0;i < (int)(len);i++ ) \
														{ \
															ch_pcie_beg[i] = ch_driv_end[(len) - (i+1)]; \
														} \
														ll_driv = *(long long*)(ch_pcie_beg); \
														(pcie) = ll_driv; \
													} while(0)	

#endif



