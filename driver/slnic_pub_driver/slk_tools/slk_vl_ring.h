/*!****************************************************************************
	@file   	slk_vl_ring.h
	@date		2019/11/20   00:00
	@author 	li.minghuan

	@brief		支持巨大内存模式，变长写入数据，一个线程写一个线程读安全

	@note		作者			时间				版本				修改原因
	@note	 	li.minghuan		2019/11/20			1.0.0				创建
******************************************************************************/
#ifndef SLK_VL_RING_H_20191120
#define SLK_VL_RING_H_20191120 

#define SLK_VLRING_BAR_LEN		(32)

#pragma pack(push, 1)
struct slk_vl_ring
{
	volatile int            m_i_write_begin;		/// 写入开始位置
	volatile int            m_i_read_begin;			/// 读取开始位置
	volatile int            m_i_cache_end;			/// 数据实际写入的结束位置	

	int                     m_i_cache_len;			/// 数据缓存的长度

	char*                   m_ch_cache_begin;		/// 缓存的起始地址
	char*					m_ch_resv;
};
#pragma pack(pop)

/// 初始化不定长ring，size表示ring的大小
extern struct slk_vl_ring* create_slk_vl_ring(int size);

/// 初始化在指定长度上的内存上映射ring,内存由外面释放
/// *************************
//// |--slk_vl_ring--|
///  |----32字节-----|
///  |----数据区-----|
///**************************
extern struct slk_vl_ring* create_slk_vl_ring_mmap(void* ptr, int ptr_len, int que_len);

/// 销毁不定长ring
extern void destory_slk_vl_ring(struct slk_vl_ring* p_ring);

/// 获取有效数据头指针
extern const char* get_data(struct slk_vl_ring* p_ring, int* ret_len);

/// 设置处理的长度
extern void set_proc_len(struct slk_vl_ring* p_ring, int len);

/// 写入数据
extern int push_data(struct slk_vl_ring* p_ring, const char* msg, int len);

#endif

