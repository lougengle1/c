/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		文件实现定长环的功能，最老版本的交互方案

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
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
	struct slk_list_node*	m_p_next;							/// 指向下一个节点
	struct slk_pkt*			m_p_pkt;							/// pkt包
	int						m_i_backup;							/// 是否转换为backup节点
};

struct slk_list
{
	struct slk_list_node	m_lst_head;							/// 队列头节点
	struct slk_list_node*	m_p_tail;							/// 队列尾指针
	struct slk_list_node*	m_p_read;							/// 当前读取的节点
	struct slk_list_node*	m_p_backup;							/// 备份节点数组
	void*					m_p_attach;							/// 需要关联的数据结构指针

	SLK_SPINLOCK_T				m_lock;								/// 队列自旋锁
};


/// slk_list node
extern struct slk_list_node* slk_create_list_node(void);
extern void slk_destroy_list_node(struct slk_list_node* node);
/// 初slk_list
extern struct slk_list* slk_create_list(void);
extern void slk_destroy_list(struct slk_list* p_list);
extern void slk_push_list_node(struct slk_list* p_list,struct slk_list_node* node);
extern struct slk_list_node* slk_get_next_node(struct slk_list* p_list);
extern struct slk_list_node* slk_pop_list_node(struct slk_list* p_list);
extern struct slk_list_node* slk_pop_free_list_node(struct slk_list* p_list);


///********************************************************************************************
/// rx方向队列
#define SLK_SESSION_LIST_COUNT		128

struct slk_rx_list
{
	struct slk_list*		m_free_list;								/// 空闲队列
	struct slk_list*		m_dma_list;									/// dma队列
	struct slk_list*		m_arp_list;									/// arp响应队列
	struct slk_list*		m_tcp_session_list[SLK_SESSION_LIST_COUNT];	/// tcp应用层处理队列
	struct slk_list*		m_icmp_session_list[SLK_SESSION_LIST_COUNT];/// icmp应用层处理队列
	uint32_t				m_i_counter;								/// 当前包总数量

};
/// rx队列
/// 创建rx队列
extern struct slk_rx_list* slk_create_rx_list(int n_free_lst);
/// 销毁rx队列
extern void slk_destroy_rx_list(struct slk_rx_list* rx_list);

/// 创建一个tcp session
extern int slk_rx_create_session(struct slk_rx_list* p_rx,void* p_attach,int session_id);
/// 销毁一个tcp session
extern void slk_rx_destroy_session(struct slk_rx_list* p_rx,int session_id);
/// 创建一个icmp session
extern int slk_rx_create_icmp_session(struct slk_rx_list* p_rx,void* p_attach);
/// 销毁一个icmp session
extern void slk_rx_destroy_icmp_session(struct slk_rx_list* p_rx,int session_id);

/// 从空闲队列中取出一个node
extern struct slk_list_node* slk_rx_pop_free_list(struct slk_rx_list* p_rx);
/// 将一个node放入空闲队列
extern void slk_rx_push_free_list(struct slk_rx_list* p_rx,struct slk_list_node* p_node,int use_lock);

/// 获取rx的下一个节点
extern struct slk_list_node* slk_rx_get_next_dma_list_node(struct slk_rx_list* p_rx);
/// 从dma队列中取出一个node
extern struct slk_list_node* slk_rx_pop_dma_list(struct slk_rx_list* p_rx);
/// 将一个node放入dma队列
extern void slk_rx_push_dma_list(struct slk_rx_list* p_rx,struct slk_list_node* p_node);

/// 从tcp session队列中取出一个node
extern struct slk_list_node* slk_rx_pop_session_list(struct slk_rx_list* p_rx, int session_id);
/// 将一个node放入tcp的某个session队列
extern void slk_rx_push_session_list(struct slk_rx_list* p_rx, int session_id ,struct slk_list_node* p_node);

/// 从icmp session队列中取出一个node
extern struct slk_list_node* slk_rx_pop_icmp_session_list(struct slk_rx_list* p_rx, int session_id);
/// 将一个node放入icmp session队列中
extern void slk_rx_push_icmp_session_list(struct slk_rx_list* p_rx, int session_id ,struct slk_list_node* p_node);

///********************************************************************************************
/// tx方向队列
struct slk_tx_list
{
	struct slk_list*		m_free_list;								/// 空闲队列
	struct slk_list*		m_dma_list;									/// dma队列
	struct slk_list*		m_tcp_session_list[SLK_SESSION_LIST_COUNT];	/// 重发/错误维护队列
	uint32_t				m_i_counter;								/// 当前包总数量
	SLK_ATOMIC_T			m_i_tx_hw_count;							/// 当前硬件包的个数
};

/// tx队列
extern struct slk_tx_list* slk_create_tx_list(int n_free_lst);
extern void slk_destroy_tx_list(struct slk_tx_list* tx_list);

/// 创建一个tcp session
extern int slk_tx_create_session(struct slk_tx_list* p_tx,void* p_attach, int session_id);
/// 销毁一个tcp session
extern void slk_tx_destroy_session(struct slk_tx_list* p_tx,int session_id);

extern struct slk_list_node* slk_tx_pop_free_list( struct slk_tx_list* p_tx );
extern void slk_tx_push_free_list( struct slk_tx_list* p_tx, struct slk_list_node* p_node,int use_lock);

extern struct slk_list_node* slk_tx_get_next_dma_list_node(struct slk_tx_list* p_tx);
extern struct slk_list_node* slk_tx_pop_dma_list( struct slk_tx_list* p_tx);
extern void slk_tx_push_dma_list( struct slk_tx_list* p_list, struct slk_list_node* p_node,int use_lock);

extern struct slk_list_node* slk_tx_pop_session_list(struct slk_tx_list* p_tx, int session_id);
extern void slk_tx_push_session_list(struct slk_tx_list* p_tx, int session_id ,struct slk_list_node* p_node);


#endif