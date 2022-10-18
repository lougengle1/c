/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		文件实现内核下直接操作硬盘文件的功能，目前需要文件已经存在

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_FILE_H_20150227
#define SLK_FILE_H_20150227

/// 注意：以下接口只能在进程上下文中使用，不能再中断上下文中用

// #include <linux/fs.h>
#include "slk_type.h"

extern int slk_open_hd_file (const char* file_name, int flag, SLK_OP_FILES hd_file );
extern int slk_read_hd_file (SLK_OP_FILES filp,char* buffer, int len);
extern int slk_write_hd_file(SLK_OP_FILES filp,char* buffer, int len);
extern int slk_close_hd_file(SLK_OP_FILES filp);

#endif
