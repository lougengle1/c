/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		驱动中的无锁链表，支持应用层和驱动间的无锁交互，为第二版

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_MEM_LIST_H_20150629
#define SLK_MEM_LIST_H_20150629

// #include <linux/types.h>
// #include <linux/kernel.h> /* printk() */
// #include <linux/pci.h>
#include "slk_type.h"

#define PAGE_SIZE 4096

#define SLK_MEM_LIST_NODE_HEAD_LEN	(32)								/// 列表节点头的长度
#define SLK_MEM_LIST_SIZE			(16)								/// 列表的大小
#define SLK_HW_DATA_MAX_LEN			(2048 - SLK_MEM_LIST_NODE_HEAD_LEN)	/// 硬件允许写入的最大长度
#define SLK_MEM_NODE_LEN			(2048)								/// 每块节点占用内存大小
#define SLK_NODE_INDEX				int									/// 节点index
#define SLK_NULL_NODE_INDEX			(-1)								/// 空节点
#define SLK_HEAD_NODE_INDEX			(-2)								/// 头节点
#define SLK_SHARE_MEM_LIST_COUNT	(4)									/// 共享队列头数量
#define SLK_COMBIN_LIST_COUNT		(65536)								/// 组合列表的数量，主要用来与应用层交互用的最大值
#define SLK_RX_MEM_LIST_PAGE_COUNT	((((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)%PAGE_SIZE == 0) ? \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE) : \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE + 1))		

#define SLK_PAGE_MEM_LIST_COUNT		(PAGE_SIZE/SLK_MEM_LIST_SIZE)		/// 一页可以分配列表头的个数

#define SLK_MEM_MAPPING_COUNT		(192*1024)							/// 最大分配节点的个数

#define SLK_TX_MEM_LIST_COUNT		32
#define SLK_TX_DAM_COUNT			16


#pragma pack(push, 1)

struct slk_hw_head
{
	uint64_t		m_user_data;
	uint32_t		m_debug_seq;
	uint16_t		m_reserved1;
	uint16_t		m_msg_body_len;
	uint8_t			m_subcmd_type;
	uint8_t			m_reserved2[5];
	uint8_t			m_mkd_id;
	uint8_t			m_reserved3[9];
};

struct slk_mem_mapping
{
	struct slk_mem_list_node*	m_p_mem_mapping[SLK_MEM_MAPPING_COUNT];		/// 预分配的数据块
	uint32_t					m_i_mem_count;							/// 当前有效的数据块个数
};

struct slk_mem_list_node
{
	SLK_NODE_INDEX				m_idx_next;							/// 指向下一个节点
	int							m_i_backup_flag;					/// 是否转换为backup节点
	uint32_t					m_i_idx;							/// 当前数据块的idx

	SLK_PTR_DAM_ADDR_T			m_p_dma_addr;						/// dma地址
	uint32_t					m_i_datalen;						/// 接收硬件数据长度
	uint16_t					m_p_private;						/// 用户自定义数据
	uint16_t					m_p_data_pos;						/// 指向有效数据的起始位置
	uint8_t						m_p_resv[4];						/// 保留

	uint8_t						m_p_data[SLK_HW_DATA_MAX_LEN];		/// 硬件传输的数据长度

};
/// 列表头
struct slk_mem_list_head
{
	SLK_NODE_INDEX				m_idx_next;							/// 指向下一个节点
};

/// 列表
struct slk_mem_list
{
	struct slk_mem_list_head	m_lst_head;							/// 队列头节点
	SLK_NODE_INDEX				m_idx_tail;							/// 队列尾指针
	SLK_NODE_INDEX				m_idx_read;							/// 当前读取的节点
	SLK_NODE_INDEX				m_i_backup;							/// 备份节点数组
};

/// 组合列表，用来保证一组与应用层交换数据
struct slk_mem_combin_list
{
	struct slk_mem_list*		m_lst_free;
	struct slk_mem_list*		m_lst_data;
};

struct slk_tx_mem_list
{
	struct slk_mem_list			m_lst_tx_dma;
	struct slk_mem_list			m_lst_tx_free;

	struct slk_mem_mapping*		m_p_mapping;

	SLK_ATOMIC_T				m_i_dma_count;
	uint32_t					counter;								/// 当前包总数量

	uint32_t					m_pkt_loss;								/// 当前映射给硬件dma的数量

	SLK_SPINLOCK_T				m_lock;								/// 队列自旋锁

};

struct slk_rx_mem_list
{
	struct slk_mem_list*		m_lst_dma;								/// dma队列
	struct slk_mem_list*		m_lst_free_share;						/// 共享空闲队列，由此队列对硬件提供数据
	struct slk_mem_list*		m_lst_resv1;							/// 保留队列1
	struct slk_mem_list*		m_lst_resv2;							/// 保留队列2
	struct slk_mem_combin_list	m_combin_list[SLK_COMBIN_LIST_COUNT];	/// 与应用层通讯的数据队列组合

	uint8_t*					m_p_page[SLK_RX_MEM_LIST_PAGE_COUNT];	/// 分配给队列头的页
	//struct slk_mem_list_node*	m_p_mem_mapping[SLK_MEM_MAPPING_COUNT];	/// 预分配的数据块
	//uint32_t					m_i_mem_count;							/// 当前有效的数据块个数

	struct slk_mem_mapping*		m_p_mapping;

	uint32_t					counter;								/// 当前包总数量
	uint32_t					m_pkt_loss;								/// 当前映射给硬件dma的数量
};

#pragma pack(pop)

typedef int (*func_slk_push_data)(struct slk_rx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );
typedef int (*func_slk_proc_tx)(struct slk_tx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );
typedef int (*func_slk_proc_dma2dma)(struct slk_rx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node);

struct slk_mem_mapping* create_mem_mapping(int count);
void destroy_mem_mapping(struct slk_mem_mapping* p_mapping);

/// index to ptr
extern struct slk_mem_list_node* slk_index_to_mem_node(SLK_NODE_INDEX index, struct slk_mem_mapping* p_mapping );
/// slk_list node
extern struct slk_mem_list_node* slk_create_mem_list_node(SLK_NODE_INDEX index, struct slk_mem_mapping* p_mapping);
/// 初slk_list
extern struct slk_mem_list* slk_create_mem_list(void* ptr,int len,SLK_NODE_INDEX back_index);
extern void slk_destroy_mem_list(struct slk_mem_list* p_list);
extern void slk_push_list_mem_node(struct slk_mem_list* p_list,SLK_NODE_INDEX node,struct slk_mem_mapping* p_mapping);
extern SLK_NODE_INDEX slk_get_first_mem_node(struct slk_mem_list* p_list,struct slk_mem_mapping* p_mapping);
extern SLK_NODE_INDEX slk_pop_list_mem_node(struct slk_mem_list* p_list,struct slk_mem_mapping* p_mapping);
extern SLK_NODE_INDEX slk_pop_free_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_mapping* p_mapping);
extern SLK_NODE_INDEX slk_pop_no_back_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_mapping* p_mapping);
extern void slk_set_back_node(struct slk_mem_list* p_list,int back_node);
extern int  slk_mem_list_is_empty(struct slk_mem_list* p_list);

/// tx mem list
extern struct slk_tx_mem_list* create_tx_mem_list(struct slk_mem_mapping* p_mapping);
extern void destroy_tx_mem_list( struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci );

extern int  slk_tx_free_to_dma(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci ,unsigned long bars, struct slk_mem_list_node* p_node,int dw);
extern int	slk_tx_free_to_dma_buffer(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci,unsigned long bars,const char* ch_buff,int len,int dw);
extern int	slk_tx_dma_to_free(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci,func_slk_proc_tx func );

/// rx mem list 
extern int slk_rx_free_to_dma(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars );
extern int slk_rx_free_to_dma_mapping(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars );
extern int slk_rx_dma_to_data(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci, func_slk_push_data func,SLK_TIMEVAL tv);
extern int slk_rx_dma_to_dma(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars,func_slk_proc_dma2dma func );
extern int slk_rx_data_to_free(struct slk_rx_mem_list* p_list);

extern struct slk_rx_mem_list* create_rx_mem_list(struct slk_mem_mapping* p_mapping);
extern void destroy_rx_mem_list( struct slk_rx_mem_list* mem_list,SLK_DEVICE p_pci );

#endif
