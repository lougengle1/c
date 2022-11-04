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

using std::string;
using std::vector;

class test_ev : public slnic_event
{ 
public:
	virtual int on_data_recv(int channel,const char *data, int data_len)
	{
		printf("channel:%d data_len:%d\n", channel , data_len);
		return 0;
		//收到的数据包括硬件头和网络头
		/*int ptr_count;
		for (ptr_count = 0; ptr_count < data_len; ptr_count++)
		{ 
			  printf(" 0x%x", data[ptr_count]); 
			  if ((ptr_count + 1) % 8 == 0) 
					printf("\n"); 
		}*/
	}

	test_ev(int bind_cpu)
	{
		m_running = false;
		m_quit_flag = false;
	}

	~test_ev()
	{
		if (m_running)
		{
			m_running = false;
		}
	}

private:
	volatile bool		m_running;

public:
	bool				m_quit_flag;
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

	st_session_group m_session_cfg1;
	memset(&m_session_cfg1, 0, sizeof(m_session_cfg1));

	//获取网卡相关信息
	string eth_name = "slnic0";
	slnic_net_info net_info;

	//获取丢包统计
	mac_packet_statistics p_mac_packet;

	//获取硬件版本信息
	slnic_hw_version_info hw_info;
	memset(&hw_info, 0, sizeof(slnic_hw_version_info));
	

	//假设这次要配置通道2 session0的过滤条件，想要配置其他的session，以此类推
	int i_chan_id = 2;
	int i_session_num = 1;

	//通道2session0的配置信息
	string str_ses0_src_mac = "";
	string str_ses0_dst_mac = "";
	string str_ses0_src_port = "";
	string str_ses0_dst_port = "5521";
	string str_ses0_src_ip = "";
	string str_ses0_dst_ip = "233.54.127.1";
	string str_ses0_net_type = "0";
	string str_ses0_net_mac = "0";

	m_session_cfg.m_channel = i_chan_id;														//配置通道												
	m_session_cfg.m_session_num = 1;															//配置session总数	
	m_session_cfg.m_cpu =1;																		//配置cpu号

	//chan2
	sprintf(m_session_cfg.m_session_config[0].m_smac, str_ses0_src_mac.c_str());				//配置源mac
	sprintf(m_session_cfg.m_session_config[0].m_dmac, str_ses0_dst_mac.c_str());				//配置目标mac
	m_session_cfg.m_session_config[0].m_sport = atoi(str_ses0_src_port.c_str());				//配置源端口
	m_session_cfg.m_session_config[0].m_dport = atoi(str_ses0_dst_port.c_str());				//配置目标端口
	sprintf(m_session_cfg.m_session_config[0].m_sip, str_ses0_src_ip.c_str());					//配置源ip
	sprintf(m_session_cfg.m_session_config[0].m_dip, str_ses0_dst_ip.c_str());					//配置目标ip
	m_session_cfg.m_session_config[0].m_net_type = atoi(str_ses0_net_type.c_str());				//配置网络数据类型
	m_session_cfg.m_session_config[0].m_mac = atoi(str_ses0_net_mac.c_str());					//配置mac口，1是mac0 ，2 是mac1,0 是不用mac口


	//配置结束

	i_slnic *inf = i_slnic::create_handle();
	if (!inf->slnic_init())
	{
		printf("slnic_init error!\n");
		return 1;
	}

	
	if (!inf->get_hw_version(hw_info))
	{
		printf("slnic_init error!\n");
		return 1;
	}
	printf("production:%d Edition%x\n", hw_info.production, ntohs(hw_info.edition));

	int sys_uinfo = inf->get_sys_unifo();
	if (sys_uinfo == SL_NIC_NF5_2MAC10G)
	{
		printf("sl_nic_NF5 2_10gMAC\n");
	}

	int driver_vsersion = inf->get_driver_version();
	printf("driver_vsersion:%d\n", driver_vsersion);

	if (!inf->slnic_get_net_info(eth_name.c_str(), &net_info))
	{
		printf("slnic_get_info error!\n");
	}

	if (!inf->slnic_fdir_config(&m_session_cfg, &ev))
	{
		printf("slnic_fdir_config error!\n");
		return 1;
	}

	if (!inf->slnic_get_packet_statistics(&p_mac_packet))
	{
		printf("slnic_fdir_config error!\n");
		return 1;
	}

	if (!inf->slnic_start())
	{
		printf("slnic_start error!\n");
		return 1;
	}

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
