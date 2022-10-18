/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng

	@brief		�ļ�ʵ�������г��õ��ĺ������ܣ��Ƕ�slk_define�Ĳ���

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����
******************************************************************************/
#ifndef SLK_FUNCTION_H_20131016
#define SLK_FUNCTION_H_20131016

#include "slk_struct.h"

extern void slk_s2u_swap(uint64_t src, uint64_t* dest, uint32_t dest_len);

extern void slk_u2s_swap(uint64_t src, uint64_t* dest, uint32_t dest_len);

extern void* slk_molloc( int len );

extern void  slk_free(void* ptr);

extern u16  pcie30_reset(void* p_pci );

extern u16  pcie30_disable(void* p_pci );

extern u16  pcie30_enable(void* p_pci);

extern int read_pcie_parent_maxpayload(SLK_PTR_PCI_DEV dev);

extern void* create_mapping_page(void);

extern void  free_mapping_page(void* page);

extern uint32_t wait_for_hw_finish(uint32_t base,uint32_t reg, uint32_t comp_value);

extern uint32_t wait_for_hw_finish_mutil(uint32_t base,uint32_t reg, int comp_count, uint32_t* comp_value, int wait_sec );

extern int wait_for_hw_finish_s10(SLK_PTR_PCI_DEV dev,unsigned long base);

extern int create_coher_mapping_mem(SLK_PTR_PCI_DEV dev,int page_count, struct slk_coher_map_dma* coher);

extern int destory_coher_mapping_mem(SLK_PTR_PCI_DEV dev,struct slk_coher_map_dma* coher);

extern void slk_dma2cpu(SLK_PTR_DAM_ADDR_T dma,SLK_PTR_PCI_DEV p_pci,long dir,int len);

extern void slk_dma2dev(SLK_PTR_DAM_ADDR_T dma,SLK_PTR_PCI_DEV p_pci,long dir,int len);

extern int  slk_get_utc_time(int* year,int* mon,int* day,int* hour,int* min,int* sec,int* usec);

extern int  slk_get_local_time(int* year, int* mon, int* day, int* hour, int* min, int* sec, int* usec);

/// ����pcieģʽΪ64λģʽ��
extern int slk_set_pci_dma_64bit(SLK_PTR_PCI_DEV pdev);

#endif
