/*!****************************************************************************
	@file   	lock.h
	@date		2013/08/29   00:00
	@author 	liu.sheng
	
	@brief		����һ�����û��Ĺ����࣬����ʵ�������еĶ��û�.

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_USER_H_20130829
#define SLK_USER_H_20130829

// #include <linux/semaphore.h>
#include "slk_type.h"

struct slk_sgl_user
{
	uint32_t					index;			/// ��ǰ�����б������
	uint32_t					is_used;		/// ��ǰ�û��Ƿ��õ�
	struct slk_quote_queue*		p_queue;		/// �û������洢���ݵĶ��У���ѡ��
	void*						p_data;			/// �û��Զ������ݣ���ѡ��
};

struct slk_user
{
	struct slk_sgl_user*		p_sgl_user;
	uint32_t					count;

	/// ����ʹ���ź�������ʹ�����������������������ڵȴ������в����жϵġ�
	/// 1����һ���û�����A��ȡ�������������ڲ���slk_userʱ���ж��ˡ�
	/// 2����ʱǡ�õȴ�������Ľ���B���У����cpu�ͻᱻ������

	/// ��ʹ���ź����Ϳ��Ա���������⡣��Ϊ�ź����ǿ��жϵġ�
	/// 1����һ���û�����A��ȡ���ź��������ڲ���slk_userʱ���ж��ˡ�
	/// 2����ʱǡ�õȴ�����ź����Ľ���B���У�����B�ڵȴ�һ��ʱ���
	///	   ����ʱ��Ƭ����������жϲ��������ó�cpu��
	///	   ��������A���л���ִ����ϣ��ó��ź�����Aִ�С�
	SLK_SEMAPHORE			user_sem;		
};
/// ����һ��slk_user��countΪsgl_user����
extern struct slk_user* slk_create_user( uint32_t count );
/// ����һ��slk_user
extern void slk_destroy_user( struct slk_user* user );

/// ������������һ��sgl_user
extern struct slk_sgl_user* slk_find_sgl_user( struct slk_user* user, uint32_t index );
/// ��ȡһ��sgl_user,������Ϊʹ��
extern struct slk_sgl_user* slk_get_sgl_user( struct slk_user* user );
/// ���һ��sgl_user�û�
extern int slk_add_sgl_user( struct slk_user* user, struct slk_quote_queue* que, void*	p_data);
/// ɾ��һ��sgl_user�û�
extern void		slk_del_sgl_user( struct slk_user* user, uint32_t index );

#endif 

