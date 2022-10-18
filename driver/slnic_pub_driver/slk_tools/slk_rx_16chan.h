#ifndef SLK_RX_CHAN_H_20171228
#define SLK_RX_CHAN_H_20171228

#include "slk_struct.h"
#include "slk_define.h"
#include "slk_register_addr.h"
#include "slk_type.h"

#pragma pack(push, 1)

/// rx通道基本信息,会映射给应用层
struct slk_rx_chanbase
{
	uint32_t					m_i_rx_l_reg_addr;		/// rx队列节点内存地址寄存器
	uint32_t					m_i_rx_reg_idx;			/// rx下标内存地址寄存器
	uint32_t					m_i_rx_reg_recove;		/// rx回写下标寄存器
	uint32_t					m_i_que_count;			/// 队列数量
	uint32_t					m_i_que_node_size;		/// 队列节点大小
};

#pragma pack(pop)

/// 映射头，需要映射给应用程序
struct slk_rx_mmap_head
{
	struct slk_coher_map_dma	m_dma_data;				/// 基本数据区
	SLK_PTR_PCI_DEV				m_p_dev;				/// pcie设备指针

	struct slk_coher_map_dma	m_reserved;				/// 保留区
	struct slk_type_pu32*		m_rx_chan_vis_count;	/// 计数器，虚拟地址队列，指向基本数据区
	struct slk_type_dma_addr*	m_rx_chan_dma_count;	/// 计数器，dam地址队列，指向基本数据区
	struct slk_rx_chanbase*		m_rx_chan_base;			/// 通道基本数据，指向基本数据区
	int							m_i_channel_count;		/// 通道的个数

	SLK_SPINLOCK_T				m_rx_chan_lock;			///<驱动自旋锁
};

/// rx 通道
struct slk_rx_channel
{
	int							m_i_chan_id;			/// rx通道id
	struct slk_coher_map_dma	m_dma_queue;			/// 与硬件交互的rx队列内存区域
	SLK_PTR_DAM_ADDR_T			m_dma_counter;			/// rx队列计数器的dma地址
	void*						m_vis_counter;			/// rx队列计数器的虚拟地址
	struct slk_rx_chanbase*		m_p_chanbase;			/// rx通道的基本信息
	SLK_PTR_PCI_DEV				m_p_dev;				///	pcie设备
	unsigned long				m_bar0_addr;			/// bar0 addr
};


/// 创建rx mmap head
extern struct slk_rx_mmap_head* create_rx_mmap_head(int chan_count,SLK_PTR_PCI_DEV p_dev);
extern struct slk_rx_mmap_head* create_alone_rx_mmap_head(SLK_PTR_PCI_DEV p_dev);

/// 设置队列通道大小
extern void set_channel_queue(struct slk_rx_mmap_head* p_head,int channel_id,int que_count, int node_size);

/// 销毁rx mmap head
extern void destory_rx_mmap_head(struct slk_rx_mmap_head* p_head);

/// 创建rx通道
/// param chan_id:   通道id
/// param base：     通道的基本信息，这些信息需要映射给应用层
/// param dma_count: rx计数的dma地址
/// param vis_count: rx计数器的虚拟地址
/// param p_dev:     pci设备
/// param bar0:      bar0，用来写寄存器
extern struct slk_rx_channel* create_rx_channel( int chan_id,struct slk_rx_chanbase* base,SLK_PTR_DAM_ADDR_T dma_count,void* vis_count,SLK_PTR_PCI_DEV p_dev,unsigned long bar0,SLK_SPINLOCK_T lcl_lock,SLK_PTR_DAM_ADDR_T min_addr );
/// 销毁一个通道
extern void destory_rx_channel( struct slk_rx_channel* p_chan );

/// push一个新节点进去
extern int push_rx_queue_node( struct slk_rx_channel* p_chan,int node,SLK_SPINLOCK_T lcl_lock );

/// 通知硬件读取到下标
extern int rx_channel_read_index(struct slk_rx_channel* p_chan, int idx, SLK_SPINLOCK_T lcl_lock);

/// 获取节点指针
extern void* get_rx_queue_node( struct slk_rx_channel* p_chan,int node );

/// 获取下一个节点数据
extern int rx_node_move_next( struct slk_rx_channel* p_chan,int node );

#endif