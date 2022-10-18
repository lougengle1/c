/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		��Ӳ���忨��װ�������豸���ƹ�tcpЭ��ջ

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_TCP_H_20120602
#define SLK_TCP_H_20120602

#include "slk_type.h"

struct psd_head
{
	u32		sourceip;    //ԴIP��ַ
	u32		destip;      //Ŀ��IP��ַ
	u8		mbz;         //�ÿ�(0)
	u8		ptcl;        //Э������
	u16		plen;        //TCP/UDP���ݰ��ĳ���(����TCP/UDP��ͷ�������ݰ������ĳ��� ��λ:�ֽ�)
};
/// ֵ������ʱ���ݲ�����
struct sl_tcp_params
{
	uint8_t*		sip;
	uint8_t*		dip;
	uint16_t		sport;
	uint16_t		dport;
	uint8_t*		data;
	uint32_t		len;
	uint32_t		seq;
	uint32_t		ack;
	struct SLK_NET_DEVICE	*dev;
};

extern u16 checksum(u16 *buffer, int size);

/// @brief ��������������ݡ�seq��ֵ��ϳɱ�׼��sk_buff�ṹ
/// @param param		�����������Ҫע����ǣ�seq��������ۼӺõģ��ڲ����ٴ���
/// @return				�ɹ���skt�� ʧ�� NULL
extern SLK_SK_BUFF sl_tcp_build_send_skb(struct sl_tcp_params* param);

/// @brief �𸴿ͻ��˵����������������������û����������Ӱ���ת����seq_ack����ack+1��Ҫ�ڵ�������������sk_buffȡ��
/// @param seq_skb		�ͻ��˴��ݹ�����seq��
/// @param new_seq		�Զ����seq
/// @param ack			������Ϻ󣬲�����ack,seq����
/// @return				�ɹ���skt�� ʧ�� NULL
extern SLK_SK_BUFF sl_answer_synack_skb(SLK_SK_BUFF seq_skb, uint32_t new_seq, uint32_t* ack);

/// @brief ���յ�client��ack������õ�ǰ��seq��ack��ֵ
/// @param ack_skb		�ͻ��˷��͹�����ack��
/// @param ack			��������Ҫ���µ�ackֵ
/// @return			
extern void sl_rcv_ack(SLK_SK_BUFF ack_skb, uint32_t* seq,uint32_t* ack);

/// @brief ���յ�client�����ݰ�����õ�ǰ��seq��ack��ֵ
/// @param ack_skb		�ͻ��˷��͹��������ݰ�
/// @param ack			��������Ҫ���µ�ackֵ
/// @return		
extern void sl_rcv_data(SLK_SK_BUFF data_skb,uint32_t* seq, uint32_t* ack);

/// @brief �����������ack��
/// @param param		ack���Ĳ���
/// @return		�ɹ���skt�� ʧ�� NULL
extern SLK_SK_BUFF sl_send_ack_skb(struct sl_tcp_params* param);

extern SLK_SK_BUFF sl_send_ack_skb_base(uint32_t sip,uint32_t dip,struct sl_tcp_params* param);

int		is_32_over(uint32_t u_base, uint32_t u_add);

#endif