#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "slnic.h"

using std::string;


#define CHERRY_TRADING_CLIENT_MSG_LEN							2048
#define	CHERRY_DRIVER_TRANSMISSION_FLAG							'M'		

i_slnic *i_slnic::create_handle()
{
	return new sl_nic_proch;
}

sl_nic_proch::sl_nic_proch()
{
	memset(m_events, 0, sizeof(slnic_event)*SL_MAX_RECV_CHANN);
	memset(m_events1, 0, sizeof(slnic_event)*SL_MAX_RECV_CHANN);
	memset(&m_num_session, 0, sizeof(m_num_session));
	m_sequence_count = 0;
}

sl_nic_proch::~sl_nic_proch()
{
	m_oic.close_slnic_fd();
	m_channel_ctl1.exit();
	m_channel_ctl2.exit();
	m_reg_ctl1.exit();
	m_reg_ctl2.exit();
}

const char * sl_nic_proch::get_api_version()
{
	string svn_version;
	svn_version = SVN_VERSION;
	return svn_version.c_str();
}


bool sl_nic_proch::get_hw_version(slnic_hw_version_info &hw_info, int dev1, int dev2)
{
	if (dev1 == 1)
	{
		void* bar0_addr = m_reg_ctl1.get_bar0_address();

		if (bar0_addr == NULL)
			return false;	

		hw_info = *((struct slnic_hw_version_info*)(bar0_addr + SYS_ID));
		printf("dev1 production:%d Edition%x\n", hw_info.production, ntohs(hw_info.edition));	
	}
	memset(&hw_info, 0, sizeof(slnic_hw_version_info));
	if (dev2 == 1)
	{
		void* bar0_addr = m_reg_ctl1.get_bar0_address();

		if (bar0_addr == NULL)
			return false;

		hw_info = *((struct slnic_hw_version_info*)(bar0_addr + SYS_ID));
		printf("dev2 production:%d Edition%x\n", hw_info.production, ntohs(hw_info.edition));
	}

	return true;
}


bool sl_nic_proch::get_driver_version(int dev1, int dev2)
{
	if (dev1 == 1)
		printf("driver_vsersion:%d\n", m_channel_ctl1.get_version());
	if (dev2 == 2)
		printf("driver_vsersion:%d\n", m_channel_ctl2.get_version());
	return true;
}

int sl_nic_proch::get_sys_unifo()
{
	unsigned int sys_uinfo1;
	m_reg_ctl1.read_reg(SLK_SYS_UINFO1, sys_uinfo1);
	return sys_uinfo1;
}



bool sl_nic_proch::slnic_init(int dev)
{
	if (dev == 0)
	{
		return true;
	}
	unsigned int sys_id, sys_seq_num;;
	m_logger.init("sl_nic_porch");
	//m_logger.write_log_info("sl_nic_porch sw version %s", get_version());

	if (m_oic.get_slnic_fd(dev) == -1)
	{
		m_logger.write_log_error("get_slnic_fd  error!");
		return false;
	}

	if (dev == 1)
	{
		if (!m_reg_ctl1.init(REG_CTL_DEVICE1))
		{
			m_logger.write_log_error("reg_ctl init error!");
			return false;
		}

		m_reg_ctl1.read_reg(SLK_SYS_ID, sys_id);
		m_reg_ctl1.read_reg(SLK_SYS_SEQU_NUM, sys_seq_num);
		m_logger.write_log_info("sl_nic_porch hw sys_id %08x, sys_seq_num %08x", sys_id, sys_seq_num);

		m_logger.write_log_info("reg_ctl init ok!");

		if (!m_channel_ctl1.init(STREAM_DEVICE1, &m_reg_ctl1))
		{
			m_logger.write_log_error("channel_ctl init error!");
			return false;
		}
	}
	if (dev == 2)
	{

		if (!m_reg_ctl2.init(REG_CTL_DEVICE2))
		{
			m_logger.write_log_error("reg_ctl init error!");
			return false;
		}

		m_reg_ctl2.read_reg(SLK_SYS_ID, sys_id);
		m_reg_ctl2.read_reg(SLK_SYS_SEQU_NUM, sys_seq_num);
		m_logger.write_log_info("sl_nic_porch hw sys_id %08x, sys_seq_num %08x", sys_id, sys_seq_num);

		m_logger.write_log_info("reg_ctl init ok!");

		if (!m_channel_ctl2.init(STREAM_DEVICE2, &m_reg_ctl2))
		{
			m_logger.write_log_error("channel_ctl init error!");
			return false;
		}
	}

	m_logger.write_log_info("channel_ctl init ok!");
	return true;
}

bool sl_nic_proch::slnic_exit()
{
	m_oic.close_slnic_fd();
	m_channel_ctl1.exit();
	m_channel_ctl2.exit();
	m_reg_ctl1.exit();
	m_reg_ctl2.exit();

	return true;
}

bool sl_nic_proch::slnic_fdir_config(st_session_group * config,slnic_event *ev, int dev)
{
	if (dev == 0)
		return true;
	int channel = -1;
	int bind_cpu = -1;

	if (!m_oic.slnic_ioctl(SLK_IOCTL_CONFIG_CHANNEL, config, dev))
	{
		return false;
	}
	channel = config->m_channel;
	bind_cpu = config->m_cpu;

	if (dev == 1)
	{
		m_channel_ctl1.regist_channel(channel, this);
		m_bind_cpus[channel] = bind_cpu;
		m_events[channel] = ev;
	}
	if (dev == 2)
	{
		m_channel_ctl2.regist_channel(channel, this);
		m_bind_cpus1[channel] = bind_cpu;
		m_events1[channel] = ev;
	}
	return true;
}

bool sl_nic_proch::slnic_delete_config(st_session_group * config)
{
	long session_regs[6] = { CHL2_SESSION0_SIP, CHL3_SESSION0_SIP,
		CHL4_SESSION0_SIP, CHL5_SESSION0_SIP,
		CHL6_SESSION0_SIP, CHL7_SESSION0_SIP };
	int num_count;
	int channel = config->m_channel;
	int session_num = config->m_session_num;

	for (num_count = 0; num_count < session_num; num_count++)
	{
		uint32_t m_session_reg_addr = session_regs[channel - 2] + num_count * 0x0040;
		uint32_t session_s_mac_addr_l = m_session_reg_addr;
		uint32_t session_s_mac_l_and_dh = session_s_mac_addr_l + 0x08;
		uint32_t session_d_mac_h = session_s_mac_addr_l + 0x010;
		uint32_t session_sip_addr = session_s_mac_addr_l + 0x18;
		uint32_t session_sport_addr = session_s_mac_addr_l + 0x20;
		uint32_t session_dip_addr = session_s_mac_addr_l + 0x28;
		uint32_t session_dport_addr = session_s_mac_addr_l + 0x30;
		uint32_t session_en_clt_addr = session_s_mac_addr_l + 0x38;

		m_reg_ctl1.write_reg(session_s_mac_addr_l, 0);
		m_reg_ctl1.write_reg(session_s_mac_l_and_dh, 0);
		m_reg_ctl1.write_reg(session_d_mac_h, 0);
		m_reg_ctl1.write_reg(session_sip_addr, 0);
		m_reg_ctl1.write_reg(session_sport_addr, 0);
		m_reg_ctl1.write_reg(session_dip_addr, 0);
		m_reg_ctl1.write_reg(session_dport_addr, 0);
		m_reg_ctl1.write_reg(session_en_clt_addr, 0);

		m_reg_ctl2.write_reg(session_s_mac_addr_l, 0);
		m_reg_ctl2.write_reg(session_s_mac_l_and_dh, 0);
		m_reg_ctl2.write_reg(session_d_mac_h, 0);
		m_reg_ctl2.write_reg(session_sip_addr, 0);
		m_reg_ctl2.write_reg(session_sport_addr, 0);
		m_reg_ctl2.write_reg(session_dip_addr, 0);
		m_reg_ctl2.write_reg(session_dport_addr, 0);
		m_reg_ctl2.write_reg(session_en_clt_addr, 0);

	}
	return true;
}

void packet_driver_head(driver_transmission_head& driver_head, int msg_len, int seq)
{
	driver_head.m_flag = CHERRY_DRIVER_TRANSMISSION_FLAG;
	driver_head.m_seq = seq;
	driver_head.m_len = msg_len;
}

bool sl_nic_proch::slnic_send(uint8_t mac_id, const char* data, short data_len)
{
	int offset = 0;
	int nlen = sizeof(driver_to_hardware_head) + data_len;
	char	ch_buff[CHERRY_TRADING_CLIENT_MSG_LEN];

	///<驱动发送数据给板卡的消息头
	driver_transmission_head* driver_head = (driver_transmission_head*)(ch_buff);
	packet_driver_head(*driver_head, nlen, m_sequence_count);
	offset += sizeof(driver_transmission_head);

	///<软件发往硬件数据头结构
	driver_to_hardware_head *head_s2u = (driver_to_hardware_head*)(&ch_buff[offset]);
	head_s2u->m_channel_id = mac_id;
	head_s2u->m_payload_lenth = ntohs(data_len);
	offset += sizeof(driver_to_hardware_head);

	memcpy(&ch_buff[offset], data, data_len);
	
	m_channel_ctl1.write_data(ch_buff,offset+data_len);

	m_sequence_count++;

	return true;
}

bool sl_nic_proch::slnic_get_net_info(const char* eth_name, slnic_net_info * info)
{
	m_net_info.get_net_info(eth_name, (struct net_info_config*)info);
	return true;
}

bool sl_nic_proch::slnic_get_packet_statistics(mac_packet_statistics* p_mac_packet_statistics,int dev)
{
	unsigned int mac0_rx_pkt_num_h;
	unsigned int mac0_rx_pkt_num_l;
	unsigned int mac0_rx_pkt_desert_num_h;
	unsigned int mac0_rx_pkt_desert_num_l;
	unsigned int mac0_tx_pkt_num_h;
	unsigned int mac0_tx_pkt_num_l;
 
	unsigned int mac1_rx_pkt_num_h;
	unsigned int mac1_rx_pkt_num_l;
	unsigned int mac1_rx_pkt_desert_num_h;
	unsigned int mac1_rx_pkt_desert_num_l;
	unsigned int mac1_tx_pkt_num_h;
	unsigned int mac1_tx_pkt_num_l;
	
	if (dev == 1)
	{
		//NF10
		m_reg_ctl1.read_reg(MAC0_RX_PKT_NUM_H, mac0_rx_pkt_num_h);
		m_reg_ctl1.read_reg(MAC0_RX_PKT_NUM_L, mac0_rx_pkt_num_l);
		p_mac_packet_statistics->mac0_rx_pkt_num = mac0_rx_pkt_num_l + ((unsigned long)mac0_rx_pkt_num_h << 32);

		m_reg_ctl1.read_reg(MAC0_RX_PKT_DESERT_NUM_H, mac0_rx_pkt_desert_num_h);
		m_reg_ctl1.read_reg(MAC0_RX_PKT_DESERT_NUM_L, mac0_rx_pkt_desert_num_l);
		p_mac_packet_statistics->mac0_rx_pkt_desert_num = mac0_rx_pkt_desert_num_l + ((unsigned long)mac0_rx_pkt_desert_num_h << 32);

		m_reg_ctl1.read_reg(MAC0_TX_PKT_NUM_H, mac0_tx_pkt_num_h);
		m_reg_ctl1.read_reg(MAC0_TX_PKT_NUM_L, mac0_tx_pkt_num_l);
		p_mac_packet_statistics->mac0_tx_pkt_num = mac0_tx_pkt_num_l + ((unsigned long)mac0_tx_pkt_num_h << 32);

		m_reg_ctl1.read_reg(MAC1_RX_PKT_NUM_H, mac1_rx_pkt_num_h);
		m_reg_ctl1.read_reg(MAC1_RX_PKT_NUM_L, mac1_rx_pkt_num_l);
		p_mac_packet_statistics->mac1_rx_pkt_num = mac1_rx_pkt_num_l + ((unsigned long)mac1_rx_pkt_num_h << 32);

		m_reg_ctl1.read_reg(MAC1_RX_PKT_DESERT_NUM_H, mac1_rx_pkt_desert_num_h);
		m_reg_ctl1.read_reg(MAC1_RX_PKT_DESERT_NUM_L, mac1_rx_pkt_desert_num_l);
		p_mac_packet_statistics->mac1_rx_pkt_desert_num = mac1_rx_pkt_desert_num_l + ((unsigned long)mac1_rx_pkt_desert_num_h << 32);

		m_reg_ctl1.read_reg(MAC1_TX_PKT_NUM_H, mac1_tx_pkt_num_h);
		m_reg_ctl1.read_reg(MAC1_TX_PKT_NUM_L, mac1_tx_pkt_num_l);
		p_mac_packet_statistics->mac1_tx_pkt_num = mac1_tx_pkt_num_l + ((unsigned long)mac1_tx_pkt_num_h << 32);

	}
	
	if (dev == 2)
	{
		//NF5
		m_reg_ctl2.read_reg(MAC0_RX_PKT_NUM_H, mac0_rx_pkt_num_h);
		m_reg_ctl2.read_reg(MAC0_RX_PKT_NUM_L, mac0_rx_pkt_num_l);
		p_mac_packet_statistics->mac0_rx_pkt_num = mac0_rx_pkt_num_l + ((unsigned long)mac0_rx_pkt_num_h << 32);

		m_reg_ctl2.read_reg(MAC0_RX_PKT_DESERT_NUM_H, mac0_rx_pkt_desert_num_h);
		m_reg_ctl2.read_reg(MAC0_RX_PKT_DESERT_NUM_L, mac0_rx_pkt_desert_num_l);
		p_mac_packet_statistics->mac0_rx_pkt_desert_num = mac0_rx_pkt_desert_num_l + ((unsigned long)mac0_rx_pkt_desert_num_h << 32);

		m_reg_ctl2.read_reg(MAC0_TX_PKT_NUM_H, mac0_tx_pkt_num_h);
		m_reg_ctl2.read_reg(MAC0_TX_PKT_NUM_L, mac0_tx_pkt_num_l);
		p_mac_packet_statistics->mac0_tx_pkt_num = mac0_tx_pkt_num_l + ((unsigned long)mac0_tx_pkt_num_h << 32);

		m_reg_ctl2.read_reg(MAC1_RX_PKT_NUM_H, mac1_rx_pkt_num_h);
		m_reg_ctl2.read_reg(MAC1_RX_PKT_NUM_L, mac1_rx_pkt_num_l);
		p_mac_packet_statistics->mac1_rx_pkt_num = mac1_rx_pkt_num_l + ((unsigned long)mac1_rx_pkt_num_h << 32);

		m_reg_ctl2.read_reg(MAC1_RX_PKT_DESERT_NUM_H, mac1_rx_pkt_desert_num_h);
		m_reg_ctl2.read_reg(MAC1_RX_PKT_DESERT_NUM_L, mac1_rx_pkt_desert_num_l);
		p_mac_packet_statistics->mac1_rx_pkt_desert_num = mac1_rx_pkt_desert_num_l + ((unsigned long)mac1_rx_pkt_desert_num_h << 32);

		m_reg_ctl2.read_reg(MAC1_TX_PKT_NUM_H, mac1_tx_pkt_num_h);
		m_reg_ctl2.read_reg(MAC1_TX_PKT_NUM_L, mac1_tx_pkt_num_l);
		p_mac_packet_statistics->mac1_tx_pkt_num = mac1_tx_pkt_num_l + ((unsigned long)mac1_tx_pkt_num_h << 32);

	}
	
	return true;
}

bool sl_nic_proch::slnic_start(int dev)
{
	if (dev == 1)
	{
		if (!m_channel_ctl1.start(dev))
		{
			return false;
		}
	}
	if (dev == 2)
	{
		if (!m_channel_ctl2.start(dev))
		{
			return false;
		}
	}

	return true;
}

bool sl_nic_proch::slnic_stop(int dev)
{
	if (dev == 1)
	{
		if (!m_channel_ctl1.stop())
		{
			return false;
		}
	}

	if (dev == 2)
	{
		if (!m_channel_ctl2.stop())
		{
			return false;
		}
	}

	return true;
}

void sl_nic_proch::on_recv_msg(int channel, const char * data, int len, int dev)
{

	if (m_events[channel] != NULL && dev == 1)
	{
		m_events[channel]->on_data_recv(channel, data, len);
	}
	if (m_events1[channel] != NULL && dev == 2)
	{
		m_events1[channel]->on_data_recv(channel, data, len);
	}
	
}

int sl_nic_proch::bind_cpu(int channel)
{
	return m_bind_cpus[channel];
}

/*
bool sl_nic_proch::get_slnic_fd(int dev)
{
	if (dev == 1)
	{
		m_slnic_fd = open(NIC_CTL_DEVICE1, O_RDWR);
		if (m_slnic_fd < 0)
			return false;
	}
	else
	{
		m_slnic_fd = open(NIC_CTL_DEVICE2, O_RDWR);
		if (m_slnic_fd < 0)
			return false;
	}

	return true;
}

void sl_nic_proch::close_slnic_fd()
{
	close(m_slnic_fd);
}
*/


