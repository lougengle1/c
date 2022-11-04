#pragma once
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define  SL_NIC_NF5_2MAC10G			(0x01)
#define  SL_NIC_NF5_2MAC1G			(0x02)
#define  SL_NIC_NF10_2MAC10G		(0x03)
#define  SL_NIC_NF10_2MAC1G			(0x04)

enum e_net_type
{
	E_NIC_NET_TYPE_UDP = 0,
	E_NIC_NET_TYPE_TCP,
};

//session过滤的配置
struct st_session_config
{
	char				m_smac[20];						//源mac地址
	char				m_dmac[20];						//目标mac地址
	char				m_sip[20];						//源ip地址
	char				m_dip[20];						//目标地址
	unsigned int		m_sport;						//源端口
	unsigned int		m_dport;						//目标端口
	int					m_net_type;						//网络传输类型tcp/udp
	int					m_mac;							//mac口的选择 0-代表mac过滤不开启 1-代表mac0 2-代表mac1
};

//session的配置
struct st_session_group
{
	st_session_config	m_session_config[16];			//一共16个session，目前支持4个
	unsigned short		m_channel;						//通道号[2,7]，取值范围为2-7。
	unsigned short		m_cpu;							//-1为不绑核,其他为绑定的cpu核
	unsigned int		m_session_num;					//一个通道配置session的总数
};

#pragma pack(push, 1)
//硬件到驱动的协议
struct st_nic_data_head
{
	uint8_t				m_mac_id;						//物理网口
	uint8_t				m_channel_id;					//目标通道id
	uint8_t				m_reserved_0[2];				//判断数据来的标志位
	uint32_t			m_bord_time_s;					//板子秒级系统时间
	uint32_t			m_bord_time_ns;					//板子纳秒级系统时间
	uint8_t				m_reserved_1;					//保留字段
	uint8_t				m_ip_prot;						//ip协议类型
	uint16_t			m_hard_payload_len;				//payload长度
	uint8_t				m_fragment_flag;				//分包长度
	uint16_t			m_fragment_offset;				//分包偏移
	uint8_t				m_reserved_2;					//保留字段
	uint32_t			m_reserved_3;					//保留字段
	unsigned long		m_reserved_4;					//保留字段
};

#pragma pack(pop)
struct slnic_net_info
{
	char					m_name[6];					//网络设备名字	
	struct sockaddr_in		sin;						//网络设备ip
	unsigned char			m_mac[6];					//网络设备mac
	struct sockaddr_in		netmask;					//网络设备子网掩码
	bool					m_running_flags;			//网络设备是否运行
};

struct mac_packet_statistics
{
	unsigned long mac0_rx_pkt_num;						//MAC0输入的包统计
	unsigned long mac0_rx_pkt_desert_num;				//MAC0输入的丢包统计
	unsigned long mac0_tx_pkt_num;						//MAC0输出的包统计
	
	unsigned long mac1_rx_pkt_num;						//MAC1输入的包统计
	unsigned long mac1_rx_pkt_desert_num;				//MAC1输入的丢包统计
	unsigned long mac1_tx_pkt_num;						//MAC1输出的包统计
};


struct slnic_hw_version_info
{
	unsigned short production;							//shengli_nic项目的产品号为0x16
	unsigned short edition;								//硬件版本的svn号
};



class slnic_event
{
public:
	virtual int on_data_recv(int channel,const char *data, int data_len) = 0;
};

class i_slnic
{
public:

	/*************************************************
	* Function:		create_handle 
	  Description:  创建i_slnic对象
	  Input:		NULL
	  Return:       i_slnic指针
	**************************************************/
	static i_slnic *create_handle();

	virtual ~i_slnic() {};

	/*************************************************
	* Function:		get_version
	  Description:	获取程序版本号
	  Input:		NULL
	  Return:       版本号
	**************************************************/
	virtual const char *get_api_version() = 0; 

	/*************************************************
	* Function:		get_hw_version
	Description:	获取硬件版本号
	Input:			dev1第一张板卡是否开启
					dev2第二张板卡是否开启
	Return:         读取是否成功
	**************************************************/
	virtual bool get_hw_version(slnic_hw_version_info &hw_info, int dev1, int dev2) = 0;

	/*************************************************
	* Function:		get_driver_version
	Description:	获取硬件版本号
	Input:			dev1第一张板卡是否开启
					dev2第二张板卡是否开启
	Return:			读取是否成功
	**************************************************/
	virtual bool get_driver_version(int dev1, int dev2) = 0;


	/*************************************************
	* Function:		get_sys_unifo
	Description:	获取具体nf5或者nf10 mac10g还是1g版本
	Input:		NULL
	Return:       版本号
	**************************************************/
	virtual int get_sys_unifo() = 0;

	/*************************************************
	* Function:		slnic_init
	  Description:	slnic进入
	  Input:		dev1第一张板卡是否开启
					dev2第二张板卡是否开启
	  Return:       return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_init(int dev) = 0;


	/*************************************************
	* Function:		slnic_exit
	  Description:	slnic退出
	  Input:		NULL
	  Return:       return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_exit() = 0;
	

	/*************************************************
	* Function:		slnic_fdir_config
	  Description:	slnic接收数据	配置ession，通道号，每个通道号绑定一个cpu来收数据
	  Input:		@config:		session配置
					@slnic_event*:	通道号收数据的函数
					dev1第一张板卡是否开启
					dev2第二张板卡是否开启
	  Return:		return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_fdir_config(st_session_group *config, slnic_event* ev, int dev) = 0;
	

	/*************************************************
	* Function:		slnic_delete_config
	  Description:	删除session数据
	  Input:		@config:		session配置
	  Return:		return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_delete_config(st_session_group *config) = 0;


	/*************************************************
	* Function:		slnic_get_net_info
	Description:	获取slnic0基本信息
	Input:		@eth_name:		设备名字列如slnic0
				@slnic_net_info 获取的信息
	Return:		return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_get_net_info(const char* eth_name, slnic_net_info*) = 0;


	virtual bool slnic_get_packet_statistics(mac_packet_statistics* p_mac_packet_statistics, int dev) = 0;

	

	/*************************************************
	* Function:		slnic_send
	  Description:	slnic选择对应的mac口发送数据
	  Input:		@mac_id:		mac口的选择
					@data：			发送的数据
					@data_len		发送数据的长度
	  Return:       return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_send(uint8_t mac_id, const char* data, short data_len) = 0;


	/*************************************************
	* Function:		slnic_start
	  Description:	slnic启动线程
	  Input:		dev第一张板卡是否开启
	
	  Return:       return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_start(int dev) = 0;


	/*************************************************
	* Function:		slnic_stop
	  Description:	slnic关闭线程
	  Input:		dev第一张板卡是否开启

	  Return:       return true代表成功，return false代表失败
	**************************************************/
	virtual bool slnic_stop(int dev) = 0;
};