/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		驱动中的hash map

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_MAP_H_20130806
#define SLK_MAP_H_20130806

#include "slk_type.h"

struct slk_map_elem
{
	struct slk_list_head	list;		/// 双向队列
	uint32_t				key;		/// key
	void*					data;		/// 数据

	uint32_t				tb_size;

	uint32_t				count;		
};

/// 创建一个slk map
/// @param tb_size		hash table的大小
/// @return				创建的slk map
extern struct slk_map_elem* slk_map_create(uint32_t tb_size);

/// 插入一个元素
/// @param	key			插入元素的key值
/// @param	p_data		插入元素的value值
/// @param	map			元素插入的容器
/// @param	tb_size		map的hash表大小
/// @return				0：插入成功。其它插入失败
extern int    slk_map_insert(uint32_t key,void* p_data,struct slk_map_elem* map);

/// 删除一个元素
/// @param	key			删除元素的key值
/// @param	map			元素删除的容器
/// @param	tb_size		map的hash表大小
/// @return				0：删除成功。其它删除失败
extern int    slk_map_remove(uint32_t key,struct slk_map_elem* map);

/// 清空map
/// @param	map			元素删除的容器
/// @param	tb_size		map的hash表大小
/// @return	
extern void   slk_map_clear(struct slk_map_elem* map);

/// 查找一个元素
/// @param	key			需要查找元素的key值
/// @param	map			元素所在的容器
/// @param	tb_size		map的hash表大小
/// @return	对应的元素值
extern void*  slk_map_find_data(uint32_t key,struct slk_map_elem* map);

#endif