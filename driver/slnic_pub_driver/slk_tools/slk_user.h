/*!****************************************************************************
	@file   	lock.h
	@date		2013/08/29   00:00
	@author 	liu.sheng
	
	@brief		这是一个多用户的管理类，用来实现驱动中的多用户.

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_USER_H_20130829
#define SLK_USER_H_20130829

// #include <linux/semaphore.h>
#include "slk_type.h"

struct slk_sgl_user
{
	uint32_t					index;			/// 当前所在列表的索引
	uint32_t					is_used;		/// 当前用户是否用到
	struct slk_quote_queue*		p_queue;		/// 用户用来存储数据的队列（可选）
	void*						p_data;			/// 用户自定义数据（可选）
};

struct slk_user
{
	struct slk_sgl_user*		p_sgl_user;
	uint32_t					count;

	/// 这里使用信号量而不使用自旋锁，是由于自旋锁在等待过程中不可中断的。
	/// 1、当一个用户进程A获取到自旋锁，并在操作slk_user时被中断了。
	/// 2、这时恰好等待这个锁的进程B运行，这个cpu就会被锁死。

	/// 而使用信号量就可以避免这个问题。因为信号量是可中断的。
	/// 1、当一个用户进程A获取到信号量，并在操作slk_user时被中断了。
	/// 2、这时恰好等待这个信号量的进程B运行，进程B在等待一段时间后，
	///	   由于时间片用完或其它中断产生，会让出cpu，
	///	   这样进程A就有机会执行完毕，让出信号量共A执行。
	SLK_SEMAPHORE			user_sem;		
};
/// 创建一个slk_user，count为sgl_user数量
extern struct slk_user* slk_create_user( uint32_t count );
/// 销毁一个slk_user
extern void slk_destroy_user( struct slk_user* user );

/// 根据索引查找一个sgl_user
extern struct slk_sgl_user* slk_find_sgl_user( struct slk_user* user, uint32_t index );
/// 获取一个sgl_user,并设置为使用
extern struct slk_sgl_user* slk_get_sgl_user( struct slk_user* user );
/// 添加一个sgl_user用户
extern int slk_add_sgl_user( struct slk_user* user, struct slk_quote_queue* que, void*	p_data);
/// 删除一个sgl_user用户
extern void		slk_del_sgl_user( struct slk_user* user, uint32_t index );

#endif 

