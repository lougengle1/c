/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		�ļ�ʵ�ָ�Ч��ֱ�Ӳ���Ӳ���ڴ�Ĺ��ܣ�������д����Щ������ʹ����Ҫ��bar0��ӳ�������⴦��

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_HD_MEM_OP_H_20170301
#define SLK_HD_MEM_OP_H_20170301


#include "slk_struct.h"


struct slk_tx_mem_op_list
{
	int							m_i_begin;			/// �ڵ���ʼλ��
	int							m_i_ready;			/// �ڵ����λ��
	unsigned long				m_p_bar1_base;		/// bar1��ʼλ��
	unsigned long				m_p_bar0_base;		/// bar0��ʼλ��
	unsigned long				m_p_beg_addr;		/// bar1д�ڴ���ʼλ��
	volatile uint32_t			counter;			/// tx���ؼ���������ʽ�����ϰ汾��
	volatile uint32_t			snd_counter;		/// ��¼���ͳ�ȥ�İ��� add by liminghuan 2018-07-20

	struct slk_coher_map_dma	m_tx_dma_count;		/// Ӳ���ϱ�����
};

/// ����tx mem op����
/// addr: Ӳ���ڴ��ַ
/// bar1_base: Ӳ��
/// tx_count: tx�ڵ�����
/// bar0_base: bar0�ĵ�ַ����0
extern struct slk_tx_mem_op_list* slk_create_tx_mem_op_list(unsigned long addr,unsigned long bar1_base,int tx_count,unsigned long bar0_base,SLK_PTR_PCI_DEV p_dev);

extern void slk_destroy_tx_mem_op_list(struct slk_tx_mem_op_list* p_lst,SLK_PTR_PCI_DEV p_dev);

extern int slk_tx_mem_op_list_write(const char* msg, int len,struct slk_tx_mem_op_list* p_lst);

extern int slk_tx_mem_op_list_write_safe(const char* msg, int len,struct slk_tx_mem_op_list* p_lst);

extern int slk_tx_mem_op_list_write_debug(const char* msg, int len,struct slk_tx_mem_op_list* p_lst, char* mem, int mem_len);

extern int slk_tx_mem_op_list_ready(struct slk_tx_mem_op_list* p_lst);

extern int slk_tx_mem_op_check_recover(struct slk_tx_mem_op_list* p_lst);

#endif