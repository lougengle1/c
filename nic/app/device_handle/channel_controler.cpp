#include "channel_controler.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <pthread.h>


#define LEN_OFF					(14)
#define SEQ_OFF					(2)

channel_controler::channel_controler()
{
	m_driver_fd = -1;

	push_node_now = 0;
	push_nodes = new unsigned int[MAX_SAVE_PUSH];
	m_running = false;
	m_rx_seq_addr = NULL;

	memset(m_events, 0, sizeof(m_events));
	memset(m_channel_addr_infos, 0, sizeof(m_channel_addr_infos));
	memset(m_channel_addrs, 0, sizeof(m_channel_addrs));
	memset(&seq_addr, 0, sizeof(slk_addr_t));
	m_version = 0;
}

channel_controler::~channel_controler()
{
	stop();

	long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);

	if (m_driver_fd >= 0)
		close(m_driver_fd);
	m_driver_fd = -1;

	if (m_rx_seq_addr && m_rx_seq_addr != (void *)-1)
	{
		munmap(m_rx_seq_addr, PAGE_SIZE);
		m_rx_seq_addr = NULL;
	}

	if (m_channel_seq != (void *)-1)
	{
		munmap(m_channel_seq, seq_addr.m_lenth);
		m_channel_seq = NULL;
	}


	for (int i = 0; i < MAX_CHANNEL; ++i)
	{
		if (m_channel_addrs[i] && m_channel_addrs[i] != (void *)-1)
			munmap(m_channel_addrs[i], m_channel_addr_infos[i].m_lenth);

		m_channel_addrs[i] = NULL;
	}

	FILE * f = fopen("pushnode.txt", "w");
	for (int i = 0; i < push_node_now; ++i)
	{
		fprintf(f, "%d\n", push_nodes[i]);
	}
	fclose(f);
}

bool channel_controler::init(const char * file_name, class reg_controler *reg_ctl)
{
	int err;

	if (!reg_ctl)
		return false;

	m_driver_fd = open(file_name, O_RDWR);
	if (m_driver_fd < 0)
		return false;

	m_reg_controler = reg_ctl;

	err = ioctl(m_driver_fd, SLK_IOCTL_VERSION, &m_version);
	if (err < 0)
	{
		close(m_driver_fd);
		m_driver_fd = -1;
		printf("ioctl error !\n");
		return false;
	}

	err = ioctl(m_driver_fd, SLK_IOCTL_GET_RXSEQ_ADDR, &seq_addr);
	if (err < 0)
	{
		close(m_driver_fd);
		m_driver_fd = -1;
		printf("ioctl error !\n");
		return false;
	}

	m_channel_seq = (struct channel_read_info_block *)mmap(NULL,
					seq_addr.m_lenth,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					m_driver_fd,
					seq_addr.m_offset);
	if (m_channel_seq == (void *)-1)
	{
		perror("mmap SLK_IOCTL_GET_RX_ADDR");
		close(m_driver_fd);
		m_driver_fd = -1;

		return false;
	}

	return true;
}

bool channel_controler::exit()
{
	stop();

	long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);

	if (m_driver_fd >= 0)
		close(m_driver_fd);
	m_driver_fd = -1;

	if (m_rx_seq_addr && m_rx_seq_addr != (void *)-1)
	{
		munmap(m_rx_seq_addr, PAGE_SIZE);
		m_rx_seq_addr = NULL;
	}

	if (m_channel_seq != (void *)-1)
	{
		munmap(m_channel_seq, seq_addr.m_lenth);
		m_channel_seq = NULL;
	}

	for (int i = 0; i < MAX_CHANNEL; ++i)
	{
		if (m_channel_addrs[i] && m_channel_addrs[i] != (void *)-1)
			munmap(m_channel_addrs[i], m_channel_addr_infos[i].m_lenth);

		m_channel_addrs[i] = NULL;
	}

	return true;
}
int channel_controler::get_version()
{
	return m_version;
}

bool channel_controler::regist_channel(int channel, channel_reader_event *ev)
{
	int err = 1;
	m_channel_addr_infos[channel].m_channel = channel;
	err = ioctl(m_driver_fd, SLK_IOCTL_GET_RX_ADDR, &m_channel_addr_infos[channel]);
	if (err == 0)
	{
		//printf("ioctl is success\n");
	}
	else if(err < 0)
	{
		printf("ioctl is error\n");
	}

	void *channal_buff = mmap(NULL, 
								m_channel_addr_infos[channel].m_lenth, 
								PROT_READ | PROT_WRITE, MAP_SHARED,
								m_driver_fd,
								m_channel_addr_infos[channel].m_offset);

	//printf("dma addr 0x%lx\n", m_channel_addr_infos[channel].m_dma_addr);

	if (channal_buff == (void *)-1)
	{
		perror("mmap SLK_IOCTL_GET_RX_ADDR");
		return false;
	}

	m_channel_addrs[channel] = channal_buff;
	m_events[channel] = ev;

	return true;
}

void * channel_controler::recv_thread(void *param)
{
	struct channel_reader_param *chn_param = (struct channel_reader_param *)param;
	const int channel = chn_param->m_channel;

	class channel_controler * handle = chn_param->m_handle;
	class channel_reader_event *ev = handle->m_events[channel];

	uint16_t &seq = handle->m_channel_seq[channel].m_info.m_read_seq;
	char *buff_base = (char *)handle->m_channel_addrs[channel];
	char *buff_addr = (char *)handle->m_channel_addrs[channel];

	const unsigned int node_size = handle->m_channel_addr_infos[channel].m_node_size;
	const unsigned int node_num  = handle->m_channel_addr_infos[channel].m_node_num;
	const unsigned int node_mask = node_num - 1;
	unsigned int push_seq = seq;

	//int bind_cpu = ev->bind_cpu(channel);
	//if (bind_cpu >= 0)
	//{
	//	cpu_set_t cpu_info;
	//	CPU_ZERO(&cpu_info);
	//	CPU_SET(bind_cpu, &cpu_info);
	//	if (0 != pthread_setaffinity_np(pthread_self(), sizeof(cpu_info), &cpu_info))
	//	{
	//		perror("pthread_attr_setaffinity_np bind cpu error\n");
	//	}
	//}

	buff_addr = buff_base + (seq & node_mask) * node_size;
	while (handle->m_running)
	{
		volatile uint16_t msg_seq = *(volatile uint16_t *)(buff_addr + SEQ_OFF);
		if ((uint16_t)(seq + 1) == msg_seq)
		{
			//printf("match channel %d[soft_counter:%d] [hard_counter %d]\n", channel, seq+1, *(volatile uint16_t *)(buff_addr + SEQ_OFF));
			ev->on_recv_msg(channel, buff_addr, ntohs(*(unsigned short *)(buff_addr + LEN_OFF))+32, chn_param->m_board);
			//printf("recv_msg complete\n");
			seq = (seq + 1);
			buff_addr = buff_base + (seq & node_mask) * node_size;

			if (((seq - push_seq) & node_mask) > (node_num >> 3))
			{
				handle->push_rx_queue_node(channel, (seq - 1) & node_mask) ;
				push_seq = seq;
			}
		}

		// 如果当前没有消息上来，且队列使用超过了1/4, 就回收一下
		else if (seq != push_seq && ((seq - push_seq) & node_mask) > (node_num >> 2))
		{
			//printf(" not channel %d[soft_counter:%d] [hard_counter %d]\n", channel, seq + 1, *(volatile uint16_t *)(buff_addr + SEQ_OFF));
			handle->push_rx_queue_node(channel, (seq - 1) & node_mask) ;
			push_seq = seq;
		}
	}
	//printf("match channel %d[soft_counter:%d] [hard_counter %d]\n", channel, seq+1, *(volatile uint16_t *)(buff_addr + SEQ_OFF));
	return NULL;
}

bool channel_controler::start(int dev)
{
	m_running = true;

	mlockall(MCL_CURRENT);
	for (int channel = 2; channel < MAX_CHANNEL; ++channel)
	{
		if (!m_events[channel])
			continue;

		m_thread_param[channel].m_channel = channel;
		m_thread_param[channel].m_handle = this;
		m_thread_param[channel].m_board = dev;

		pthread_create(&m_recv_threads[channel], NULL, channel_controler::recv_thread, &m_thread_param[channel]);

		int bind_cpu = m_events[channel]->bind_cpu(channel);
		//printf("bind_cpu is %d\n", bind_cpu);
		if (bind_cpu >= 0)
		{
			cpu_set_t cpu_info;
			CPU_ZERO(&cpu_info);
			CPU_SET(bind_cpu, &cpu_info);
			if (0 != pthread_setaffinity_np(m_recv_threads[channel], sizeof(cpu_info), &cpu_info))
			{
				printf("pthread_attr_setaffinity_np bind cpu error [channel:%d] [cpu:%d]\n", channel,bind_cpu);
				return false;
			}
		}
	}

	return true;
}

bool channel_controler::stop()
{
	if (!m_running)
		return true;

	m_running = false;

	for (int channel = 2; channel < MAX_CHANNEL; ++channel)
	{
		if (!m_events[channel])
			continue;
		void *ret_ptr;
		pthread_join(m_recv_threads[channel], &ret_ptr);
	}

	return true;
}

bool channel_controler::write_data(const char * buf, int count)
{
	int len = 0;
	write(m_driver_fd, buf, count);
	if (len < 0)
	{
		printf("write_data is error\n");
	}
	return true;
}

int channel_controler::channel_write(void * buff, int len)
{
#if 0
	char write_buff[2048];
	struct stream_head_s2u *head = (struct stream_head_s2u *)write_buff;
	memset(head, 0, sizeof(*head));

	head->m_mode = 1;
	head->m_mkd_id = 1;
	memcpy(head + 1, buff, len);

	write(m_driver_fd, write_buff, len + sizeof(*head));

#endif
	return len;
}

bool channel_controler::push_rx_queue_node(int channel, int node)
{
	const long addr_regs[8] = {
		SW_PKT_CUR_MEM_OFF0,
		SW_PKT_CUR_MEM_OFF1,
		SW_PKT_CUR_MEM_OFF2,
		SW_PKT_CUR_MEM_OFF3,
		SW_PKT_CUR_MEM_OFF4,
		SW_PKT_CUR_MEM_OFF5,
		SW_PKT_CUR_MEM_OFF6,
		SW_PKT_CUR_MEM_OFF7,
	};

	m_reg_controler->write_reg(addr_regs[channel],  node);

	return true;
}
