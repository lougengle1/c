#include <linux/pci.h>
#include <linux/module.h>
#include <linux/init.h>
#include "sl_nic_netdev.h"
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h> /* sl_debug() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> /* mark_bh */
#include <linux/time.h>
#include <linux/rtc.h>
#include "sl_nic_define.h"
#include "sl_nic_dev.h"
#include "slk_tools/slk_kernel.h"
#include "slk_tools/slk_define.h"
#include <linux/netdevice.h>
#include "user_irq_define.h"
#include <linux/netpoll.h>



#define IOCTL_SLN 1

struct slk_channel_config_status
{
	struct slk_channel_config slk_config;
	int channel_status;
};

static
int	slnic_open(struct inode *pnode, struct file* pfile)
{
	printk("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	//����8��ͨ�����ڴ�
	pfile->private_data = kmalloc(sizeof(struct slk_channel_config_status) * MAX_CHANNEL_NUN, GFP_KERNEL);
	if (pfile->private_data == NULL)
	{
		printk("slnic_open failed");
		return -1;
	}

	memset(pfile->private_data, 0, sizeof(struct slk_channel_config_status) * MAX_CHANNEL_NUN);
	return 0;
}

static
int slnic_release(struct inode * pnode, struct file * pfile)
{
	struct rx_recover rx_rec_buff;
	long session_regs[6] = { CHL2_SESSION0_SIP, CHL3_SESSION0_SIP,
		CHL4_SESSION0_SIP, CHL5_SESSION0_SIP,
		CHL6_SESSION0_SIP, CHL7_SESSION0_SIP };
	int channel_status_count;
	int channel_count;
	int num_count;
	struct slk_channel_config_status *p_slk_channel_config_status = pfile->private_data;

	for (channel_status_count = 2; channel_status_count < 8; channel_status_count++)
	{
		//�жϱ����ַ��豸ͨ���Ƿ�ʹ��
		struct slk_channel_config *config = &p_slk_channel_config_status[channel_status_count].slk_config;
		if (p_slk_channel_config_status[channel_status_count].channel_status != 1)
			continue;
		
		//��ȡ�ڴ������õ�ͨ��
		channel_count = config->m_channel;
		//�ı�ͨ��״̬��ֵ
		g_driver_data.channel_status[channel_count] = 0;

		//��λͨ����ָ��session������
		for (num_count = 0; num_count < config->m_session_num; num_count++)
		{
			uint32_t m_session_reg_addr = session_regs[channel_count - 2] + num_count * 0x0040;

			uint32_t session_s_mac_addr_l = m_session_reg_addr;
			uint32_t session_s_mac_l_and_dh = session_s_mac_addr_l + 0x08;
			uint32_t session_d_mac_h = session_s_mac_addr_l + 0x010;
			uint32_t session_sip_addr = session_s_mac_addr_l + 0x18;
			uint32_t session_sport_addr = session_s_mac_addr_l + 0x20;
			uint32_t session_dip_addr = session_s_mac_addr_l + 0x28;
			uint32_t session_dport_addr = session_s_mac_addr_l + 0x30;
			uint32_t session_en_clt_addr = session_s_mac_addr_l + 0x38;


			WRITE_REG(g_driver_data.m_mem_base0, session_s_mac_addr_l, 0);
			WRITE_REG(g_driver_data.m_mem_base0, session_s_mac_l_and_dh, 0);
			WRITE_REG(g_driver_data.m_mem_base0, session_d_mac_h, 0);
			WRITE_REG(g_driver_data.m_mem_base0, session_sip_addr, 0);
			WRITE_REG(g_driver_data.m_mem_base0, session_sport_addr, 0);
			WRITE_REG(g_driver_data.m_mem_base0, session_dip_addr, 0);
			WRITE_REG(g_driver_data.m_mem_base0, session_dport_addr, 0);
			WRITE_REG(g_driver_data.m_mem_base0, session_en_clt_addr, 0);
		}

		//ɾ��ͨ��֮�󣬶���ĳ��ͨ��������
		rx_rec_buff.buffer = g_driver_data.m_rx_channel.m_dma_chan[channel_count].m_virt_addr_start;

		//�����Ҫ�ͷŽڵ��±�
		rx_rec_buff.m_next_chunk = (g_driver_data.m_rx_seqs[channel_count].m_info.m_read_seq)&(g_driver_data.m_rx_channel.m_dma_chan[channel_count].m_node_num-1);
		//��ȡ��ǰ�������
		rx_rec_buff.m_chunk_count = g_driver_data.m_rx_seqs[channel_count].m_info.m_read_seq;

		//��һ���ڵ���������
		rx_rec_buff.m_chunk_count = rx_rec_buff.m_chunk_count + 1;


		while (1)
		{
			//�������ȣ�˵��session����ȡ��֮������ͨ���������Ѿ������
			if (rx_rec_buff.buffer[rx_rec_buff.m_next_chunk].info.m_chunk_count != rx_rec_buff.m_chunk_count)
			{
				sl_debug("not soft_sounter:%d,hard_counter:%d\n", rx_rec_buff.m_chunk_count, rx_rec_buff.buffer[rx_rec_buff.m_next_chunk].info.m_chunk_count);
				g_driver_data.m_rx_seqs[channel_count].m_info.m_read_seq = rx_rec_buff.m_chunk_count - 1;
				break;
			}

			WRITE_REG(g_driver_data.m_mem_base0, SW_PKT_CUR_MEM_OFF0 + SW_PKT_CUR_MEM_OFFSET * channel_count, rx_rec_buff.m_next_chunk);

			rx_rec_buff.m_next_chunk = (rx_rec_buff.m_next_chunk + 1) & (g_driver_data.m_rx_channel.m_dma_chan[channel_count].m_node_num - 1);
			rx_rec_buff.m_chunk_count = (rx_rec_buff.m_chunk_count + 1) & 0xffff;
		}
	}


	
	
	if (NULL != pfile->private_data)
		kfree(pfile->private_data);
	return 0;
}

unsigned int ip_str_to_num(const char *buf)
{
	unsigned int tmpip[4] = { 0 };
	unsigned int tmpip32 = 0;
	sscanf(buf, "%d.%d.%d.%d", &tmpip[0], &tmpip[1], &tmpip[2], &tmpip[3]);
	tmpip32 = (tmpip[3] << 24) | (tmpip[2] << 16) | (tmpip[1] << 8) | tmpip[0];
	return tmpip32;
}

static void do_config_session(struct hw_session_cnf cnf)
{
	int s_mac_addr[6] = { 0 };
	int d_mac_addr[6] = { 0 };

	const char	*sip = "0";
	const char	*dip = "0";
	uint16_t	 sport = 0;
	uint16_t	 dport = 0;

	uint32_t en_ctl = 0;
	uint32_t s_mac_addr_h = 0;
	uint32_t s_mac_l_and_dh = 0;
	uint32_t d_mac_addr_l = 0;

	uint32_t session_s_mac_addr_l;
	uint32_t session_s_mac_l_and_dh;
	uint32_t session_d_mac_h;
	uint32_t session_sip_addr;
	uint32_t session_sport_addr;
	uint32_t session_dip_addr;
	uint32_t session_dport_addr;
	uint32_t session_en_clt_addr;

	sscanf(cnf.m_s_mac, "%x:%x:%x:%x:%x:%x", &s_mac_addr[0], &s_mac_addr[1], &s_mac_addr[2], &s_mac_addr[3], &s_mac_addr[4], &s_mac_addr[5]);
	sscanf(cnf.m_d_mac, "%x:%x:%x:%x:%x:%x", &d_mac_addr[0], &d_mac_addr[1], &d_mac_addr[2], &d_mac_addr[3], &d_mac_addr[4], &d_mac_addr[5]);
	sip = cnf.m_src_ip;
	dip = cnf.m_dst_ip;
	sport = cnf.m_src_port;
	dport = cnf.m_dst_port;

	s_mac_addr_h = (s_mac_addr[3]) | (s_mac_addr[2] << 8) | (s_mac_addr[1] << 16) | (s_mac_addr[0] << 24);
	s_mac_l_and_dh = (d_mac_addr[1]) | (d_mac_addr[0] << 8) | (s_mac_addr[5] << 16) | (s_mac_addr[4] << 24);
	d_mac_addr_l = (d_mac_addr[5]) | (d_mac_addr[4] << 8) | (d_mac_addr[3] << 16) | (d_mac_addr[2] << 24);


	session_s_mac_addr_l = cnf.m_session_reg_addr;
	session_s_mac_l_and_dh = session_s_mac_addr_l + 0x08;
	session_d_mac_h = session_s_mac_addr_l + 0x010;
	session_sip_addr = session_s_mac_addr_l + 0x18;
	session_sport_addr = session_s_mac_addr_l + 0x20;
	session_dip_addr = session_s_mac_addr_l + 0x28;
	session_dport_addr = session_s_mac_addr_l + 0x30;
	session_en_clt_addr = session_s_mac_addr_l + 0x38;


	if (cnf.m_session_tcp)
		en_ctl = 06 << 24;		//����tcp����
	else
		en_ctl = 17 << 24;		//����udp����

	if (cnf.m_mac == 1)			//����mac0����
	{
		en_ctl |= 0 << 20;
		en_ctl |= 1 << 14;
	}

	if (cnf.m_mac == 2)			//����mac1����
	{
		en_ctl |= 1 << 20;
		en_ctl |= 1 << 14;
	}

	if (cnf.m_enable_smac)		//����smac����
		en_ctl |= 1 << 13;
	if (cnf.m_enable_dmac)		//����dmac����
		en_ctl |= 1 << 12;
	if (cnf.m_enable_src_ip)	//����src_ip����
		en_ctl |= 1 << 11;
	if (cnf.m_enable_src_port)	//����src_port����
		en_ctl |= 1 << 10;
	if (cnf.m_enable_dst_ip)	//����d_ip����
		en_ctl |= 1 << 9;
	if (cnf.m_enable_dst_port)	//����d_port����
		en_ctl |= 1 << 8;
	en_ctl |= 1;

	WRITE_REG(g_driver_data.m_mem_base0,session_s_mac_addr_l, s_mac_addr_h);
	WRITE_REG(g_driver_data.m_mem_base0,session_s_mac_l_and_dh, s_mac_l_and_dh);
	WRITE_REG(g_driver_data.m_mem_base0,session_d_mac_h, d_mac_addr_l);
	WRITE_REG(g_driver_data.m_mem_base0,session_sip_addr, ntohl(ip_str_to_num(sip)));
	WRITE_REG(g_driver_data.m_mem_base0,session_sport_addr, /*htons*/(sport));
	WRITE_REG(g_driver_data.m_mem_base0,session_dip_addr, ntohl(ip_str_to_num(dip)));
	WRITE_REG(g_driver_data.m_mem_base0,session_dport_addr, /*htons*/(dport));
	WRITE_REG(g_driver_data.m_mem_base0,session_en_clt_addr, en_ctl);

	sl_debug("[en_ctl:%08x] \n", en_ctl);
}

static
long slnic_data_ioctl(struct file * pfile, unsigned int cmd, unsigned long arg)
{
	unsigned int port_num;
	int ret = 0;
	int channel_count;
	int num_count = 0;
	struct hw_session_cnf scnf;

	long session_regs[6] = { CHL2_SESSION0_SIP, CHL3_SESSION0_SIP,
		CHL4_SESSION0_SIP, CHL5_SESSION0_SIP,
		CHL6_SESSION0_SIP, CHL7_SESSION0_SIP };

	struct slk_channel_config_status * p_slk_channel_config_status = pfile->private_data;
	struct slk_channel_config *config = kmalloc(sizeof(struct slk_channel_config), GFP_KERNEL);
	memset(config, 0, sizeof(struct slk_channel_config));

	switch (cmd)
	{
	case SLK_IOCTL_CONFIG_CHANNEL:
		{
			if (copy_from_user(config, (void*)arg, /*MAX_CHANNEL_NUN */sizeof(struct slk_channel_config)) != 0)
				return -EFAULT;
			
			//������������ĸ�ͨ��
			channel_count = config->m_channel;

			//�ж�ȫ��ͨ���Ƿ�ռ��
			if (g_driver_data.channel_status[channel_count] & 1)
				return -EFAULT;

			//����ȫ��ͨ��״̬������������ͨ����ռ��
			g_driver_data.channel_status[channel_count] = 1;

			//���þֲ�ͨ��״̬��Ϣ
			memset(&p_slk_channel_config_status[channel_count].slk_config, 0, sizeof(struct slk_channel_config));
			memcpy(&p_slk_channel_config_status[channel_count].slk_config, config, sizeof(struct slk_channel_config));
			p_slk_channel_config_status[channel_count].channel_status = 1;
			
			//����ͨ����session��Ϣ
			for (num_count = 0; num_count < config->m_session_num; num_count++)
			{
				memset(&scnf, 0, sizeof(scnf));
				//����session��Ϣ
				scnf.m_s_mac = config->m_session_config[num_count].m_smac;
				scnf.m_d_mac = config->m_session_config[num_count].m_dmac;
				scnf.m_src_ip = config->m_session_config[num_count].m_sip;
				scnf.m_src_port = config->m_session_config[num_count].m_sport;
				scnf.m_dst_ip = config->m_session_config[num_count].m_dip;
				scnf.m_dst_port = config->m_session_config[num_count].m_dport;
				scnf.m_session_tcp = config->m_session_config[num_count].m_net_type;
				scnf.m_mac = config->m_session_config[num_count].m_mac;


				sl_debug("[m_s_mac:%s] [m_d_mac:%s] [m_src_ip:%s] [m_dst_ip:%s] [m_src_port:%d] [m_dst_port:%d] [m_session_tcp:%d] [m_mac:%d]\n",
					scnf.m_s_mac,
					scnf.m_d_mac,
					scnf.m_src_ip,
					scnf.m_dst_ip,
					scnf.m_src_port,
					scnf.m_dst_port,
					scnf.m_session_tcp,
					scnf.m_mac);

				//���������Ϣ
				scnf.m_enable_smac = strlen(scnf.m_s_mac) != 0;
				scnf.m_enable_dmac = strlen(scnf.m_d_mac) != 0;
				scnf.m_enable_src_ip = strlen(scnf.m_src_ip) != 0;
				scnf.m_enable_dst_ip = strlen(scnf.m_dst_ip) != 0;
				scnf.m_enable_src_port = scnf.m_src_port != 0;
				scnf.m_enable_dst_port = scnf.m_dst_port != 0;


				sl_debug("[m_enable_smac:%d] [m_enable_dmac:%d] [m_enable_src_ip:%d] [m_enable_dst_ip:%d] [m_enable_src_port:%d] [m_enable_dst_port:%d]\n",
					scnf.m_enable_smac,
					scnf.m_enable_dmac,
					scnf.m_enable_src_ip,
					scnf.m_enable_dst_ip,
					scnf.m_enable_src_port,
					scnf.m_enable_dst_port);
				scnf.m_session_reg_addr = session_regs[channel_count - 2] + num_count * 0x0040;
				do_config_session(scnf);
			}
			if (NULL != config)
			{
				kfree(config);
			}
		}
		break;
	case IOCTL_SLN:
		{
			struct slnic_device_info info;
			struct net_device *ndev;
			struct slk_net_priv *priv;
			if (copy_from_user(&info, (void*)arg, sizeof(info)) != 0)
				return -EFAULT;

			port_num = info.port;
			ndev = g_driver_data.ndev[port_num];
			priv = netdev_priv(ndev);
			info.tx_packets = priv->stats.tx_packets;
			info.tx_bytes = priv->stats.tx_bytes;
			info.tx_dropped = priv->stats.tx_dropped;
			info.tx_errors = priv->stats.tx_errors;

			info.rx_packets = priv->stats.rx_packets;
			info.rx_bytes = priv->stats.rx_bytes;
			info.rx_errors = priv->stats.rx_errors;
			info.rx_enable = priv->m_rx_enable;
			if (copy_to_user((void *)arg, &info, sizeof(info)) != 0)
				ret = -EFAULT;
		}
		break;
	default:
		ret = -EFAULT;
		break;
	}

	return ret;
}


struct file_operations g_fop_slnic =
{
	.owner = THIS_MODULE,
	.open = slnic_open,
	.release = slnic_release,
	.unlocked_ioctl = slnic_data_ioctl,
};
