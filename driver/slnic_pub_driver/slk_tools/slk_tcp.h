/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		把硬件板卡包装成网络设备，绕过tcp协议栈

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_TCP_H_20120602
#define SLK_TCP_H_20120602

#include "slk_type.h"

struct psd_head
{
	u32		sourceip;    //源IP地址
	u32		destip;      //目的IP地址
	u8		mbz;         //置空(0)
	u8		ptcl;        //协议类型
	u16		plen;        //TCP/UDP数据包的长度(即从TCP/UDP报头算起到数据包结束的长度 单位:字节)
};
/// 值用作临时传递参数用
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

/// @brief 这个函数负责将数据、seq等值组合成标准的sk_buff结构
/// @param param		传入参数（需要注意的是：seq是由外界累加好的，内部不再处理）
/// @return				成功：skt， 失败 NULL
extern SLK_SK_BUFF sl_tcp_build_send_skb(struct sl_tcp_params* param);

/// @brief 答复客户端的连接请求。这个函数会根据用户的请求连接包，转换成seq_ack包（ack+1需要在调用这个函数后从sk_buff取）
/// @param seq_skb		客户端传递过来的seq包
/// @param new_seq		自定义的seq
/// @param ack			处理完毕后，产生的ack,seq不变
/// @return				成功：skt， 失败 NULL
extern SLK_SK_BUFF sl_answer_synack_skb(SLK_SK_BUFF seq_skb, uint32_t new_seq, uint32_t* ack);

/// @brief 接收到client的ack包，获得当前的seq、ack的值
/// @param ack_skb		客户端发送过来的ack包
/// @param ack			服务器需要更新的ack值
/// @return			
extern void sl_rcv_ack(SLK_SK_BUFF ack_skb, uint32_t* seq,uint32_t* ack);

/// @brief 接收到client的数据包，获得当前的seq、ack的值
/// @param ack_skb		客户端发送过来的数据包
/// @param ack			服务器需要更新的ack值
/// @return		
extern void sl_rcv_data(SLK_SK_BUFF data_skb,uint32_t* seq, uint32_t* ack);

/// @brief 向服务器发送ack包
/// @param param		ack包的参数
/// @return		成功：skt， 失败 NULL
extern SLK_SK_BUFF sl_send_ack_skb(struct sl_tcp_params* param);

extern SLK_SK_BUFF sl_send_ack_skb_base(uint32_t sip,uint32_t dip,struct sl_tcp_params* param);

int		is_32_over(uint32_t u_base, uint32_t u_add);

#endif