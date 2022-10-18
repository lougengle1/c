/*!****************************************************************************
	@file   	slk_vl_ring.h
	@date		2019/11/20   00:00
	@author 	li.minghuan

	@brief		֧�־޴��ڴ�ģʽ���䳤д�����ݣ�һ���߳�дһ���̶߳���ȫ

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	li.minghuan		2019/11/20			1.0.0				����
******************************************************************************/
#ifndef SLK_VL_RING_H_20191120
#define SLK_VL_RING_H_20191120 

#define SLK_VLRING_BAR_LEN		(32)

#pragma pack(push, 1)
struct slk_vl_ring
{
	volatile int            m_i_write_begin;		/// д�뿪ʼλ��
	volatile int            m_i_read_begin;			/// ��ȡ��ʼλ��
	volatile int            m_i_cache_end;			/// ����ʵ��д��Ľ���λ��	

	int                     m_i_cache_len;			/// ���ݻ���ĳ���

	char*                   m_ch_cache_begin;		/// �������ʼ��ַ
	char*					m_ch_resv;
};
#pragma pack(pop)

/// ��ʼ��������ring��size��ʾring�Ĵ�С
extern struct slk_vl_ring* create_slk_vl_ring(int size);

/// ��ʼ����ָ�������ϵ��ڴ���ӳ��ring,�ڴ��������ͷ�
/// *************************
//// |--slk_vl_ring--|
///  |----32�ֽ�-----|
///  |----������-----|
///**************************
extern struct slk_vl_ring* create_slk_vl_ring_mmap(void* ptr, int ptr_len, int que_len);

/// ���ٲ�����ring
extern void destory_slk_vl_ring(struct slk_vl_ring* p_ring);

/// ��ȡ��Ч����ͷָ��
extern const char* get_data(struct slk_vl_ring* p_ring, int* ret_len);

/// ���ô���ĳ���
extern void set_proc_len(struct slk_vl_ring* p_ring, int len);

/// д������
extern int push_data(struct slk_vl_ring* p_ring, const char* msg, int len);

#endif

