/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		�����е���������֧��Ӧ�ò�������������������Ϊ�ڶ���

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_MEM_LIST_H_20150629
#define SLK_MEM_LIST_H_20150629

// #include <linux/types.h>
// #include <linux/kernel.h> /* printk() */
// #include <linux/pci.h>
#include "slk_type.h"

#define PAGE_SIZE 4096

#define SLK_MEM_LIST_NODE_HEAD_LEN	(32)								/// �б�ڵ�ͷ�ĳ���
#define SLK_MEM_LIST_SIZE			(16)								/// �б�Ĵ�С
#define SLK_HW_DATA_MAX_LEN			(2048 - SLK_MEM_LIST_NODE_HEAD_LEN)	/// Ӳ������д�����󳤶�
#define SLK_MEM_NODE_LEN			(2048)								/// ÿ��ڵ�ռ���ڴ��С
#define SLK_NODE_INDEX				int									/// �ڵ�index
#define SLK_NULL_NODE_INDEX			(-1)								/// �սڵ�
#define SLK_HEAD_NODE_INDEX			(-2)								/// ͷ�ڵ�
#define SLK_SHARE_MEM_LIST_COUNT	(4)									/// �������ͷ����
#define SLK_COMBIN_LIST_COUNT		(65536)								/// ����б����������Ҫ������Ӧ�ò㽻���õ����ֵ
#define SLK_RX_MEM_LIST_PAGE_COUNT	((((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)%PAGE_SIZE == 0) ? \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE) : \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE + 1))		

#define SLK_PAGE_MEM_LIST_COUNT		(PAGE_SIZE/SLK_MEM_LIST_SIZE)		/// һҳ���Է����б�ͷ�ĸ���

#define SLK_MEM_MAPPING_COUNT		(192*1024)							/// ������ڵ�ĸ���

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
	struct slk_mem_list_node*	m_p_mem_mapping[SLK_MEM_MAPPING_COUNT];		/// Ԥ��������ݿ�
	uint32_t					m_i_mem_count;							/// ��ǰ��Ч�����ݿ����
};

struct slk_mem_list_node
{
	SLK_NODE_INDEX				m_idx_next;							/// ָ����һ���ڵ�
	int							m_i_backup_flag;					/// �Ƿ�ת��Ϊbackup�ڵ�
	uint32_t					m_i_idx;							/// ��ǰ���ݿ��idx

	SLK_PTR_DAM_ADDR_T			m_p_dma_addr;						/// dma��ַ
	uint32_t					m_i_datalen;						/// ����Ӳ�����ݳ���
	uint16_t					m_p_private;						/// �û��Զ�������
	uint16_t					m_p_data_pos;						/// ָ����Ч���ݵ���ʼλ��
	uint8_t						m_p_resv[4];						/// ����

	uint8_t						m_p_data[SLK_HW_DATA_MAX_LEN];		/// Ӳ����������ݳ���

};
/// �б�ͷ
struct slk_mem_list_head
{
	SLK_NODE_INDEX				m_idx_next;							/// ָ����һ���ڵ�
};

/// �б�
struct slk_mem_list
{
	struct slk_mem_list_head	m_lst_head;							/// ����ͷ�ڵ�
	SLK_NODE_INDEX				m_idx_tail;							/// ����βָ��
	SLK_NODE_INDEX				m_idx_read;							/// ��ǰ��ȡ�Ľڵ�
	SLK_NODE_INDEX				m_i_backup;							/// ���ݽڵ�����
};

/// ����б�������֤һ����Ӧ�ò㽻������
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
	uint32_t					counter;								/// ��ǰ��������

	uint32_t					m_pkt_loss;								/// ��ǰӳ���Ӳ��dma������

	SLK_SPINLOCK_T				m_lock;								/// ����������

};

struct slk_rx_mem_list
{
	struct slk_mem_list*		m_lst_dma;								/// dma����
	struct slk_mem_list*		m_lst_free_share;						/// ������ж��У��ɴ˶��ж�Ӳ���ṩ����
	struct slk_mem_list*		m_lst_resv1;							/// ��������1
	struct slk_mem_list*		m_lst_resv2;							/// ��������2
	struct slk_mem_combin_list	m_combin_list[SLK_COMBIN_LIST_COUNT];	/// ��Ӧ�ò�ͨѶ�����ݶ������

	uint8_t*					m_p_page[SLK_RX_MEM_LIST_PAGE_COUNT];	/// ���������ͷ��ҳ
	//struct slk_mem_list_node*	m_p_mem_mapping[SLK_MEM_MAPPING_COUNT];	/// Ԥ��������ݿ�
	//uint32_t					m_i_mem_count;							/// ��ǰ��Ч�����ݿ����

	struct slk_mem_mapping*		m_p_mapping;

	uint32_t					counter;								/// ��ǰ��������
	uint32_t					m_pkt_loss;								/// ��ǰӳ���Ӳ��dma������
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
/// ��slk_list
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
