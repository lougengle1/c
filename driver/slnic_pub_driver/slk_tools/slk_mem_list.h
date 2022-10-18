/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		驱动中的无锁链表，为第一版

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_MEM_LIST_H_20150629
#define SLK_MEM_LIST_H_20150629

#include "slk_type.h"

#define SLK_MEM_LIST_NODE_HEAD_LEN	32
#define SLK_HW_DATA_MAX_LEN			(2048 - SLK_MEM_LIST_NODE_HEAD_LEN)
#define SLK_MEM_NODE_LEN			(2048)
#define SLK_NODE_INDEX				int
#define SLK_NULL_NODE_INDEX			(-1)
#define SLK_HEAD_NODE_INDEX			(-2)
#define SLK_TOTAL_BACK_NODE_COUNT	(3)

#define SLK_MEM_MAPPING_COUNT	(128*1024)

#ifdef SLK_WAXBERRY
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
#else
/// 硬件项目标准头
struct slk_hw_head
{
	uint8_t		m_msg_type;			///< 命令类型
	uint8_t		m_reserved1[5];		///< 保留字段
	uint16_t	m_msg_body_len;		///< 消息包体长度
	uint32_t	m_second;			///< 时间戳(秒)
	uint32_t	m_nano_second;		///< 时间戳(纳秒)
	uint8_t		m_reserved2[5];		///< 保留字段
	uint8_t		m_session_id;		///< 会话编号
	uint8_t		m_reserved3[10];	///< 保留字段
};
#endif

struct slk_mem_list_node
{
	SLK_NODE_INDEX				m_idx_next;							/// 指向下一个节点
	int							m_i_backup_flag;							/// 是否转换为backup节点

	SLK_PTR_DAM_ADDR_T			m_p_dma_addr;
	uint32_t					m_i_datalen; 
	uint32_t					m_p_private;
	uint8_t						m_p_resv[8];

	uint8_t						m_p_data[SLK_HW_DATA_MAX_LEN];		/// 

};

struct slk_mem_list
{
	struct slk_mem_list_node	m_lst_head;							/// 队列头节点
	SLK_NODE_INDEX				m_idx_tail;							/// 队列尾指针
	SLK_NODE_INDEX				m_idx_read;							/// 当前读取的节点
	SLK_NODE_INDEX				m_i_backup;							/// 备份节点数组
	uint8_t						m_p_resv[4];
};

struct slk_rx_mem_list
{
	struct slk_mem_list*		m_lst_free;
	struct slk_mem_list*		m_lst_dma;
	struct slk_mem_list*		m_lst_data;

	uint8_t*					m_p_page;
	struct slk_mem_list_node*	m_p_mem_mapping[SLK_MEM_MAPPING_COUNT];
	uint32_t					m_i_mem_count;

	uint32_t					counter;			/// 当前包总数量
	uint32_t					m_pkt_loss;			/// 当前映射给硬件dma的数量
};

typedef int (*func_slk_push_data)(struct slk_rx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );
/// index to ptr
extern struct slk_mem_list_node* slk_index_to_mem_node(SLK_NODE_INDEX index, struct slk_mem_list_node** beg, int count );
/// slk_list node
extern struct slk_mem_list_node* slk_create_mem_list_node(SLK_NODE_INDEX index, struct slk_mem_list_node** beg, int count);
/// 初slk_list
extern struct slk_mem_list* slk_create_mem_list(void* ptr,int len,SLK_NODE_INDEX back_index);
extern void slk_destroy_mem_list(struct slk_mem_list* p_list);
extern void slk_push_list_mem_node(struct slk_mem_list* p_list,SLK_NODE_INDEX node,struct slk_mem_list_node** beg_node, uint32_t count);
extern SLK_NODE_INDEX slk_get_first_mem_node(struct slk_mem_list* p_list,struct slk_mem_list_node** beg_node, uint32_t count);
extern SLK_NODE_INDEX slk_pop_list_mem_node(struct slk_mem_list* p_list,struct slk_mem_list_node** beg_node, uint32_t count);
extern SLK_NODE_INDEX slk_pop_free_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_list_node** beg_node, uint32_t count);
extern SLK_NODE_INDEX slk_pop_no_back_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_list_node** beg_node, uint32_t count);
extern int  slk_mem_list_is_empty(struct slk_mem_list* p_list);


/// rx mem list 
extern int slk_rx_free_to_dma(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars );
extern int slk_rx_free_to_dma_mapping(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci,unsigned long bars );
extern int slk_rx_dma_to_data(struct slk_rx_mem_list* p_list,SLK_DEVICE p_pci, SLK_TIMEVAL tv, func_slk_push_data fun );
extern int slk_rx_dma_to_dma(struct slk_rx_mem_list* p_list,unsigned long bars );
extern int slk_rx_data_to_free(struct slk_rx_mem_list* p_list);

extern int add_one_page(struct slk_rx_mem_list* p_list);
extern struct slk_rx_mem_list* create_rx_mem_list(int count);
extern void destroy_rx_mem_list( struct slk_rx_mem_list* mem_list,SLK_DEVICE p_pci );

#endif