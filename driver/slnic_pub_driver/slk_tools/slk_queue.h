/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		�����е���������֧��Ӧ�ò�����������������������û�ж��ⱸ�ݽڵ㣬�����Ż����ṹ��������Ϊ������

	@note		����			ʱ��				�汾				�޸�ԭ��
	@note	 	liu.sheng		2013/08/29			1.0.0				����  
******************************************************************************/
#ifndef SLK_QUEUE_PROC_H_20120605
#define SLK_QUEUE_PROC_H_20120605

#include "slk_type.h"

#ifndef		FPGA_QUOTE_BUF_MAX_SIZE
#define		FPGA_QUOTE_BUF_MAX_SIZE	4096*512	/// ���黺����е���󻺴�������
#endif


#define		FPGA_LOG_DATA_SIZE		1024		/// log���е����ݿ��С
#define		FPGA_LOG_QUE_LEN		1024		/// log���еĳ���

#define		FPGA_TX_RING_LEN		16			/// ���Ͷ��еĳ���
#define		FPGA_TX_DMA_LEN			16			/// ��Ӳ��������dma���г���

#define		FPGA_DATA_BUFFER_SIZE	2048		/// dma���ݿ��С

/// ��Ӳ��������dma��
struct slk_packet
{
	struct slk_packet*		next;

	char*				data;
	unsigned int			datalen; //tx: dw ����,rx: bytes
	SLK_PTR_DAM_ADDR_T				dma_addr;
};


struct slk_ring
{
	struct slk_packet*		packet_ring;

	struct slk_packet*		beg_empty;			/// �ն��е���ʼλ��
	struct slk_packet*		beg_proc;			/// proc������ʼλ��
	struct slk_packet*		beg_dma;			/// dma������ʼλ��

	volatile uint32_t		counter;			/// ��ǰ��������
	volatile uint32_t		snd_counter;		/// ��¼���ͳ�ȥ�İ��� ls 20180518

	SLK_SPINLOCK_T				lock;				/// ����������
};

/// ���ӵȵ�����Ϣ����
struct slk_log_node
{
	struct slk_log_node*		next;
	uint8_t						ch_data[FPGA_LOG_DATA_SIZE];
	int							len;
};

struct slk_log_queue
{
	struct slk_log_node*		log_queue;
	struct slk_log_node*		beg_empty;
	struct slk_log_node*		beg_storage;

	SLK_SPINLOCK_T					log_lock;	/// ����������
};

/// ���鵼������
struct slk_quote_node
{
	uint8_t*					p_pos;
	struct slk_quote_node*		next;
};


struct slk_quote_queue
{
	uint8_t						ch_data[FPGA_QUOTE_BUF_MAX_SIZE];
	struct slk_quote_node*		read_head;			/// ��ȡͷ
	struct slk_quote_node*		p_read_pos;			/// ��ǰ��ȡ��λ��
	struct slk_quote_node*		write_head;			/// д��ͷ
	struct slk_quote_node*		p_write_pos;		/// д��λ��
		
	unsigned int				loss_pkt;			/// ��ʧ������

	uint32_t					node_size;			/// ÿ���ڵ�Ĵ�С
	uint32_t					node_count;			/// �ܹ��ڵ������

	SLK_SPINLOCK_T					quote_read_lock;	/// ����������
};


struct slk_data_time
{
	uint8_t         m_nop;				///< ռλ
	uint8_t			m_hour;				///< Сʱ��
	uint8_t			m_minute;			///< ������
	uint8_t			m_sec;				///< ����
	uint32_t		m_nanosec;			///< ������
};

///************************************************************************************
//udp queue,������Ӧ�ò㽻���Ķ���
extern struct slk_quote_queue* slk_setup_quote_ring(uint32_t node_size, uint32_t node_count);

/// ����ֵ��0��ȷ������������
extern int slk_write_quote(struct slk_quote_queue* q_queue,uint8_t* data, uint32_t len);

extern int slk_write_quote_resend(struct slk_quote_queue* q_queue,uint8_t* data, uint32_t len,int count);

extern int slk_read_quote(struct slk_quote_queue* q_queue,uint8_t* data, uint32_t len);

extern int slk_queue_used_count(struct slk_quote_queue* q_queue);

/// �ж϶����ǲ��ǿգ�0���� 1���ǿ�
extern int slk_quote_empty(struct slk_quote_queue* q_queue);

extern void slk_clear_quote_ring(struct slk_quote_queue* q_queue);

///***********************************************************************************
// rx,ע�⣺�ڵ���rx_pop_data_from_hw�������ݲ�������Ҫ�ڴ��������dmaӳ��
extern struct slk_ring* slk_setup_rx_ring(SLK_DEVICE p_pci_dev,unsigned long bas_addr);
extern struct slk_ring* slk_setup_rx_ring_sync_single(SLK_DEVICE p_pci_dev,unsigned long bas_addr);
extern struct slk_ring* slk_setup_mult_rx_ring_sync_single(SLK_DEVICE p_pci_dev,unsigned long bas_addr,unsigned int reg_addr,int rx);

/// ����ֵ��0��ȷ������������
extern int slk_rx_push_buf_to_hw(struct slk_ring* p_rx_ring,unsigned long bas_addr);
extern int slk_rx_push_buf_to_hw_addr(struct slk_ring* p_rx_ring,unsigned long bas_addr,unsigned int addr,int rx);

extern int slk_rx_pop_data_from_hw(struct slk_ring* p_rx_ring,unsigned long bas_addr);
extern int slk_rx_pop_data_from_hw_addr(struct slk_ring* p_rx_ring,unsigned long bas_addr,unsigned int reg_addr,int rx);

extern void slk_clear_rx_ring(struct slk_ring* p_rx_ring,SLK_DEVICE p_pci_dev);
extern void slk_clear_rx_ring_sync_single(struct slk_ring* p_rx_ring,SLK_DEVICE p_pci_dev);

///***********************************************************************************
/// tx
extern struct slk_ring* slk_setup_tx_ring(void);

extern int slk_tx_push_data_to_hw_dw(struct slk_ring* p_tx_ring,SLK_DEVICE p_pci_dev,unsigned long bas_addr,char *buf, int len);

extern int slk_tx_push_data_to_hw(struct slk_ring* p_tx_ring,SLK_DEVICE p_pci_dev,unsigned long bas_addr,char *buf, int len);

extern int slk_tx_push_data_to_hw_dw_safe(struct slk_ring* p_tx_ring,SLK_DEVICE p_pci_dev,unsigned long bas_addr,char *buf, int len);

extern int slk_tx_push_data_to_hw_safe(struct slk_ring* p_tx_ring,SLK_DEVICE p_pci_dev,unsigned long bas_addr,char *buf, int len);

extern int slk_tx_pop_free_from_hw(struct slk_ring* p_tx_ring);

extern void slk_clear_tx_ring(struct slk_ring* p_tx_ring,SLK_DEVICE p_pci_dev);

///***********************************************************************************
/// log ����
extern struct slk_log_queue* slk_setup_log_ring(void);

extern int slk_write_log(struct slk_log_queue* q_log,uint8_t msg_type, uint8_t* ch_buff, int len);

extern int slk_write_raw_log(struct slk_log_queue* q_log, uint8_t* ch_buff, int len );

extern int slk_read_log (struct slk_log_queue* q_log,uint8_t* ch_buff,int len);

extern void slk_clear_log_ring(struct slk_log_queue* q_log);

#endif