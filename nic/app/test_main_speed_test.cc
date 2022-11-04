#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include "interface/i_slnic.h"

#define MAX_RAW_PACK_BUFF			(1000000)
using std::string;
using std::vector;

struct raw_head
{
	unsigned int	seq;
	unsigned long long err_seq;
	unsigned int	len;
	unsigned long	time;
};


struct raw_pack
{
	struct raw_head m_head;
};

class test_ev : public slnic_event
{ 
public:
	virtual int on_data_recv(int channel,const char *data, int data_len)
	{
		struct timespec time_now;
		clock_gettime(CLOCK_REALTIME, &time_now);

		if (m_pack_last < MAX_RAW_PACK_BUFF)
		{
			m_raw_pack[m_pack_last].m_head.seq = *(int *)(data + 42);
			m_raw_pack[m_pack_last].m_head.len = data_len - 42;
			m_raw_pack[m_pack_last].m_head.time = time_now.tv_sec;
			m_raw_pack[m_pack_last].m_head.time *= 1000000000;
			m_raw_pack[m_pack_last].m_head.time += time_now.tv_nsec;
			m_pack_last++;
		}
		return 0;
	}

	test_ev(int bind_cpu)
	{
		m_running = false;
		m_quit_flag = false;
		m_pack_last = 0;
		m_raw_pack = new raw_pack[MAX_RAW_PACK_BUFF];
	}

	~test_ev()
	{
		FILE *fout = fopen("recv_pack_porch.log", "w");
		for (size_t i = 0; i < MAX_RAW_PACK_BUFF; ++i)
		{
			fprintf(fout, "%ld,%ld,%ld\n",
				m_raw_pack[i].m_head.seq,
				m_raw_pack[i].m_head.len,
				m_raw_pack[i].m_head.time);
		}
		fclose(fout);

	}

private:
	bool				m_running;
	int					m_pack_last;
public:
	bool				m_quit_flag;
	raw_pack			*m_raw_pack;
};


void help_info()
{
	printf("\n\n-----------------------------------------------------------\n\n");
	printf("help:              <help>.\n");
	printf("quit:              <q or quit>quit this test.\n");
}

void command(vector<string>& vec_cmd, i_slnic *inf, test_ev *ev)
{
	st_session_group  session_group;
	int channel;

	if (vec_cmd.empty())
	{
		return;
	}

	const string CMD_HELP_INFO = "help";
	const string CMD_QUIT_APP = "quit";
	const string CMD_Q_APP = "q";

	string CMD = vec_cmd[0];

	if (CMD_QUIT_APP == CMD || CMD_Q_APP == CMD)
	{
		ev->m_quit_flag = true;
	}
	else if (CMD_HELP_INFO == CMD)
	{
		help_info();
	}
	else
		printf("error cmd!\n");
}

void ssplit(const string & ori, vector<string>& tokens, const string & delimiters)
{
	string::size_type lastPos = 0;
	string::size_type pos = ori.find_first_of(delimiters, lastPos);
	while (string::npos != lastPos)
	{
		tokens.push_back(ori.substr(lastPos, pos - lastPos));
		lastPos = pos + delimiters.length();
		if (string::npos == pos)
			break;
		pos = ori.find_first_of(delimiters, lastPos + 1);
	}
}

void input_command(i_slnic *inf, test_ev *ev)
{
	char cmd[1024];

	printf("[cmd]:");
	fflush(stdout);
	fgets(cmd, sizeof(cmd), stdin);
	int len = strlen(cmd);
	if (len <= 0)
	{
		return;
	}
	cmd[len - 1] = 0;
	if (strlen(cmd) <= 0)
	{
		return;
	}

	string str = cmd;
	vector<string> vec_str;
	ssplit(str.c_str(), vec_str, " ");

	command(vec_str,inf,ev);
}



int test_nic()
{
	bool b_ret;
	test_ev ev(-1);
	st_session_group m_session_cfg;
	memset(&m_session_cfg, 0, sizeof(m_session_cfg));

	//假设这次要配置通道2session0的过滤条件
	int i_chan_id = 2;
	int i_session_id = 0;
	string str_src_mac = "";
	string str_dst_mac = "";
	string str_src_port = "10001";
	string str_dst_port = "";
	string str_src_ip = "3.3.3.3";
	string str_dst_ip = "";

	m_session_cfg.m_channel = i_chan_id;												//配置通道												
	m_session_cfg.m_session_num = 1;													//配置session号	
	m_session_cfg.m_cpu = 1;															//配置cpu号

	sprintf(m_session_cfg.m_session_config[0].m_smac, str_src_mac.c_str());				//配置源mac
	sprintf(m_session_cfg.m_session_config[0].m_dmac, str_dst_mac.c_str());				//配置目标mac
	m_session_cfg.m_session_config[0].m_sport = atoi(str_src_port.c_str());				//配置源端口
	m_session_cfg.m_session_config[0].m_dport = atoi(str_dst_port.c_str());				//配置目标端口
	sprintf(m_session_cfg.m_session_config[0].m_sip, str_src_ip.c_str());				//配置源ip
	sprintf(m_session_cfg.m_session_config[0].m_dip, str_dst_ip.c_str());				//配置目标ip

	//配置结束

	i_slnic *inf = i_slnic::create_handle();

	if (!inf->slnic_init())
	{
		printf("slnic_init error!\n");
		return 1;
	}

	if (!inf->slnic_fdir_config(&m_session_cfg, &ev))
	{
		printf("slnic_fdir_config error!\n");
		return 1;
	}

	inf->slnic_start();

	while (!ev.m_quit_flag)
	{
		input_command(inf, &ev);
	}

	inf->slnic_stop();
	inf->slnic_exit();

    return 0;
}


int main(int argc, char *argv[])
{
	return test_nic();
}
