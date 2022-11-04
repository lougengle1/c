#pragma once
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define  SL_NIC_NF5_2MAC10G			(0x01)
#define  SL_NIC_NF5_2MAC1G			(0x02)
#define  SL_NIC_NF10_2MAC10G		(0x03)
#define  SL_NIC_NF10_2MAC1G			(0x04)

enum e_net_type
{
	E_NIC_NET_TYPE_UDP = 0,
	E_NIC_NET_TYPE_TCP,
};

//session���˵�����
struct st_session_config
{
	char				m_smac[20];						//Դmac��ַ
	char				m_dmac[20];						//Ŀ��mac��ַ
	char				m_sip[20];						//Դip��ַ
	char				m_dip[20];						//Ŀ���ַ
	unsigned int		m_sport;						//Դ�˿�
	unsigned int		m_dport;						//Ŀ��˿�
	int					m_net_type;						//���紫������tcp/udp
	int					m_mac;							//mac�ڵ�ѡ�� 0-����mac���˲����� 1-����mac0 2-����mac1
};

//session������
struct st_session_group
{
	st_session_config	m_session_config[16];			//һ��16��session��Ŀǰ֧��4��
	unsigned short		m_channel;						//ͨ����[2,7]��ȡֵ��ΧΪ2-7��
	unsigned short		m_cpu;							//-1Ϊ�����,����Ϊ�󶨵�cpu��
	unsigned int		m_session_num;					//һ��ͨ������session������
};

#pragma pack(push, 1)
//Ӳ����������Э��
struct st_nic_data_head
{
	uint8_t				m_mac_id;						//��������
	uint8_t				m_channel_id;					//Ŀ��ͨ��id
	uint8_t				m_reserved_0[2];				//�ж��������ı�־λ
	uint32_t			m_bord_time_s;					//�����뼶ϵͳʱ��
	uint32_t			m_bord_time_ns;					//�������뼶ϵͳʱ��
	uint8_t				m_reserved_1;					//�����ֶ�
	uint8_t				m_ip_prot;						//ipЭ������
	uint16_t			m_hard_payload_len;				//payload����
	uint8_t				m_fragment_flag;				//�ְ�����
	uint16_t			m_fragment_offset;				//�ְ�ƫ��
	uint8_t				m_reserved_2;					//�����ֶ�
	uint32_t			m_reserved_3;					//�����ֶ�
	unsigned long		m_reserved_4;					//�����ֶ�
};

#pragma pack(pop)
struct slnic_net_info
{
	char					m_name[6];					//�����豸����	
	struct sockaddr_in		sin;						//�����豸ip
	unsigned char			m_mac[6];					//�����豸mac
	struct sockaddr_in		netmask;					//�����豸��������
	bool					m_running_flags;			//�����豸�Ƿ�����
};

struct mac_packet_statistics
{
	unsigned long mac0_rx_pkt_num;						//MAC0����İ�ͳ��
	unsigned long mac0_rx_pkt_desert_num;				//MAC0����Ķ���ͳ��
	unsigned long mac0_tx_pkt_num;						//MAC0����İ�ͳ��
	
	unsigned long mac1_rx_pkt_num;						//MAC1����İ�ͳ��
	unsigned long mac1_rx_pkt_desert_num;				//MAC1����Ķ���ͳ��
	unsigned long mac1_tx_pkt_num;						//MAC1����İ�ͳ��
};


struct slnic_hw_version_info
{
	unsigned short production;							//shengli_nic��Ŀ�Ĳ�Ʒ��Ϊ0x16
	unsigned short edition;								//Ӳ���汾��svn��
};



class slnic_event
{
public:
	virtual int on_data_recv(int channel,const char *data, int data_len) = 0;
};

class i_slnic
{
public:

	/*************************************************
	* Function:		create_handle 
	  Description:  ����i_slnic����
	  Input:		NULL
	  Return:       i_slnicָ��
	**************************************************/
	static i_slnic *create_handle();

	virtual ~i_slnic() {};

	/*************************************************
	* Function:		get_version
	  Description:	��ȡ����汾��
	  Input:		NULL
	  Return:       �汾��
	**************************************************/
	virtual const char *get_api_version() = 0; 

	/*************************************************
	* Function:		get_hw_version
	Description:	��ȡӲ���汾��
	Input:			dev1��һ�Ű忨�Ƿ���
					dev2�ڶ��Ű忨�Ƿ���
	Return:         ��ȡ�Ƿ�ɹ�
	**************************************************/
	virtual bool get_hw_version(slnic_hw_version_info &hw_info, int dev1, int dev2) = 0;

	/*************************************************
	* Function:		get_driver_version
	Description:	��ȡӲ���汾��
	Input:			dev1��һ�Ű忨�Ƿ���
					dev2�ڶ��Ű忨�Ƿ���
	Return:			��ȡ�Ƿ�ɹ�
	**************************************************/
	virtual bool get_driver_version(int dev1, int dev2) = 0;


	/*************************************************
	* Function:		get_sys_unifo
	Description:	��ȡ����nf5����nf10 mac10g����1g�汾
	Input:		NULL
	Return:       �汾��
	**************************************************/
	virtual int get_sys_unifo() = 0;

	/*************************************************
	* Function:		slnic_init
	  Description:	slnic����
	  Input:		dev1��һ�Ű忨�Ƿ���
					dev2�ڶ��Ű忨�Ƿ���
	  Return:       return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_init(int dev) = 0;


	/*************************************************
	* Function:		slnic_exit
	  Description:	slnic�˳�
	  Input:		NULL
	  Return:       return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_exit() = 0;
	

	/*************************************************
	* Function:		slnic_fdir_config
	  Description:	slnic��������	����ession��ͨ���ţ�ÿ��ͨ���Ű�һ��cpu��������
	  Input:		@config:		session����
					@slnic_event*:	ͨ���������ݵĺ���
					dev1��һ�Ű忨�Ƿ���
					dev2�ڶ��Ű忨�Ƿ���
	  Return:		return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_fdir_config(st_session_group *config, slnic_event* ev, int dev) = 0;
	

	/*************************************************
	* Function:		slnic_delete_config
	  Description:	ɾ��session����
	  Input:		@config:		session����
	  Return:		return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_delete_config(st_session_group *config) = 0;


	/*************************************************
	* Function:		slnic_get_net_info
	Description:	��ȡslnic0������Ϣ
	Input:		@eth_name:		�豸��������slnic0
				@slnic_net_info ��ȡ����Ϣ
	Return:		return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_get_net_info(const char* eth_name, slnic_net_info*) = 0;


	virtual bool slnic_get_packet_statistics(mac_packet_statistics* p_mac_packet_statistics, int dev) = 0;

	

	/*************************************************
	* Function:		slnic_send
	  Description:	slnicѡ���Ӧ��mac�ڷ�������
	  Input:		@mac_id:		mac�ڵ�ѡ��
					@data��			���͵�����
					@data_len		�������ݵĳ���
	  Return:       return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_send(uint8_t mac_id, const char* data, short data_len) = 0;


	/*************************************************
	* Function:		slnic_start
	  Description:	slnic�����߳�
	  Input:		dev��һ�Ű忨�Ƿ���
	
	  Return:       return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_start(int dev) = 0;


	/*************************************************
	* Function:		slnic_stop
	  Description:	slnic�ر��߳�
	  Input:		dev��һ�Ű忨�Ƿ���

	  Return:       return true����ɹ���return false����ʧ��
	**************************************************/
	virtual bool slnic_stop(int dev) = 0;
};