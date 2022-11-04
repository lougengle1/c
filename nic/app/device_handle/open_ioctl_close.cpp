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
#include "open_ioctl_close.h"

open_ioctl_close::open_ioctl_close()
{
	m_slnic_fd_1 = -1;
	m_slnic_fd_2 = -1;
}

open_ioctl_close::~open_ioctl_close()
{

}

bool open_ioctl_close::get_slnic_fd(int dev)
{
	if (dev == 1)
	{
		m_slnic_fd_1 = open(NIC_CTL_DEVICE1, O_RDWR);
		if (m_slnic_fd_1 < 0)
		{
			printf("dev1 open failed!\n");
			return false;
		}
	}
	else if (dev == 2)
	{
		m_slnic_fd_2 = open(NIC_CTL_DEVICE2, O_RDWR);
		if (m_slnic_fd_2 < 0)
		{
			printf("dev2 open failed!\n");
			return false;
		}
	}

	return true;
}

void open_ioctl_close::close_slnic_fd()
{
	if (m_slnic_fd_1 != 0)
	{
		close(m_slnic_fd_1);
	}
	if (m_slnic_fd_2 != 0)
	{
		close(m_slnic_fd_2);
	}
}

bool open_ioctl_close::slnic_ioctl(int channel_id, st_session_group*  config,int dev)
{
	if (dev == 1)
	{
		if (ioctl(m_slnic_fd_1, SLK_IOCTL_CONFIG_CHANNEL, config) != 0)
		{
			printf("slnic ioctl is error [m_slnic_fd:%d]\n", m_slnic_fd_1);
			return false;
		}
	}

	if (dev == 2)
	{
		if (ioctl(m_slnic_fd_2, SLK_IOCTL_CONFIG_CHANNEL, config) != 0)
		{
			printf("slnic ioctl is error [m_slnic_fd:%d]\n", m_slnic_fd_2);
			return false;
		}
	}
	return true;
}