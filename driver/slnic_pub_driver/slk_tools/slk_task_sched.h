/*!****************************************************************************
	@file   	slk_define.h
	@date		2014/08/29   00:00
	@author 	liu.sheng
	
	@brief		进程调度相关的函数包装

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_TASK_SCHED_H_20131121
#define SLK_TASK_SCHED_H_20131121
//#include <linux/spinlock.h>
//#include <linux/wait.h>

#include "slk_type.h"

struct slk_task_ctl
{
	SLK_WAIT_QUEUE_HEAD_T		m_event;	
	uint32_t					m_flag;		/// 0: sleep 1:wake up
};

extern struct slk_task_ctl* slk_create_task_ctl(void);
extern void   slk_destroy_task_ctl(struct slk_task_ctl* ctl);

extern void slk_set_task_sleep (struct slk_task_ctl* ctl);
extern void slk_set_task_runing(struct slk_task_ctl* ctl);

#endif