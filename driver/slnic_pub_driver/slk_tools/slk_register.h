/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		寄存器操作相关函数

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef MGO_REGISTER_H_20121121
#define MGO_REGISTER_H_20121121

#include "slk_type.h"

#define			USER_CHILD_REG_LEN		13
#define			USER_CHILD_REG_TYPE_LEN	1
#define			USER_CHILD_REG_BAR_LEN	4
#define			USER_CHILD_REG_ADDR_LEN	4
#define			USER_CHILD_REG_DATA_LEN 4

#define			REG_ML_TYPE_READ		'R'
#define			REG_ML_TYPE_WRIT		'W'
#define			REG_ML_TYPE_HIG_READ	'r'
#define			REG_ML_TYPE_HIG_WRIT	'w'
#define			REG_ML_TYPE_COMP		'C'

#define			REG_HIGH_READ_MEM		'u'
#define			REG_HIGH_WRITE_MEM		'd'

#pragma pack(push, 1)
struct reg_high_layer
{
	uint8_t			op_type;
	uint32_t		base_addr;
	uint32_t		reg_addr;
	uint32_t		reg_data;
};

struct slk_hight_reg
{
	uint8_t			m_reg_type;
	uint32_t		m_reg_count;
	uint8_t			m_reg_subtype;
	uint32_t		m_base_addr;
	uint32_t		m_reg_addr;
	uint32_t		m_reg_data;
	uint32_t		m_user_data;
};

#pragma pack(pop)

struct reg_multi_layer
{
	uint32_t		reg_addr;
	uint32_t		bar_count;
	uint8_t			ctl_type;
	union
	{
		uint32_t	write_data;
		uint32_t	comp_data;
	}reg_data;
};

struct fpga_reg_ctl_unit
{
	int						   i_reg_count;
	struct reg_multi_layer	   reg_layer[32];
	unsigned int			   reg_number;					
};

extern void     conver_user_to_reg(const char* ch_reg_data, struct fpga_reg_ctl_unit* reg_unit);
extern uint32_t	fpga_reg_operation(int addr_reg_count, struct reg_multi_layer* addr_reg_array, unsigned long* bar_addrs, int bar_count);
extern uint32_t	slk_high_child_register(const char* ch_reg_data, unsigned long* bar_addrs);
extern uint32_t slk_high_register(struct slk_hight_reg* reg, unsigned long* bar_addrs );


#endif

