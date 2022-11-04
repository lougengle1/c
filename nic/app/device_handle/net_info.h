#pragma once
#include <string.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

struct net_info_config
{
	char					m_name[6];
	struct sockaddr_in		sin;
	unsigned char			m_mac[6];
	struct sockaddr_in		netmask;
	bool					m_running_flags;

};

class net_info
{
public:
	net_info();
	~net_info();

	bool get_net_info(const char* name, net_info_config*m_net_info);
	bool get_slnic_status(int fd ,net_info_config*m_net_info,struct ifreq ifr);
	bool get_slnic_ip(int fd, net_info_config*m_net_info, struct ifreq ifr);
	bool get_slnic_mac(int fd, net_info_config*m_net_info, struct ifreq ifr);
	bool get_slnic_netmask(int fd, net_info_config*m_net_info, struct ifreq ifr);

public:
	net_info_config					m_net_info;
	int								m_sock_fd;
};