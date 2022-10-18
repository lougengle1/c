/*!****************************************************************************
	@file   	slk_define.h
	@date		2016/08/29   00:00
	@author 	liu.sheng
	
	@brief		驱动中的无锁链表，支持应用层和驱动间的无锁交互，并且没有额外备份节点，性能优化，结构更清晰，为第三版

	@note		作者			时间				版本				修改原因
	@note	 	liu.sheng		2013/08/29			1.0.0				创建  
******************************************************************************/
#ifndef SLK_QUEUE_PROC_H_20120605
#define SLK_QUEUE_PROC_H_20120605

#include "slk_type.h"

#ifndef		FPGA_QUOTE_BUF_MAX_SIZE
#define		FPGA_QUOTE_BUF_MAX_SIZE	4096*512	/// 行情缓存队列的最大缓存区长度
#endif


#define		FPGA_LOG_DATA_SIZE		1024		/// log队列的数据块大小
#define		FPGA_LOG_QUE_LEN		1024		/// log队列的长度

#define		FPGA_TX_RING_LEN		16			/// 发送队列的长度
#define		FPGA_TX_DMA_LEN			16			/// 与硬件交互的dma队列长度

#define		FPGA_DATA_BUFFER_SIZE	2048		/// dma数据块大小

/// 与硬件交互的dma环
struct slk_packet
{
	struct slk_packet*		next;

	char*				data;
	unsigned int			datalen; //tx: dw 对齐,rx: bytes
	SLK_PTR_DAM_ADDR_T				dma_addr;
};


struct slk_ring
{
	struct slk_packet*		packet_ring;

	struct slk_packet*		beg_empty;			/// 空队列的起始位置
	struct slk_packet*		beg_proc;			/// proc队列起始位置
	struct slk_packet*		beg_dma;			/// dma队列起始位置

	volatile uint32_t		counter;			/// 当前包总数量
	volatile uint32_t		snd_counter;		/// 记录发送出去的包数 ls 20180518

	SLK_SPINLOCK_T				lock;				/// 队列自旋锁
};

/// 日子等导出信息部分
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

	SLK_SPINLOCK_T					log_lock;	/// 队列自旋锁
};

/// 行情导出部分
struct slk_quote_node
{
	uint8_t*					p_pos;
	struct slk_quote_node*		next;
};


struct slk_quote_queue
{
	uint8_t						ch_data[FPGA_QUOTE_BUF_MAX_SIZE];
	struct slk_quote_node*		read_head;			/// 读取头
	struct slk_quote_node*		p_read_pos;			/// 当前读取的位置
	struct slk_quote_node*		write_head;			/// 写入头
	struct slk_quote_node*		p_write_pos;		/// 写入位置
		
	unsigned int				loss_pkt;			/// 丢失包数量

	uint32_t					node_size;			/// 每个节点的大小
	uint32_t					node_count;			/// 总共节点的数量

	SLK_SPINLOCK_T					quote_read_lock;	/// 队列自旋锁
};


struct slk_data_time
{
	uint8_t         m_nop;				///< 占位
	uint8_t			m_hour;				///< 小时数
	uint8_t			m_minute;			///< 分钟数
	uint8_t			m_sec;				///< 秒数
	uint32_t		m_nanosec;			///< 纳秒数
};

///************************************************************************************
//udp queue,驱动与应用层交互的队列
extern struct slk_quote_queue* slk_setup_quote_ring(uint32_t node_size, uint32_t node_count);

/// 返回值：0正确，其它：丢包
extern int slk_write_quote(struct slk_quote_queue* q_queue,uint8_t* data, uint32_t len);

extern int slk_write_quote_resend(struct slk_quote_queue* q_queue,uint8_t* data, uint32_t len,int count);

extern int slk_read_quote(struct slk_quote_queue* q_queue,uint8_t* data, uint32_t len);

extern int slk_queue_used_count(struct slk_quote_queue* q_queue);

/// 判断队列是不是空，0：空 1：非空
extern int slk_quote_empty(struct slk_quote_queue* q_queue);

extern void slk_clear_quote_ring(struct slk_quote_queue* q_queue);

///***********************************************************************************
// rx,注意：在调用rx_pop_data_from_hw后获得数据并处理，需要在处理后重新dma映射
extern struct slk_ring* slk_setup_rx_ring(SLK_DEVICE p_pci_dev,unsigned long bas_addr);
extern struct slk_ring* slk_setup_rx_ring_sync_single(SLK_DEVICE p_pci_dev,unsigned long bas_addr);
extern struct slk_ring* slk_setup_mult_rx_ring_sync_single(SLK_DEVICE p_pci_dev,unsigned long bas_addr,unsigned int reg_addr,int rx);

/// 返回值：0正确，其它：丢包
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
/// log 队列
extern struct slk_log_queue* slk_setup_log_ring(void);

extern int slk_write_log(struct slk_log_queue* q_log,uint8_t msg_type, uint8_t* ch_buff, int len);

extern int slk_write_raw_log(struct slk_log_queue* q_log, uint8_t* ch_buff, int len );

extern int slk_read_log (struct slk_log_queue* q_log,uint8_t* ch_buff,int len);

extern void slk_clear_log_ring(struct slk_log_queue* q_log);

#endif