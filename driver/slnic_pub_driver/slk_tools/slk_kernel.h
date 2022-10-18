/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		文件定义一些和linux操作系统版本相关的宏

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建
******************************************************************************/
#ifndef SLK_KERNEL_20160218
#define SLK_KERNEL_20160218

#include <linux/version.h>


/// 3.7.0及以上版本不在支持VM_RESERVED宏
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
	#include <linux/rtc.h>
	#define SLK_VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#else
	#include <linux/rtc.h>
	#define SLK_VM_RESERVED VM_RESERVED
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	#include "slk_4_14_x.h"
	#define SLK_KERNEL_WRITE(a,b,c,d) kernel_write(a,b,c,d)
#else
	#define SLK_KERNEL_WRITE(a,b,c,d) vfs_write(a,b,c,d)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	#include "slk_4_14_x.h"
	#define SLK_KERNEL_READ(a,b,c,d) kernel_read(a,b,c,d)
#else
	#define SLK_KERNEL_READ(a,b,c,d) vfs_read(a,c,d,b)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
	#include "slk_5.0.0x.h"
	#define GET_KERNEL_TIME(SL_TIME_IN) sl_get_kernel_time(SL_TIME_IN)
#else
 	#define GET_KERNEL_TIME(SL_TIME_IN) do_gettimeofday(SL_TIME_IN)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	#include "slk_4_14_x.h"
	#include <linux/timer.h>
	#define SLK_CLCOK_SETUP(a,b,c) timer_setup(a,b,0)
#else
	#include <linux/timer.h>
	#define SLK_CLCOK_SETUP(a,b,c) setup_timer(a,b,c)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,1,0)
  #define STRUCT_TIMEX __kernel_timex
#else
  #define STRUCT_TIMEX timex
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define ioremap_nocache ioremap
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
  #define RTC_TIME_TO_TM(a,b)	rtc_time64_to_tm(a,b);
#else
  #define RTC_TIME_TO_TM(a,b)	rtc_time_to_tm(a,b);
#endif

#endif

