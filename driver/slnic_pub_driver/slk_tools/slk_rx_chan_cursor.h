#ifndef SLK_RX_CHAN_H_20171228
#define SLK_RX_CHAN_H_20171228

#include "slk_struct.h"
#include "slk_define.h"
#include "slk_register_addr.h"

/**
 * �˿�Ϊ8/16ͨ���α귽���İ汾�����������ڷ��α귽����
 */

#pragma pack(push, 1)

/// rxͨ��������Ϣ,��ӳ���Ӧ�ò�
struct slk_rx_chanbase
{
	uint32_t					m_i_rx_reg_queue_node_dma;		/// rx���нڵ��ַ�ļĴ���
	uint32_t					m_i_rx_reg_cursor_dma;	        /// rx�洢�α������ڴ��ַ�ļĴ���
    uint32_t					m_i_rx_reg_recove;		        /// rx��д�±�Ĵ���
    uint32_t					m_i_que_count;			        /// ��������
	uint32_t					m_i_que_node_size;		        /// ���нڵ��С
};

#pragma pack(pop)

/// ӳ��ͷ����Ҫӳ���Ӧ�ó���
struct slk_rx_mmap_head
{
	struct slk_coher_map_dma	m_dma_data;				/// ����������
	SLK_PTR_PCI_DEV				m_p_dev;				/// pcie�豸ָ��

	struct slk_coher_map_dma	m_reserved;				/// ������
	struct slk_type_pu32*		m_rx_chan_vis_cursor;	/// �α��ڴ棬�����ַ���У�ָ�����������
	struct slk_type_dma_addr*	m_rx_chan_dma_cursor;	/// �α��ڴ棬dam��ַ���У�ָ�����������
	struct slk_rx_chanbase*		m_rx_chan_base;			/// ͨ���������ݣ�ָ�����������
    int							m_i_channel_count;		/// ͨ���ĸ���

	SLK_SPINLOCK_T					m_rx_chan_lock;			///<����������
};

/// rx ͨ��
struct slk_rx_channel
{
	int							m_i_chan_id;			/// rxͨ��id
	struct slk_coher_map_dma	m_dma_queue;			/// ��Ӳ��������rx�����ڴ�����
	SLK_PTR_DAM_ADDR_T			m_dma_cursor;			/// rx�����α��ڴ��dma��ַ
	void*						m_vis_cursor;			/// rx�����α��ڴ�������ַ
	struct slk_rx_chanbase*		m_p_chanbase;			/// rxͨ���Ļ�����Ϣ
	SLK_PTR_PCI_DEV				m_p_dev;				///	pcie�豸
	unsigned long				m_bar0_addr;			/// bar0 addr
};


/// ����rx mmap head
extern struct slk_rx_mmap_head* create_rx_mmap_head(int chan_count,SLK_PTR_PCI_DEV p_dev);
extern struct slk_rx_mmap_head* create_alone_rx_mmap_head(SLK_PTR_PCI_DEV p_dev);

/// ���ö���ͨ����С
extern void set_channel_queue(
    struct slk_rx_mmap_head* p_head,
    int channel_id,
    int que_count, 
    int node_size
);

/// ����rx mmap head
extern void destory_rx_mmap_head(struct slk_rx_mmap_head* p_head);


/// ����rxͨ��
/// param chan_id:   ͨ��id
/// param base��     ͨ���Ļ�����Ϣ����Щ��Ϣ��Ҫӳ���Ӧ�ò�
/// param dma_count: rx������dma��ַ
/// param vis_count: rx�������������ַ
/// param p_dev:     pci�豸
/// param bar0:      bar0������д�Ĵ���
extern struct slk_rx_channel* create_rx_channel(int chan_id, struct slk_rx_chanbase* base, SLK_PTR_DAM_ADDR_T dma_cursor, void* vis_cursor,
    SLK_PTR_PCI_DEV p_dev, unsigned long bar0, SLK_SPINLOCK_T lcl_lock, SLK_PTR_DAM_ADDR_T min_addr);

/// ����rxͨ��
/// param chan_id:   ͨ��id
/// param base��     ͨ���Ļ�����Ϣ����Щ��Ϣ��Ҫӳ���Ӧ�ò�
/// param dma_count: rx������dma��ַ
/// param vis_count: rx�������������ַ
/// param p_dev:     pci�豸
/// param bar0:      bar0������д�Ĵ���
/// param min_addr:  ���õ���Сdma��ַ
/// param alloc_time:  ���������
/// param use_anywall: ���޷�������ʵ�ַʱ�Ƿ�����ʹ��
extern struct slk_rx_channel* create_rx_channel_limit_alloc(int chan_id, struct slk_rx_chanbase* base, SLK_PTR_DAM_ADDR_T dma_cursor, void* vis_cursor, 
    SLK_PTR_PCI_DEV p_dev, unsigned long bar0, SLK_SPINLOCK_T lcl_lock, SLK_PTR_DAM_ADDR_T min_addr, int alloc_time, int use_anywall);

/// ����һ��ͨ��
extern void destory_rx_channel( struct slk_rx_channel* p_chan );

/// pushһ���½ڵ��ȥ
extern int push_rx_queue_node( struct slk_rx_channel* p_chan,int node, SLK_SPINLOCK_T lcl_lock );

/// ֪ͨӲ����ȡ���±�
extern int rx_channel_recover_index(struct slk_rx_channel* p_chan, int idx, SLK_SPINLOCK_T lcl_lock);

/// ��ȡ�ڵ�ָ��
extern void* get_rx_queue_node( struct slk_rx_channel* p_chan,int node );

/// ��ȡ��һ���ڵ�����
extern int rx_node_move_next( struct slk_rx_channel* p_chan,int node );

#endif