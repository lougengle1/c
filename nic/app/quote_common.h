#pragma once

#include <stdint.h>

#pragma pack(push, 1)
/// add by zhou.hu review 2016/9/7 MACͷ
struct mac_head
{
public:
	uint8_t		m_dst_addr[6];
	uint8_t		m_src_addr[6];
	uint16_t	m_type;
public:
	void swap();
};

/// add by zhou.hu review 2016/9/7 IPͷ
union ip_version
{
	uint8_t		m_ver_and_len;
	struct
	{
		uint8_t	m_head_len : 4;
		uint8_t	m_version : 4;
	};
};

union ip_offset
{
	uint16_t		m_slice_and_offset;
	struct
	{
		uint16_t	m_slice : 3;
		uint16_t	m_offset : 13;
	};
};

union ip_addr
{
	uint32_t		m_ip;
	struct
	{
		uint8_t		m_b[4];
	};
};

struct ip_head
{
	ip_version				m_version;				///<�汾���ײ�����
	uint8_t					m_tos;					///<8λ��������
	uint16_t				m_total_len;			///<16λ�ܳ���
	uint16_t				m_packet_id;			///<16λ��ʶ
	ip_offset				m_offset;				///<3λ��־��Ϣ����13λƫ��
	uint8_t					m_ttl;					///<8λ����ʱ��(TTL)
	uint8_t					m_protocol;				///<8λЭ��
	uint16_t				m_check_sum;			///<16λ�ײ�У���
	ip_addr					m_src_ip;				///<32λԴ��ַ
	ip_addr					m_dst_ip;				///<32λĿ���ַ

public:
	void swap();
};


class udp_head
{
public:
	uint16_t		m_src_port;			///< Դ�˿�
	uint16_t		m_dst_port;			///< Ŀ��˿�
	uint16_t		m_udp_len;			///< UDP����
	uint16_t		m_crc;				///< �����
public:
	void swap();
};

struct tcp_head
{
	unsigned short	m_src_port;					//Դ�˿� 16bit
	unsigned short	m_dst_port;					//Ŀ�Ķ˿� 16bit
	unsigned int	m_ui_seq_num;				//���к� 32bit
	unsigned int	m_ui_ack_num;				//ȷ�Ϻ� 32bit
	unsigned short	m_head_len_and_flag;		//ǰ4λ tcpͷ����, ��6λ������ ��6λ��־λ
	unsigned short	m_window_size;				//���ڴ�С16bit
	unsigned short	m_check_sum;				//У��� 16bit
	unsigned short	m_surgent_pointer;			//��������ƫ�� 16bit
};

#pragma pack(pop)
