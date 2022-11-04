#pragma once
#include "reg_controler.h"
#include "channel_controler.h"
#include "../interface/i_slnic.h"

#include <pthread.h>

#define SL_MAX_RECV_CHANN		(8)
#define SL_MAX_SESSION_CHL		(8)
#define SL_MAX_SESSION_NUM		(16)

#define SL_PORCH_SW_VERSION		"s_1.0"

class open_ioctl_close
{
public:
	open_ioctl_close();
	~open_ioctl_close();
public:
	bool slnic_ioctl(int channel_id, st_session_group*  config, int dev);

public:
	bool get_slnic_fd(int dev);
	void close_slnic_fd();

public:
	int						 m_slnic_fd_1;
	int						 m_slnic_fd_2;
};

