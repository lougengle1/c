/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		�����е���������֧��Ӧ�ò�����������������������û�ж��ⱸ�ݽڵ㣬�����Ż����ṹ��������Ϊ������

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_MEM_LIST_H_20150629
#define SLK_MEM_LIST_H_20150629

//#include <linux/types.h>
//#include <linux/kernel.h> /* printk() */
//#include <linux/pci.h>
#include "slk_define.h"
#include "slk_type.h"

#define PAGE_SIZE 4096
#define SLK_COMBIN_LIST_COUNT		(65536)	// 有问题待定

#define SLK_MEM_LIST_NODE_HEAD_LEN	(32)								/// �б�ڵ�ͷ�ĳ���
#define SLK_MEM_LIST_SIZE			(8)									/// �б��С
#define SLK_HW_DATA_MAX_LEN			(2048 - SLK_MEM_LIST_NODE_HEAD_LEN)	/// Ӳ������д�����󳤶�
#define SLK_MEM_NODE_LEN			(2048)								/// ÿ��ڵ�ռ���ڴ��С
#define SLK_NODE_INDEX				int									/// �ڵ�index
#define SLK_NULL_NODE_INDEX			(-1)								/// �սڵ�
#define SLK_SHARE_MEM_LIST_COUNT	(2)									/// �������ͷ����
#define SLK_RX_MEM_LIST_PAGE_COUNT	((((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)%PAGE_SIZE == 0) ? \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE) : \
										(((SLK_COMBIN_LIST_COUNT*2+SLK_SHARE_MEM_LIST_COUNT)*SLK_MEM_LIST_SIZE)/PAGE_SIZE + 1))		

#define SLK_PAGE_MEM_LIST_COUNT		(PAGE_SIZE/SLK_MEM_LIST_SIZE)		/// һҳ���Է����б�ͷ�ĸ���

#define SLK_MEM_MAPPING_COUNT		(192*1024)							/// ������ڵ�ĸ���

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
	struct slk_mem_list_node*	m_p_mem_mapping[SLK_MEM_MAPPING_COUNT];	/// Ԥ��������ݿ�
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
};

/// ��������
struct slk_mem_lck_list
{
	struct slk_mem_list_head	m_lst_head;							/// ����ͷ�ڵ�
	SLK_SPINLOCK_T				m_lst_lock;							/// ����������
	SLK_NODE_INDEX				m_idx_tail;							/// ����βָ��
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
	SLK_ATOMIC_T					m_i_dma_count;
	uint32_t					counter;								/// ��ǰ��������
	uint32_t					m_pkt_loss;								/// ��ǰӳ���Ӳ��dma������
};

struct slk_rx_mem_list
{
	struct slk_mem_list*		m_lst_dma;								/// dma����
	struct slk_mem_list*		m_lst_free_share;						/// ������ж��У��ɴ˶��ж�Ӳ���ṩ����
	struct slk_mem_combin_list	m_combin_list[SLK_COMBIN_LIST_COUNT];	/// ��Ӧ�ò�ͨѶ�����ݶ������

	uint8_t*					m_p_page[SLK_RX_MEM_LIST_PAGE_COUNT];	/// ���������ͷ��ҳ
	struct slk_mem_mapping*		m_p_mapping;

	uint32_t					counter;								/// ��ǰ��������
	uint32_t					m_pkt_loss;								/// ��ǰӳ���Ӳ��dma������
};

#pragma pack(pop)

#define		NODE_ONLY					((struct slk_mem_list_node*)(-1))
/// �ж϶����Ƿ�Ϊ��
#define		LST_EMPTY(lst)				( lst->m_lst_head.m_idx_next == SLK_NULL_NODE_INDEX )
/// �ж϶����Ƿ�ֻ��һ��Ԫ��
#define		LST_ONLY(lst)				((lst->m_lst_head.m_idx_next != SLK_NULL_NODE_INDEX) && (lst->m_idx_tail == lst->m_lst_head.m_idx_next))
/// �жϽڵ��Ƿ�Ϊ��
#define		NODE_EMPTY(idx,mapping)		((idx > mapping->m_i_mem_count) || (idx == SLK_NULL_NODE_INDEX)) 
/// ��ȡ�ڵ��ָ��
#define		NODE_POINT(idx,mapping)		((struct slk_mem_list_node*)(*(mapping->m_p_mem_mapping + idx)))
/// ��ȡ�ڵ���һ����ֵ
#define		NODE_NEXT_IDX(idx,mapping)	(((struct slk_mem_list_node*)(*(mapping->m_p_mem_mapping + idx)))->m_idx_next)


typedef int (*func_slk_push_data)(struct slk_rx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );
typedef int (*func_slk_proc_tx)(struct slk_tx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );
typedef int (*func_slk_proc_rx_dma2dma)(struct slk_rx_mem_list* p_list,int id_node,struct slk_mem_list_node* p_node );

struct slk_mem_mapping* create_mem_mapping(int count);
void destroy_mem_mapping(struct slk_mem_mapping* p_mapping);

/// slk_list node
extern struct slk_mem_list_node* slk_create_mem_list_node(SLK_NODE_INDEX index, struct slk_mem_mapping* p_mapping);

/// dmaӳ��
extern int  slk_dma_map_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);
/// dma���ӳ��
extern void slk_dma_unmap_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);
/// ˢ��cpu�������ӳ�䣬ʱ���С�ܶ�
extern void slk_ss2cpu_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);
/// ˢ��dev�������ӳ�䣬ʱ���С�ܶ�
extern void slk_ss2dev_mem_list_node(struct slk_mem_list_node* node,SLK_DEVICE p_pci,long dir);

/// ****************************************slk_list*************************************************
/// ��slk_list
extern struct slk_mem_list* slk_create_mem_list(void* ptr,int len);
extern void slk_destroy_mem_list(struct slk_mem_list* p_list);
extern void slk_push_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_list_node* node,struct slk_mem_mapping* p_mapping);
extern struct slk_mem_list_node* slk_pop_mem_list_node(struct slk_mem_list* p_list,struct slk_mem_mapping* p_mapping);
/// ��ȷ����ȫ������£����԰Ѷ���ȡ�գ�ע��:ʹ���߱���ȷ���ڻ�ȡ�Ĺ����У������������̲߳���
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

/// �������ֻ������в����͸�Ӳ��
extern int  slk_tx_push_dma_list_only(struct slk_tx_mem_list* mem_list,SLK_DEVICE p_pci ,struct slk_mem_list_node* p_node);
/// ����ĺ����ǲ�������
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