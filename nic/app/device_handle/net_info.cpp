#include "net_info.h"


net_info::net_info()
{
	m_sock_fd = -1;
	memset(&m_net_info, 0, sizeof(net_info_config));
}

net_info::~net_info()
{
	close(m_sock_fd);
}

bool net_info::get_net_info(const char* name, net_info_config* m_net_info)
{

	struct ifreq ifr;
	
	m_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(ifr.ifr_name,name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;


	if (!get_slnic_status(m_sock_fd, m_net_info, ifr))
	{
		printf("get_slnic_status error\n");
		return false;
	}

	if (!get_slnic_ip(m_sock_fd, m_net_info, ifr))
	{
		printf("get_slnic_ip error\n");
		return false;
	}

	if (!get_slnic_mac(m_sock_fd, m_net_info, ifr))
	{
		printf("get_slnic_mac error\n");
		return false;
	}

	if (!get_slnic_netmask(m_sock_fd, m_net_info, ifr))
	{
		printf("get_slnic_netmask error\n");
		return false;
	}

	return true;
}

bool net_info::get_slnic_status(int fd, net_info_config * m_net_info, struct ifreq ifr)
{
	int i_ret =-1;
	if ((i_ret=ioctl(m_sock_fd, SIOCGIFFLAGS, &ifr)) == 0)
	{
		if (ifr.ifr_flags & IFF_RUNNING)
		{
			m_net_info->m_running_flags = true;
		}
		else
		{
			m_net_info->m_running_flags = false;
		}
	}
	else if(i_ret<0)
	{
		close(fd);
		return false;
	}

	return true;
}

bool net_info::get_slnic_ip(int fd, net_info_config * m_net_info, struct ifreq ifr)
{
	int i_ret = -1;
	if ((i_ret=ioctl(fd, SIOCGIFADDR, &ifr)) == 0)							//获取ip
	{
		memcpy(&m_net_info->sin, &ifr.ifr_addr, sizeof(m_net_info->sin));
		//printf("slnic0: %s\n", inet_ntoa(m_net_info->sin.sin_addr));
	}
	else if(i_ret < 0)
	{
		close(fd);
		return false;
	}

	return true;
}

bool net_info::get_slnic_mac(int fd, net_info_config * m_net_info, struct ifreq ifr)
{
	int i_ret = -1;
	if ((i_ret = ioctl(fd, SIOCGIFADDR, &ifr)) == 0)							//获取mac
	{
		memcpy(m_net_info->m_mac, ifr.ifr_hwaddr.sa_data, 6);
		//printf("hardware address %x:%x:%x:%x:%x:%x\n",
		//	m_net_info->m_mac[0], m_net_info->m_mac[1], m_net_info->m_mac[2],
		//	m_net_info->m_mac[3], m_net_info->m_mac[4], m_net_info->m_mac[5]);
	}
	else if (i_ret < 0)
	{
		close(fd);
		return false;
	}
	return true;
}

bool net_info::get_slnic_netmask(int fd, net_info_config * m_net_info,struct ifreq ifr)
{
	int i_ret = -1;
	if ((i_ret = ioctl(fd, SIOCGIFADDR, &ifr)) == 0)
	{
		memcpy(&m_net_info->netmask, &ifr.ifr_netmask, sizeof(m_net_info->netmask));
		//printf("netmask: %s\n", inet_ntoa(m_net_info->netmask.sin_addr));
	}
	else if (i_ret < 0)
	{
		close(fd);
		return false;
	}
	return true;
}
