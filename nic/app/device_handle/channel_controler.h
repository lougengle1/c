#pragma once
#include <pthread.h>
#include "device_define.h"
#include <time.h>

#include "reg_controler.h"

#define MAX_SAVE_PUSH	(5000000)

#define SYS_CACHE_LINE_SIZE			64
struct channel_read_info
{
	unsigned short			m_read_seq;
};

struct channel_read_info_block
{
	union
	{
		struct channel_read_info m_info;
		char 					 m_payload[SYS_CACHE_LINE_SIZE];
	};
};

class channel_reader_event
{
public:
	virtual void on_recv_msg(int channel, const char *data, int len, int dev) = 0;
	virtual int bind_cpu(int channel) = 0;
};

class channel_controler;
struct channel_reader_param
{
	int								m_channel;
	class channel_controler *		m_handle;
	int								m_board;			//´ú±íÄÄÕÅ°å¿¨
};

class channel_controler
{
public:
	channel_controler();
	~channel_controler();
	
	bool init(const char *file_name, class reg_controler *reg_ctl);
	bool exit();
	int get_version();
public:
	bool regist_channel(int channel, channel_reader_event *);
	int channel_write(void *buff, int len);

public:
	bool start(int dev);
	bool stop();
	bool write_data(const char* buf,int count);

private:
	static void *recv_thread(void *);
	bool push_rx_queue_node(int channel, int node);

	void								*m_rx_seq_addr;
	volatile bool						 m_running;
	channel_reader_event				*m_events[MAX_CHANNEL];
	struct slk_addr_t					 m_channel_addr_infos[MAX_CHANNEL];
	void								*m_channel_addrs[MAX_CHANNEL];
	int									 m_driver_fd;
	pthread_t							 m_recv_threads[MAX_CHANNEL];
	struct channel_reader_param			 m_thread_param[MAX_CHANNEL];
	struct channel_read_info_block		*m_channel_seq;

	class reg_controler					*m_reg_controler;

	int									 push_node_now;
	unsigned int 						*push_nodes;
	struct slk_addr_t					seq_addr;
	int									m_version;
};
