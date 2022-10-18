/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		文件实现对内核态下内存的整合和管理功能，主要用途是将内核内存提供给应用层使用

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_GFP_H_20161109
#define SLK_GFP_H_20161109

#define  SLK_MAX_GFP_NODE_COUNT		(1024*256)
#define  SLK_MAX_GFP_PAGE_COUNT		(4096*2)

#pragma pack(push, 1)
struct slk_gfp
{
	char*			m_p_node[SLK_MAX_GFP_NODE_COUNT];
	int				m_i_node_count;
	int				m_i_node_size;
	char*			m_p_page[SLK_MAX_GFP_PAGE_COUNT];
	int				m_i_page_count;
};

#pragma pack(pop)

/// 创建gfp对象,1、node_count < SLK_MAX_GFP_NODE_COUNT 2、(4096 % node_size) == 0 3、总内存大小的页数小于SLK_MAX_GFP_PAGE_COUNT
extern struct slk_gfp* slk_gfp_create(int node_size,int node_count);
/// 销毁gfp对象
extern void slk_gfp_destroy(struct slk_gfp* p_gfp);
/// 获取gfp节点对象
extern char* slk_gfp_get_node(struct slk_gfp* p_gfp,int node_idx);

extern int   slk_gfp_copy(struct slk_gfp* p_fgp, char* data, int len);

#endif