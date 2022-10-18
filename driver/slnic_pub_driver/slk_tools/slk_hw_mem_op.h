/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		文件实现高效的直接操作硬件内存的功能，包括读写，这些方法的使用需要对bar0的映射做特殊处理

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_HD_MEM_OP_H_20170301
#define SLK_HD_MEM_OP_H_20170301


#include "slk_struct.h"


struct slk_tx_mem_op_list
{
	int							m_i_begin;			/// 节点起始位置
	int							m_i_ready;			/// 节点结束位置
	unsigned long				m_p_bar1_base;		/// bar1起始位置
	unsigned long				m_p_bar0_base;		/// bar0起始位置
	unsigned long				m_p_beg_addr;		/// bar1写内存起始位置
	volatile uint32_t			counter;			/// tx本地计数器（格式兼容老版本）
	volatile uint32_t			snd_counter;		/// 记录发送出去的包数 add by liminghuan 2018-07-20

	struct slk_coher_map_dma	m_tx_dma_count;		/// 硬件上报计数
};

/// 创建tx mem op队列
/// addr: 硬件内存地址
/// bar1_base: 硬件
/// tx_count: tx节点数量
/// bar0_base: bar0的地址，填0
extern struct slk_tx_mem_op_list* slk_create_tx_mem_op_list(unsigned long addr,unsigned long bar1_base,int tx_count,unsigned long bar0_base,SLK_PTR_PCI_DEV p_dev);

extern void slk_destroy_tx_mem_op_list(struct slk_tx_mem_op_list* p_lst,SLK_PTR_PCI_DEV p_dev);

extern int slk_tx_mem_op_list_write(const char* msg, int len,struct slk_tx_mem_op_list* p_lst);

extern int slk_tx_mem_op_list_write_safe(const char* msg, int len,struct slk_tx_mem_op_list* p_lst);

extern int slk_tx_mem_op_list_write_debug(const char* msg, int len,struct slk_tx_mem_op_list* p_lst, char* mem, int mem_len);

extern int slk_tx_mem_op_list_ready(struct slk_tx_mem_op_list* p_lst);

extern int slk_tx_mem_op_check_recover(struct slk_tx_mem_op_list* p_lst);

#endif