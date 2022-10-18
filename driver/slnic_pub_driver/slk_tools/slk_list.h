/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		�ļ�ʵ�ֶ������Ĺ��ܣ����ϰ汾�Ľ�������

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_LIST_H_20140529
#define SLK_LIST_H_20140529

#include "slk_type.h"

//#include <linux/semaphore.h>
//#include <linux/types.h>
//#include <linux/kernel.h> /* printk() */
//#include <asm/atomic.h>
//#include <linux/pci.h>
//#include <linux/spinlock.h>


#define SLK_LIST_BACKUP_COUNT	2
#define SLK_PKT_MAX_LEN			2048
#define SLK_TX_DMA_MAX_COUNT	16

struct slk_pkt
{
	uint8_t					m_p_data[SLK_PKT_MAX_LEN];			/// 
	unsigned int			m_i_datalen; 
	SLK_PTR_DAM_ADDR_T		m_p_dma_addr;
	uint32_t				m_p_private;
};

struct slk_list_node
{
	struct slk_list_node*	m_p_next;							/// ָ����һ���ڵ�
	struct slk_pkt*			m_p_pkt;							/// pkt��
	int						m_i_backup;							/// �Ƿ�ת��Ϊbackup�ڵ�
};

struct slk_list
{
	struct slk_list_node	m_lst_head;							/// ����ͷ�ڵ�
	struct slk_list_node*	m_p_tail;							/// ����βָ��
	struct slk_list_node*	m_p_read;							/// ��ǰ��ȡ�Ľڵ�
	struct slk_list_node*	m_p_backup;							/// ���ݽڵ�����
	void*					m_p_attach;							/// ��Ҫ���������ݽṹָ��

	SLK_SPINLOCK_T				m_lock;								/// ����������
};


/// slk_list node
extern struct slk_list_node* slk_create_list_node(void);
extern void slk_destroy_list_node(struct slk_list_node* node);
/// ��slk_list
extern struct slk_list* slk_create_list(void);
extern void slk_destroy_list(struct slk_list* p_list);
extern void slk_push_list_node(struct slk_list* p_list,struct slk_list_node* node);
extern struct slk_list_node* slk_get_next_node(struct slk_list* p_list);
extern struct slk_list_node* slk_pop_list_node(struct slk_list* p_list);
extern struct slk_list_node* slk_pop_free_list_node(struct slk_list* p_list);


///********************************************************************************************
/// rx�������
#define SLK_SESSION_LIST_COUNT		128

struct slk_rx_list
{
	struct slk_list*		m_free_list;								/// ���ж���
	struct slk_list*		m_dma_list;									/// dma����
	struct slk_list*		m_arp_list;									/// arp��Ӧ����
	struct slk_list*		m_tcp_session_list[SLK_SESSION_LIST_COUNT];	/// tcpӦ�ò㴦�����
	struct slk_list*		m_icmp_session_list[SLK_SESSION_LIST_COUNT];/// icmpӦ�ò㴦�����
	uint32_t				m_i_counter;								/// ��ǰ��������

};
/// rx����
/// ����rx����
extern struct slk_rx_list* slk_create_rx_list(int n_free_lst);
/// ����rx����
extern void slk_destroy_rx_list(struct slk_rx_list* rx_list);

/// ����һ��tcp session
extern int slk_rx_create_session(struct slk_rx_list* p_rx,void* p_attach,int session_id);
/// ����һ��tcp session
extern void slk_rx_destroy_session(struct slk_rx_list* p_rx,int session_id);
/// ����һ��icmp session
extern int slk_rx_create_icmp_session(struct slk_rx_list* p_rx,void* p_attach);
/// ����һ��icmp session
extern void slk_rx_destroy_icmp_session(struct slk_rx_list* p_rx,int session_id);

/// �ӿ��ж�����ȡ��һ��node
extern struct slk_list_node* slk_rx_pop_free_list(struct slk_rx_list* p_rx);
/// ��һ��node������ж���
extern void slk_rx_push_free_list(struct slk_rx_list* p_rx,struct slk_list_node* p_node,int use_lock);

/// ��ȡrx����һ���ڵ�
extern struct slk_list_node* slk_rx_get_next_dma_list_node(struct slk_rx_list* p_rx);
/// ��dma������ȡ��һ��node
extern struct slk_list_node* slk_rx_pop_dma_list(struct slk_rx_list* p_rx);
/// ��һ��node����dma����
extern void slk_rx_push_dma_list(struct slk_rx_list* p_rx,struct slk_list_node* p_node);

/// ��tcp session������ȡ��һ��node
extern struct slk_list_node* slk_rx_pop_session_list(struct slk_rx_list* p_rx, int session_id);
/// ��һ��node����tcp��ĳ��session����
extern void slk_rx_push_session_list(struct slk_rx_list* p_rx, int session_id ,struct slk_list_node* p_node);

/// ��icmp session������ȡ��һ��node
extern struct slk_list_node* slk_rx_pop_icmp_session_list(struct slk_rx_list* p_rx, int session_id);
/// ��һ��node����icmp session������
extern void slk_rx_push_icmp_session_list(struct slk_rx_list* p_rx, int session_id ,struct slk_list_node* p_node);

///********************************************************************************************
/// tx�������
struct slk_tx_list
{
	struct slk_list*		m_free_list;								/// ���ж���
	struct slk_list*		m_dma_list;									/// dma����
	struct slk_list*		m_tcp_session_list[SLK_SESSION_LIST_COUNT];	/// �ط�/����ά������
	uint32_t				m_i_counter;								/// ��ǰ��������
	SLK_ATOMIC_T			m_i_tx_hw_count;							/// ��ǰӲ�����ĸ���
};

/// tx����
extern struct slk_tx_list* slk_create_tx_list(int n_free_lst);
extern void slk_destroy_tx_list(struct slk_tx_list* tx_list);

/// ����һ��tcp session
extern int slk_tx_create_session(struct slk_tx_list* p_tx,void* p_attach, int session_id);
/// ����һ��tcp session
extern void slk_tx_destroy_session(struct slk_tx_list* p_tx,int session_id);

extern struct slk_list_node* slk_tx_pop_free_list( struct slk_tx_list* p_tx );
extern void slk_tx_push_free_list( struct slk_tx_list* p_tx, struct slk_list_node* p_node,int use_lock);

extern struct slk_list_node* slk_tx_get_next_dma_list_node(struct slk_tx_list* p_tx);
extern struct slk_list_node* slk_tx_pop_dma_list( struct slk_tx_list* p_tx);
extern void slk_tx_push_dma_list( struct slk_tx_list* p_list, struct slk_list_node* p_node,int use_lock);

extern struct slk_list_node* slk_tx_pop_session_list(struct slk_tx_list* p_tx, int session_id);
extern void slk_tx_push_session_list(struct slk_tx_list* p_tx, int session_id ,struct slk_list_node* p_node);


#endif