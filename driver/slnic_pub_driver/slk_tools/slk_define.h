/*!****************************************************************************
	@file   	slk_define.h
	@date		2014/08/29   00:00
	@author 	liu.sheng
	
	@brief		�ļ������������г��õ��ĺ꣬���������������Ŀͨ�ú궨�����˹���.

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_DEFINE_H_20120605
#define SLK_DEFINE_H_20120605

#include "slk_lib.h"

/// ���64λ���ݵĸߵ�λ��ַ
#ifdef PCI_ADDR_64

/// д�Ĵ���
#define WRITE_REG(a, reg, val) \
	(*((volatile uint64_t*)(a + reg))) = (uint64_t)(val)

#define WRITE_RAW_REG(reg, val) \
	(*((volatile uint64_t*)(reg))) = (uint64_t)(val)

/// ���Ĵ���
#define READ_REG(a, reg) \
	(uint64_t)(*((volatile uint64_t*)(a + reg)))

#define READ_RAW_REG(reg) \
	(uint64_t)(*((volatile uint64_t*)(reg)))

#define FPGA_LODWORD(_qw)   (_qw) 
#define FPGA_HIDWORD(_qw)   (_qw)
#else
/// д�Ĵ���
#define WRITE_REG(a, reg, val) \
	(*((volatile uint32_t*)(a + reg))) = (uint32_t)(val)

#define WRITE_RAW_REG(reg, val) \
	(*((volatile uint32_t*)(reg))) = (uint32_t)(val)

/// ���Ĵ���
#define READ_REG(a, reg) \
	(uint32_t)(*((volatile uint32_t*)(a + reg)))

#define READ_RAW_REG(reg) \
	(uint32_t)(*((volatile uint32_t*)(reg)))

#define FPGA_LODWORD(_qw)    ((u32)(_qw))
#define FPGA_HIDWORD(_qw)    ((u32)(((_qw) >> 32) & 0xffffffff))

#endif

/// mac��ַ��ӡ
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]

/// ��Ϣ��־ʱ���ʽ��׼��ӡ
#define SLK_LOG_TIME_FMT "%d-%02d-%02d %02d:%02d:%02d.%06d"

/// ��־16���Ƹ�ʽ��׼��ӡ
#define SLK_LOG_LINE_FMT "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"

/// ip��ַ��ӡ
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
#define		SLK_COMBIN_LIST_COUNT		(1)									/// 1��ͨ��
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
#define		MODULE_NAME					"onload_kiwi"					/// ������onload ��driver
#define		SLK_COMBIN_LIST_COUNT		(65536 + 256)						/// ����б����������Ҫ������Ӧ�ò㽻���õ����ֵ,����65536��session��256��server
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
#define		SLK_COMBIN_LIST_COUNT		(7)									/// ����ͨ��
#endif

#ifdef		SLK_TCPDUMP
#define		MODULE_NAME					"hd_tcpdump"
#define		SLK_COMBIN_LIST_COUNT		(4)									/// ����ͨ��
#endif

#ifdef		SLK_BOARD_CHECK
#define		MODULE_NAME					"board_check"
#define		SLK_COMBIN_LIST_COUNT		(4)									/// ����ͨ��
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

/// ������־
#define		sl_error(format, arg...)      slk_error_log(format, ##arg) 
#define		sl_error_ex(format, arg...)   slk_error_log_ex(format " file=%s, line=%d\n", ##arg, __FILE__, __LINE__) 

/// ������־
#define		sl_warning(format, arg...)    slk_warning_log(format, ##arg)
#define		sl_warning_ex(format, arg...) slk_warning_log_ex(format " file=%s, line=%d\n", ##arg, __FILE__, __LINE__)

/// debug��־
#define		sl_debug(format, arg...)    slk_debug_log(format, ##arg)
#define		sl_debug_ex(format, arg...) slk_debug_log_ex(format " file=%s, line=%d\n", ##arg, __FILE__, __LINE__)

/// data��־����׼˫�����ݴ�ӡ��ʽ
#define		sl_data(head,msg,len,seq)	slk_print_bin_ex(head,msg,len,seq)

/// data��־����׼˫�����ݴ�ӡ��ʽ
#define		sl_data_ex(head,msg,len,seq)	slk_print_default_ex(head,msg,len,seq)

/// ��ʱ��־
#define		sl_local(head,msg,len,seq)	slk_print_local_ex(head,msg,len,seq)

#define     sl_session_data_ex(head,msg,len,seq)   slk_print_session_data_ex(head,msg,len,seq)

#define			sl_cur_cpu	task_cpu(current)

/// ����������Ϣ��ض���
#define			SLK_MSG_TYPE_DATA				'M'
#define			SLK_MSG_TYPE_QUE_ATTACH			'Q'

#define			SLK_QUE_ATT_STYPE_BIND			'B'
#define			SLK_QUE_ATT_STYPE_UNBIND		'U'

///дָ���Ĵ���
#define			SLK_WRITE_REG					'W'
///��ָ���Ĵ���
#define			SLK_READ_REG					'R'

#define			SLK_HIGH_READ_REG				'r'
#define			SLK_HIGH_WRITE_REG				'w'
///��ȡbar0��ַ�ռ䳤��
#define			SLK_READ_BAR_LEN				'L'

/// SLK IOCTL��ص�����
#define			SLK_IOCTL_CMD_MV_NEXT			(100)

#define			SLK_TX_MEM_WT_BASE				0x0
#define			SLK_TX_ADDR(bar,i)				((void*)( bar + SLK_TX_MEM_WT_BASE + i*2048 ))

#define			SLK_RX_QUE_HMEM_MAX_COUNT		(100)

/// ��ʱ��صĺ���
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



