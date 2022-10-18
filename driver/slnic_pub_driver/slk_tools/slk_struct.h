/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		����tools�õ��Ľṹ

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_STRUCT_H_20160922
#define SLK_STRUCT_H_20160922

#include "slk_type.h"

#pragma pack(push, 1)

struct slk_type_pchar
{
	char*			m_ch_buffer;
};

struct slk_type_pu32
{
	uint32_t*		m_u32_addr;
};

struct slk_type_dma_addr
{
	SLK_PTR_DAM_ADDR_T		m_dam_addr;
};

struct slk_tx_data
{
	unsigned char				 m_ch_flag;
	unsigned int				 m_i_seq;
	unsigned int				 m_i_pld_len;
	char						 m_ch_data[2048];
};

struct slk_que_attach
{
	unsigned char				 m_ch_flag;
	unsigned int				 m_i_seq;
	unsigned int				 m_i_pld_len;
	unsigned char				 m_ch_sub_flag;
	unsigned int				 m_i_queue_id;
};

struct slk_driver_head
{
	unsigned char				m_ch_flag;
	unsigned int				m_i_seq;
	unsigned int				m_i_pld;
};



#pragma pack(pop)



struct slk_coher_map_dma
{
	SLK_PTR_DAM_ADDR_T	m_dam_addr;
	void*				m_vis_addr;
	int					m_i_len;
};

struct slk_cdev_cls
{
    SLK_CDEV					m_cdev;			    ///<���������ַ��豸
    SLK_CLASS               	m_dev_cls;			/// �ַ��豸��Ӧ����
    SLK_DEV_T						m_dev_t;            ///<�ַ��豸��
};

#endif
