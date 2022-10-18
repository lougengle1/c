#ifndef SLK_MMAP_RING_H_20171013
#define SLK_MMAP_RING_H_20171013

// #include <linux/types.h>  /* size_t */
// #include <linux/kernel.h> /* printk() */
#include "slk_define.h"
#include "slk_type.h"

struct slk_mmap_ring
{
	uint32_t*		m_i_read_begin;			/// 读的起始位置
	uint32_t*		m_i_write_begin;		/// 写的起始位置
	uint32_t*		m_i_max_node_len;		/// 节点最大长度
	uint32_t*		m_i_ring_count;			/// 环的数量
	uint32_t*		m_i_idx;				/// 当前ring的自描述

	uint8_t*		m_ch_ring_begin;		/// 环的起始位置

	uint8_t*		m_ch_mmap_buffer;		/// 映射区间的起始位置
	uint32_t		m_i_mmap_len;			/// 映射的长度

};

extern struct slk_mmap_ring* create_mmap_ring(uint32_t node_len,uint32_t node_count,uint32_t idx);
extern void destroy_mmap_ring(struct slk_mmap_ring* p_ring);

extern int  write_node_to_ring(struct slk_mmap_ring* p_ring,const uint8_t* msg,int len);
extern int  read_node_to_ring(struct slk_mmap_ring* p_ring,uint8_t* msg, int len);



#endif