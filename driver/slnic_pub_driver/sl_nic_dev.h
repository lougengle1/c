#ifndef SLNIC_CDEV_H
#define SLNIC_CDEV_H
#include <linux/cdev.h>
#include <linux/fs.h>

extern struct file_operations g_fop_slnic;

struct hw_session_cnf
{
	char				*m_s_mac;
	char				*m_d_mac;
	char				*m_src_ip;
	char				*m_dst_ip;

	uint16_t			 m_src_port;
	uint16_t			 m_dst_port;

	bool				 m_session_tcp;
	int					 m_mac;

	bool				 m_enable_smac;
	bool				 m_enable_dmac;
	bool				 m_enable_src_ip;
	bool				 m_enable_src_port;
	bool				 m_enable_dst_ip;
	bool				 m_enable_dst_port;

	int					 m_session_channel;
	uint32_t			 m_session_reg_addr;
};

struct rx_rece_chunk
{
	struct hardware_to_driver_head_info info;
	char pay_load[SLNIC_PALOAD_SIZE];
};

struct rx_recover
{
	volatile struct rx_rece_chunk* buffer;
	unsigned int			  m_next_chunk;
	unsigned short			  m_chunk_count;
};

#endif
