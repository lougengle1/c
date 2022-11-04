#pragma once
#include "interface/i_slnic.h"
#include "device_handle/reg_controler.h"
#include "device_handle/channel_controler.h"
#include "device_handle/open_ioctl_close.h"
#include "device_handle/net_info.h"
#include "tool_lib/efh_loger.h"

#include <pthread.h>

#define SL_MAX_RECV_CHANN		(8)
#define SL_MAX_SESSION_CHL		(8)
#define SL_MAX_SESSION_NUM		(16)

#define SL_PORCH_SW_VERSION		"s_1.0"


#pragma pack(push, 1)
struct driver_to_hardware_head
{
	uint8_t			m_mode;
	uint8_t			m_resv1;
	uint8_t			m_channel_id;
	unsigned long	m_user_data;
	int				m_resv2 : 24;
	unsigned short	m_payload_lenth;
	unsigned long	resv3;
	unsigned long	resv4;
};

struct driver_transmission_head
{
	char m_flag;														///<与驱动传送的消息标志位
	int	 m_seq;															///<与驱动传送的消息序号
	int	 m_len;															///<传输数据的长度
};


struct hw_session_cnf
{
	char				*m_s_mac;
	char				*m_d_mac;
	char				*m_src_ip;
	char				*m_dst_ip;

	uint16_t			 m_src_port;
	uint16_t			 m_dst_port;

	bool				 m_session_tcp;

	bool				 m_enable_smac;
	bool				 m_enable_dmac;
	bool				 m_enable_src_ip;
	bool				 m_enable_src_port;
	bool				 m_enable_dst_ip;
	bool				 m_enable_dst_port;

	int					 m_session_channel;
	uint32_t			 m_session_reg_addr;
};


struct sl_version_info
{
	unsigned int days2 : 4;
	unsigned int days1 : 2;
	unsigned int month2 : 4;
	unsigned int month1 : 2;
	unsigned int year2 : 4;
	unsigned int year1 : 4;
	unsigned int version3 : 4;
	unsigned int version2 : 4;
	unsigned int version1 : 4;
};

#pragma pack(pop)

class sl_nic_proch : public i_slnic, public channel_reader_event
{
public:
	sl_nic_proch();
	~sl_nic_proch();

	const char *get_api_version();
	bool get_hw_version(slnic_hw_version_info &hw_info, int dev1, int dev2);
	bool get_driver_version(int dev1, int dev2);
	int get_sys_unifo();
	bool slnic_init(int dev);
	bool slnic_exit();

	bool slnic_fdir_config(st_session_group*config, slnic_event* ev, int dev2);
	bool slnic_delete_config(st_session_group *config);
	bool slnic_send(uint8_t mac_id, const char* data, short data_len);
	bool slnic_get_net_info(const char* eth_name, slnic_net_info *info);
	bool slnic_get_packet_statistics(mac_packet_statistics* p_mac_packet_statistics, int dev);
	bool slnic_start(int dev);
	bool slnic_stop(int dev);

public:
	void on_recv_msg(int channel, const char *data, int len, int dev);
	int bind_cpu(int channel);

public:
	int						 m_num_session[SL_MAX_SESSION_CHL];
	int						 m_bind_cpus[SL_MAX_RECV_CHANN];
	int						 m_bind_cpus1[SL_MAX_RECV_CHANN];
	slnic_event				*m_events[SL_MAX_RECV_CHANN];
	slnic_event				*m_events1[SL_MAX_RECV_CHANN];
	st_session_group		 m_session_cfg[8];
	reg_controler			 m_reg_ctl1;
	channel_controler		 m_channel_ctl1;
	open_ioctl_close		 m_oic;
	reg_controler			 m_reg_ctl2;
	channel_controler		 m_channel_ctl2;
	efh_loger				 m_logger;
	net_info				 m_net_info;
	int						 m_sequence_count;
};