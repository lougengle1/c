/*!****************************************************************************
	@file   	slk_define.h
	@date		2014/08/29   00:00
	@author 	liu.sheng
	
	@brief		������־�ı�־��дʱ��

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_LOG_H_20160801
#define SLK_LOG_H_20160801

#include "slk_define.h"
#include "slk_type.h"

extern void slk_binary(uint8_t dir,const char* msg, int len);
extern int  slk_create_default_log(int __size);
extern void slk_destory_default_log(void);
extern void slk_print_default(const char* head, const char* msg, int len, int seq);

extern void slk_print_bin(const char* head,unsigned char* msg, int len, int seq);
extern void slk_print_local(const char* head,unsigned char* msg, int len, int seq);

#endif