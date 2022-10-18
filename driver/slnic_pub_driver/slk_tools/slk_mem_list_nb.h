/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		驱动中的无锁链表，支持应用层和驱动间的无锁交互，并且没有额外备份节点，性能优化，结构更清晰，为第三版

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_MEM_LIST_H_20150629
#define SLK_MEM_LIST_H_20150629

//#include <linux/types.h>
//#include <linux/kernel.h> /* printk() */
//#include <linux/pci.h>
#include "slk_define.h"
#include "slk_type.h"

#define PAGE_SIZE 4096
#define SLK_COMBIN_LIST_COUNT		(65536)	// 鏈夐棶棰樺緟瀹�

#define SLK_MEM_LIST_NODE_HEAD_LEN	(32)								/// 列表节点头的长度
#define SLK_MEM_LIST_SIZE			(8)									/// 列表大小
#define SLK_HW_DATA_MAX_LEN			(2048 - SLK_MEM_LIST_NODE_HEAD_LEN)	/// 硬件允许写入的最大长度
#define SLK_MEM_NODE_LEN			(2048)								/// 每块节点占用内存大小
#define SLK_NODE_INDEX				int									/// 节点index
#define SLK_NULL_NODE_INDEX			(-1)								/// 空节点
#define SLK_SHARE_MEM_LIST_COUNT	(2)									/// 共享队列头数量
#define SLK_RX_MEM_LIST_PAGE_COUNT	((((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)%PAGE_SIZE == 0) ? \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE) : \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE + 1))		

#define SLK_PAGE_MEM_LIST_COUNT		(PAGE_SIZE/SLK_MEM_LIST_SIZE)		/// 一页可以分配列表头的个数

#define SLK_MEM_MAPPING_COUNT		(192*1024)							/// 最大分配节点的个数

#define SLK_TX_MEM_LIST_COUNT		32
#define SLK_TX_TAG_COUNT			16
#define SLK_TX_LST_COUNT			(SLK_TX_TAG_COUNT+1)


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
	struct slk_mem_list_node*	m_p_mem_mapping[SLK_MEM_MAPPING_COUNT];	/// 预分配的数据块
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
};

/// 有锁链表
struct slk_mem_lck_list
{
	struct slk_mem_list_head	m_lst_head;							/// 队列头节点
	SLK_SPINLOCK_T				m_lst_lock;							/// 队列自旋锁
	SLK_NODE_INDEX				m_idx_tail;							/// 队列尾指针
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
	SLK_ATOMIC_T					m_i_dma_count;
	uint32_t					counter;								/// 当前包总数量
	uint32_t					m_pkt_loss;								/// 当前映射给硬件dma的数量
};

struct slk_rx_mem_list
{
	struct slk_mem_list*		m_lst_dma;								/// dma队列
	struct slk_mem_list*		m_lst_free_share;						/// 共享空闲队列，由此队列对硬件提供数据
	struct slk_mem_combin_list	m_combin_list[SLK_COMBIN_LIST_COUNT];	/// 与应用层通讯的数据队列组合

	uint8_t*					m_p_page[SLK_RX_MEM_LIST_PAGE_COUNT];	/// 分配给队列头的页
	struct slk_mem_mapping*		m_p_mapping;

	uint32_t					counter;								/// 当前包总数量
	uint32_t					m_pkt_loss;								/// 当前映射给硬件dma的数量
};

#pragma pack(pop)

#define		NODE_ONLY					((struct slk_mem_list_node*)(-1))
/// 判断队列是否为空
#define		LST_EMPTY(lst)				( lst->m_lst_head.m_idx_next == SLK_NULL_NODE_INDEX )
/// 判断队列是否只有一个元素
#define		LST_ONLY(lst)				((lst->m_lst_head.m_idx_next != SLK_NULL_NODE_INDEX) && (lst->m_idx_tail == lst->m_lst_head.m_idx_next))
/// 判断节点是否为空
#define		NODE_EMPTY(idx,mapping)		((idx > mapping->m_i_mem_count) || (idx == SLK_NULL_NODE_INDEX)) 
/// 获取节点的指针
#define		NODE_POINT(idx,mapping)		((struct slk_mem_list_node*)(*(mapping->m_p_mem_mapping + idx)))
/// 获取节点下一个的值
#define		NODE_NEXT_IDX(idx,mapping)	(((struct slk_mem_list_node*)(*(mapping->m_p_mem_mapping + idx)))->m_idx_next)


typedef int (*func_slk_push_data)(struct slk_rx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );
typedef int (*func_slk_proc_tx)(struct slk_tx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );
typedef int (*func_slk_proc_rx_dma2dma)(struct slk_rx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );

struct slk_mem_mapping* create_mem_mapping(int count);
void destroy_mem_mapping(struct slk_mem_mapping* p_mapping);

/// slk_list node
extern struct slk_mem_list_node* slk_create_mem_list_node(SLK_NODE_INDEX index, struct slk_mem_mapping* p_mapping);

/// dma映射
extern int  slk_dma_map_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);
/// dma解除映射
extern void slk_dma_unmap_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);
/// 刷到cpu，不解除映射，时间会小很多
extern void slk_ss2cpu_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);
/// 刷到dev，不解除映射，时间会小很多
extern void slk_ss2dev_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);

/// ****************************************slk_list*************************************************
/// 初slk_list
extern struct slk_mem_list* slk_create_mem_list(void* ptr,int len);
extern void slk_destroy_mem_list(struct slk_mem_list* p_list);
extern void slk_push_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_list_node* node,struct slk_mem_mapping* p_mapping);
extern struct slk_mem_list_node* slk_pop_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_mapping* p_mapping);
/// 在确定安全的情况下，可以把队列取空，注意:使用者必须确保在获取的过程中，不会有另外线程操作
extern struct slk_mem_list_node* slk_pop_mem_list_last_node(struct slk_mem_list* p_list,struct slk_mem_mapping* p_mapping);

///*****************************************slk_lck_list********************************************
extern struct slk_mem_lck_list* slk_create_mem_lck_list(void* ptr,int len);
extern void slk_destroy_mem_lck_list(struct slk_mem_lck_list* p_list);
extern void slk_push_mem_lck_list_node(struct slk_mem_lck_list* p_list,struct slk_mem_list_node* node,struct slk_mem_mapping* p_mapping);
extern struct slk_mem_list_node* slk_pop_mem_lck_list_node(struct slk_mem_lck_list* p_list,struct slk_mem_mapping* p_mapping);


///*****************************************slk_tx_list*********************************************
/// tx mem list
extern struct slk_tx_mem_list* create_tx_mem_list(struct slk_mem_mapping* p_mapping);
extern void destroy_tx_mem_list( struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci );

/// 这个函数只放入队列不发送给硬件
extern int  slk_tx_push_dma_list_only(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci ,struct slk_mem_list_node* p_node);
/// 下面的函数是不可重入
extern int  slk_tx_free_to_dma(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci ,unsigned long bars, struct slk_mem_list_node* p_node,int dw);
extern int	slk_tx_free_to_dma_buffer(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci,unsigned long bars,const char* ch_buff,int len,int dw);
extern int	slk_tx_dma_to_free(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci,func_slk_proc_tx func );

///*****************************************slk_rx_list*********************************************
/// rx mem list 
extern struct slk_rx_mem_list* create_rx_mem_list(struct slk_mem_mapping* p_mapping);
extern void destroy_rx_mem_list( struct slk_rx_mem_list* mem_list,SLK_DEVICE p_pci );

extern int slk_rx_free_to_dma(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars );
extern int slk_rx_free_to_dma_mapping(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars );
extern int slk_rx_dma_to_data(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci, func_slk_push_data func,SLK_TIMEVAL tv );
extern int slk_rx_dma_to_dma(struct slk_rx_mem_list* p_list,unsigned long bars );
extern int slk_rx_dma_to_dma_sync(struct slk_rx_mem_list* p_list,unsigned long bars, func_slk_proc_rx_dma2dma func );
extern int slk_rx_data_to_free(struct slk_rx_mem_list* p_list);

extern int slk_rx_free_to_dma_sso(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars);
extern int slk_rx_dma_to_data_sso(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci, func_slk_push_data func, int max_len);

#endif