#pragma once
#include "device_define.h"
#define	SYS_ID						0x0010 
#define MAC0_RX_PKT_NUM_H			0x4058
#define MAC0_RX_PKT_NUM_L			0x4060
#define MAC0_RX_PKT_DESERT_NUM_H	0x4068
#define MAC0_RX_PKT_DESERT_NUM_L    0x4070
#define MAC0_TX_PKT_NUM_H			0x4078
#define MAC0_TX_PKT_NUM_L			0x4080

#define MAC1_RX_PKT_NUM_H			0x4158 
#define MAC1_RX_PKT_NUM_L			0x4160
#define MAC1_RX_PKT_DESERT_NUM_H	0x4168
#define MAC1_RX_PKT_DESERT_NUM_L	0x4170
#define MAC1_TX_PKT_NUM_H			0x4178
#define MAC1_TX_PKT_NUM_L			0x4180

class reg_controler
{
public:
	reg_controler();
	~reg_controler();

	bool init(const char *file_name);
	bool just_init(const char *file_name);
	void* get_bar0_address();
	bool exit();

public:
	bool read_reg(long addr, unsigned int &val);
	bool write_reg(long addr, long val);

private:
	int					m_device_fd;
	void			*	m_bar0_addr;
};
