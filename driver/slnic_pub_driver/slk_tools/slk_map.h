/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		�����е�hash map

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_MAP_H_20130806
#define SLK_MAP_H_20130806

#include "slk_type.h"

struct slk_map_elem
{
	struct slk_list_head	list;		/// ˫�����
	uint32_t				key;		/// key
	void*					data;		/// ����

	uint32_t				tb_size;

	uint32_t				count;		
};

/// ����һ��slk map
/// @param tb_size		hash table�Ĵ�С
/// @return				������slk map
extern struct slk_map_elem* slk_map_create(uint32_t tb_size);

/// ����һ��Ԫ��
/// @param	key			����Ԫ�ص�keyֵ
/// @param	p_data		����Ԫ�ص�valueֵ
/// @param	map			Ԫ�ز��������
/// @param	tb_size		map��hash���С
/// @return				0������ɹ�����������ʧ��
extern int    slk_map_insert(uint32_t key,void* p_data,struct slk_map_elem* map);

/// ɾ��һ��Ԫ��
/// @param	key			ɾ��Ԫ�ص�keyֵ
/// @param	map			Ԫ��ɾ��������
/// @param	tb_size		map��hash���С
/// @return				0��ɾ���ɹ�������ɾ��ʧ��
extern int    slk_map_remove(uint32_t key,struct slk_map_elem* map);

/// ���map
/// @param	map			Ԫ��ɾ��������
/// @param	tb_size		map��hash���С
/// @return	
extern void   slk_map_clear(struct slk_map_elem* map);

/// ����һ��Ԫ��
/// @param	key			��Ҫ����Ԫ�ص�keyֵ
/// @param	map			Ԫ�����ڵ�����
/// @param	tb_size		map��hash���С
/// @return	��Ӧ��Ԫ��ֵ
extern void*  slk_map_find_data(uint32_t key,struct slk_map_elem* map);

#endif