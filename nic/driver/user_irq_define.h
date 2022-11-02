#ifndef USER_IRQ_DEFINE_H_20120613
#define USER_IRQ_DEFINE_H_20120613

/// 一级寄存器地址
#define		SYS_SOFT_RST				0x0000 //TSPE_CTRL0			0x0
#define		TX_CTRL						0x6000 //TX_CTRL			0x80e0
#define		RX_CTRL						0x6400 //RX_CTRL			0x8130
#define		RX_DATA_CTRL				0x6408 //MAC_CTRL0			0x8220

#define		RX_REG_COUNTER				0x2038 //0x0028
#define		TX_REG_COUNTER				0x2008 //0x0030

#define		SYS_TX_REGISTER				0x2050
#define		SYS_RX_REGISTER				0x2048

#define		REG_KIWI_SESSION_CONTROL		0x4800 // session使能
#define		REG_KIWI_SESSION_SERVER_FLAG	0x4808 // 服务器标志
#define		REG_KIWI_SESSION_CONFIG_DONE	0x4810 // 寄存器配置完成			
#define		REG_KIWI_SESSION_REMOTE_IP		0x4818 // 远端ip
#define		REG_KIWI_SESSION_LOCAL_PORT		0x4820 // 本地port
#define		REG_KIWI_SESSION_REMOTE_PORT	0x4828 // 远端port
#define		REG_KIWI_SESSION_PROTOCOL		0x4830 // tcp udp标志
#define		REG_KIWI_SESSION_ID				0x4838 // session id
#define		REG_KIWI_SSTHREFSH_GROUP		0x5820 // 拥塞分组寄存器
#define		SYS_STATUS						0x0138 // 系统状态寄存器

#define		REG_KIWI_RXDMA_FOBIDDEN			0x4848	// 控制rx上传数据

#define		MAC0_LED						0x4050
#define		MAC1_LED						0x4150

#define		MAC0_ADDR_H						0x4000
#define		MAC0_ADDR_L						0x4008
#define		MAC_OFFSET						0x100


#define		REG_KIWI_CONTROL_BYPASS			0x5808
#define		KIWI_CONTROL_BYPASS_VALUE		0x10

#define		REG_SESSION_FULL_STATUS			0x2048	/// 一致性dma方式，反馈session是不是满的寄存器地址

#define		REG_PCIE_CTRL_ADDR				0x2000

#define		REG_ENABLE_RECV					0x0140

#define		REG_ERROR_INFO					0x2058
#define		REG_HARDWARE_DONE				0x148

//新增寄存器
//系统级
#define		SYS_ID						0x0010 
#define		SYS_SEQU_NUM				0x0018 
#define		SYS_UINFO1					0x0020 //只读
#define		SYS_UINFO2					0x0028 //只读
#define		SYS_INFOR_CTRL				0x0120 //只写
#define		SYS_IRQ						0x0130 //
#define		BOARD_TEMPERATURE_DATA		0x0140//r

#define		SYS_TIME_HMS				0x0400 //
#define		SYS_TIME_NS					0x0408 //
#define		SYS_TIME_CTRL				0x0410 //
#define		SYS_SETTIME_DELAY_NS		0x0418 //只读
//网络层
#define		QMAC_ADDR0					0x4068 //
#define		QMAC_ADDR1					0x4070 //
#define		QMAC_RX_CTRL				0x40b8 //

#define		ACEM_TIMER					0x0500
/// 一级寄存器数据
#define		FPGA_IRQ_DISABLE 			0x00000840 
#define		FPGA_IRQ_ENABLE 			0x00000846


/// 二级寄存器地址
/// 第一级地址
#define		FLTR_CTRL_REG				0xe000 //0xA480

/// rx table Entry
#define		SIP_REG						0x0001
#define		SIP_MASK_REG   				0x0002
#define		DIP_REG        				0x0003
#define		DIP_MASK_REG   				0x0004
#define		SPORT_REG      				0x0005
#define		DPORT_REG      				0x0006
#define		PROT_REG       				0x0007
#define		RTABLE_REG     				0x0008
/// rx table Entry 的使能位
#define		UPREG_CTRL_REG				0xA480


/// 二级寄存器数据
#define		SYS_SOFT_RST_DISABLE_DATA	0xffffffff
#define		SYS_SOFT_RST_ENABLE_DATA	0x0

/// 第一级数据
#define		FLTR_DATA_REG				0xe008 //0xA488
/// 第二级数据

/// rx table Entry0
#define		SIP_REG_DATA1				0xa013402 
#define		SIP_MASK_REG_DATA1   		0xffffffff
#define		DIP_REG_DATA1        		0x1003    
#define		DIP_MASK_REG_DATA1   		0x0       
#define		SPORT_REG_DATA1      		0xffff13bb
#define		DPORT_REG_DATA1      		0x0       
#define		PROT_REG_DATA1       		0xff      
#define		RTABLE_REG_DATA1    		0x80000305

/// rx table Entry1
#define		SIP_REG_DATA2				0xa0134fe 
#define		SIP_MASK_REG_DATA2   		0xffffffff
#define		DIP_REG_DATA2        		0x2003    
#define		DIP_MASK_REG_DATA2   		0x0       
#define		SPORT_REG_DATA2      		0xffff13bc
#define		DPORT_REG_DATA2      		0x0       
#define		PROT_REG_DATA2       		0xff      
#define		RTABLE_REG_DATA2    		0x80000101

/// rx table Entry 的使能位（基地址，对应的Entry0）
#define		UPREG_CTRL_REG_BASE			0x015A0000

/// 二级寄存器控制码
#define		UPM_GROUP_WRITE_REG			0x015b0000
#define		IP_PORT_FILTER_WRITE_REG	0x015a0000
#define		LOGIC_REG_WRITE_GROUP		0x01590000

#define		UPM_GROUP_READ_REG			0x014b0000
#define		IP_PORT_FILTER_READ_REG		0x014a0000
#define		LOGIC_REG__READ_GROUP		0x01490000

/// 三级寄存器数据
#define		TX_TABLE_UHEADER_BATE0		0xFF550136

#define		SESSION_COUNT				128
#define		SESSION_OPEN				0x00000001
#define		SESSION_CLOSE				0x00010000
#define		SESSION_RESET				0x00000100
#define		SESSION_ENABLE				0x00000001
#define		SESSION_DISABLE				0x00000000


#define		SESSION_CONNECT_SUC				8
#define		SESSION_ACTIVE_DISCONNECT		10
#define		SESSION_PASSIVE_DISCONNECT		13

#define		SESSION_SW_ACTIVE_DISCONNECT	255
#define		SESSION_STATUS_CLOSE			0
#define		SESSION_STATUS_OPEN				1
#define		SESSION_STATUS_ACTIVE_CLOSE		2
#define		SESSION_STATUS_PASSIVE_CLOSE	3

#define		SYS_STATUS_DATA				0x0032

#define CHL2_SESSION0_SIP				   0x6800
#define CHL3_SESSION0_SIP				   0x6c00
#define CHL4_SESSION0_SIP				   0x7000	
#define CHL5_SESSION0_SIP				   0x7400	
#define CHL6_SESSION0_SIP				   0x7800	
#define CHL7_SESSION0_SIP				   0x7c00	

#endif

