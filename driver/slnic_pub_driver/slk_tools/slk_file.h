/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		�ļ�ʵ���ں���ֱ�Ӳ���Ӳ���ļ��Ĺ��ܣ�Ŀǰ��Ҫ�ļ��Ѿ�����

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_FILE_H_20150227
#define SLK_FILE_H_20150227

/// ע�⣺���½ӿ�ֻ���ڽ�����������ʹ�ã��������ж�����������

// #include <linux/fs.h>
#include "slk_type.h"

extern int slk_open_hd_file (const char* file_name, int flag, SLK_OP_FILES hd_file );
extern int slk_read_hd_file (SLK_OP_FILES filp,char* buffer, int len);
extern int slk_write_hd_file(SLK_OP_FILES filp,char* buffer, int len);
extern int slk_close_hd_file(SLK_OP_FILES filp);

#endif
