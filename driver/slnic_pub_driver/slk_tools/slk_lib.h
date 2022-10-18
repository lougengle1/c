#ifndef SLK_LIB_H_20200908
#define SLK_LIB_H_20200908

#include "slk_tcp.h"
#include "slk_hw_mem_op.h"


#ifdef			SHENGLI_LOG
// debug日志
#ifdef			SHENGLI_LOG_DEBUG
#define			SLK_DEBUG_LOG               1
#else
#define			SLK_DEBUG_LOG               0
#endif

/// 错误日志
#  ifdef		SHENGLI_LOG_ERROR
#define			SLK_ERROR_LOG				1
#else
#define			SLK_ERROR_LOG               0
#endif

// 警告日志
#ifdef			SHENGLI_LOG_WARNING
#define			SLK_WARNING_LOG				1
#else
#define			SLK_WARNING_LOG				0
#endif

/// data日志，标准双向数据打印格式
#ifdef			SHENGLI_LOG_DATA
#define			SLK_LOG_DATA				1
#else
#define			SLK_LOG_DATA				0
#endif

/// 临时日志
#ifdef			SHENGLI_LOG_LACAL
#define			SLK_LOG_LACAL				1
#else
#define			SLK_LOG_LACAL				0
#endif

#ifdef 			SHENGLI_LOG_SESSION
#define			SLK_LOG_SESSION				1
#else
#define			SLK_LOG_SESSION				0
#endif

#else
#define			SLK_DEBUG_LOG               0
#define			SLK_ERROR_LOG				0
#define			SLK_WARNING_LOG				0
#define			SLK_LOG_DATA				0
#define			SLK_LOG_LACAL				0
#define			SLK_LOG_SESSION				0
#endif

#ifdef		SHENGLI_S10
#define 	SHENGLI_S10_ONLY				1
#else 
#define 	SHENGLI_S10_ONLY				0
#endif

#if SHENGLI_S10_ONLY
 #define			SLK_MEM_TX_NODE_COUNT			(8)
 #define			SLK_HW_HEAD_LEN					(8)
#else
 #define			SLK_MEM_TX_NODE_COUNT			(16)
 #define			SLK_HW_HEAD_LEN					(32)
#endif


#define 	TMO_GET_MEM_TX					SLK_MEM_TX_NODE_COUNT

void* slk_kmalloc(int size);

void slk_kfree(void* memory_addr);

uint64_t slt_get_gfp_kernel(void);

uint64_t slt_get_kernel_action(void);

void* slk_dma_alloc_coherent(SLK_PTR_PCI_DEV p_dev,int size,SLK_PTR_DAM_ADDR_T dam_addr,uint64_t memory_antion);

void slk_dma_free_coherent(SLK_PTR_PCI_DEV p_dev,int len,void* vis_addr,SLK_PTR_DAM_ADDR_T dam_addr);

void slk_SetPageReserved(unsigned long page);

int slk_pci_set_dma_mask(SLK_PTR_PCI_DEV dev);

int slk_pci_set_consistent_dma_mask(SLK_PTR_PCI_DEV dev);

void slk_ClearPageReserved(unsigned long page);

void slk_spin_lock_init(SLK_SPINLOCK_T rx_chan);

void slk_spin_lock(SLK_SPINLOCK_T lcl_lock);

void slk_spin_unlock(SLK_SPINLOCK_T lcl_lock);

int slt_get_page_size(void);

void slk_strcpy(char* info,const char* src);

void slk_memset(void* buff,int num,int size);

void slk_pci_write_config_word(const SLK_PTR_PCI_DEV dev, int where,u16 val);

void slk_pci_read_config_word(const SLK_PTR_PCI_DEV dev, int where, u16 *val);

void slk_msleep(unsigned int msecs);

void slk_pci_set_master(SLK_PTR_PCI_DEV dev_pci);

unsigned long slk__get_free_page(void);

void slk_free_page(unsigned long addr);

void slk_wmb(void);

void slk_memcpy(void *dest, const void *src, unsigned int count);

bool slk_unlikely(bool counter);

bool slk_likely(bool counter);

void slk_pci_read_config_dword(SLK_PTR_PCI_DEV dev, int where, u32 *val);

void* slk_kmalloc_g(int size);

void slk_atomic_set(SLK_ATOMIC_T i_tx_hw_count,int num);

void slk_atomic_dec(SLK_ATOMIC_T i_tx_hw_count);

void slk_atomic_inc(SLK_ATOMIC_T i_tx_hw_count);

u32 slk_jhash_1word(u32 a, u32 initval);

void SLK_INIT_LIST_HEAD(struct slk_list_head* list);


void slk_list_add_rcu(struct slk_list_head* new, struct slk_list_head* head);

void slk_list_del_rcu(struct slk_list_head* entry);

int slk_list_empty(struct slk_list_head* head);

struct slk_map_elem* slk_list_entry(struct slk_list_head* next, struct slk_map_elem* stream);

void slk_list_del(struct slk_list_head* next);

int slk_get_dma_to_device(void);

void slk_free_pages(unsigned long addr, unsigned int order);

void slk_dma_unmap_single(SLK_DEVICE dev,SLK_PTR_DAM_ADDR_T handle,unsigned int size,int dir);

int slk_get_dma_from_device(void);

unsigned long slk_get_free_pages(int order);

void* slk_dma_map_single(SLK_DEVICE p_pci, void* cpu_addr, unsigned int size, int dir);

u32 slk_ntohs(uint16_t x);

void slk_list_for_each_entry(struct slk_map_elem* stream,struct slk_list_head* list);

int slk_atomic_read(SLK_ATOMIC_T dma_count);

void* get_pcie_dev_bus_self(SLK_PTR_PCI_DEV p_dev);

void slk_dma_sync_single_for_cpu(SLK_DEVICE p_pci, SLK_PTR_DAM_ADDR_T dma_handle, unsigned int size, long dir);

void slk_dma_sync_single_for_device(SLK_DEVICE p_pci, SLK_PTR_DAM_ADDR_T handle,unsigned int size, long dir);

int slk_pci_find_capability(SLK_PTR_PCI_DEV dev, int cap);

int slk_pci_find_ext_capability(SLK_PTR_PCI_DEV dev, int cap);

void slk_pci_read_config_byte(SLK_PTR_PCI_DEV dev, int where, u8 *val);

void slk_pci_write_config_byte(SLK_PTR_PCI_DEV dev, int where, u8 val);

void slk_pci_write_config_dword(SLK_PTR_PCI_DEV dev, int where, u32 val);

void slk_udelay(uint64_t loops);

void slk_msleep_interruptible(uint64_t msecs);

void slk_wake_up_interruptible(SLK_WAIT_QUEUE_HEAD_T head_t);

void slk_init_waitqueue_head(SLK_WAIT_QUEUE_HEAD_T head_t);

void slk_sema_init(SLK_SEMAPHORE sem, int val);

int slk_down_interruptible(SLK_SEMAPHORE sem);

void slk_up(SLK_SEMAPHORE sem);

SLK_MM_SEGMENT_T slk_get_fs(void);

void slk_set_fs(SLK_MM_SEGMENT_T seg);

void* slk_filp_open(const char *filename, int flags, uint16_t mode);

void* slk_get_filp_f_op(SLK_FILE filp);

int open_hd_file(const char* file_name, int flag, SLK_OP_FILES files);

long slk_vfs_read(SLK_FILE file, char* buf, unsigned int count, long long pos);

long slk_vfs_write(SLK_FILE file, char* buf, unsigned int count, long long pos);

bool SLK_IS_ERR(SLK_FILE filp);

void close_hd_file(SLK_OP_FILES hd_file);

void slk_filp_close(SLK_FILE fild, void* id);

void slk_pci_restore_state(SLK_PTR_PCI_DEV dev);

u8 slk_get_pm_cap_offset(SLK_PTR_PCI_DEV		 dev);

u16 get_slk_pci_d0(void);

void slk_pci_save_state(SLK_PTR_PCI_DEV dev_pci);

u16 get_slk_pci_d3hot(void);

SLK_PTR_PCI_DEV slk_get_dev_bus_self(SLK_PTR_PCI_DEV dev);

int get_utc_time(int* year,int* mon,int* day,int* hour,int* min,int* sec,int* usec);

int get_local_time(int* year, int* mon, int* day, int* hour, int* min, int* sec, int* usec);

uint64_t slk_get_o_trunc(void);

uint64_t slk_get_o_sync(void);

uint64_t slk_get_o_creat(void);

bool slk_kthread_should_stop(void);

void slk_wake_up_process(SLK_PTR_TASK_STRUCT td_write);

SLK_SK_BUFF slk_sl_tcp_build_send_skb_base(uint32_t sip,uint32_t dip,struct sl_tcp_params* param);

SLK_SK_BUFF slk_sl_answer_synack_skb(SLK_SK_BUFF q_skb, uint32_t new_seq, uint32_t* ack);

unsigned slk_in_aton(unsigned char *str);

void slk_sl_rcv_ack(SLK_SK_BUFF skb, uint32_t* seq, uint32_t* ack);

void slk_sl_rcv_data(SLK_SK_BUFF skb, uint32_t* seq, uint32_t* ack);

extern void slk_debug(uint8_t dir, const char* msg,int subtype);

extern void slk_warning(uint8_t dir, const char* msg, int subtype);

extern void slk_error(uint8_t dir, const char* msg, int subtype);

extern void slk_fail(uint8_t dir,const char* msg, int subtype);

extern void slk_short(uint8_t dir,const char* msg, int subtype);

extern void slk_register(unsigned int addr,unsigned value, uint8_t flag);

void slk_printk_len_msg(uint8_t dir, int len);

void slk_printk_sum(uint8_t dir, uint8_t p_print7,uint8_t p_print6, uint8_t p_print5, uint8_t p_print4,
						  uint8_t p_print3, uint8_t p_print2, uint8_t p_print1, uint8_t p_print);

void slk_printk_len(uint8_t dir);

void slk_prink_seq(const char* head, int seq, int len);

void slk_prink_p_print(const char* head, int seq, int i, uint8_t p_print, uint8_t p_print1, uint8_t p_print2, uint8_t p_print3, 
							  uint8_t p_print4, uint8_t p_print5, uint8_t p_print6, uint8_t p_print7, uint8_t p_print8,
							  uint8_t p_print9, uint8_t p_print10, uint8_t p_print11, uint8_t p_print12, uint8_t p_print13, 
							  uint8_t p_print14, uint8_t p_print15);

void slk_snprintf(char* ch_fmtn, int size, uint8_t np_print);

void slk_printk_ch(const char* head, int seq, int num, char* ch_fmt);

void slk_snprintf_s(char* ch_file_path, int size, uint32_t file_indx);

void slk_snprintf_ss(char* ch_msg, int size, char* ch_tm, const char* head, int seq, int num, char* ch_fmt);

void slk_kthread_stop(SLK_PTR_TASK_STRUCT td_write);

void slk_snpritf_msg(char* ch_msg, int size, char*ch_tm, const char* head, int seq, int len);

void slk_snprintf_p_print(char *ch_msg, int size, char* ch_tm, const char* head, int seq, int i, uint8_t p_print, uint8_t p_print1, uint8_t p_print2, uint8_t p_print3, 
							  uint8_t p_print4, uint8_t p_print5, uint8_t p_print6, uint8_t p_print7, uint8_t p_print8,
							  uint8_t p_print9, uint8_t p_print10, uint8_t p_print11, uint8_t p_print12, uint8_t p_print13, 
							  uint8_t p_print14, uint8_t p_print15);

uint32_t slk_strlen(char* ch_msg);

void slk_snprintf_sum(char* ch_tm, int size, int sys_year, int sys_mon, int sys_day,
							int sys_hour, int sys_min, int sys_sec, int sys_usec);

void slk_printk_dev_status(int proj, uint32_t status, uint32_t right_status);

void slk_printk_error_buff(char* error_buff);

SLK_PTR_TASK_STRUCT slk_kthread_create(int(*threadfn)(void* data), void* data);

unsigned long slk_copy_from_user(void *to, const void *from, unsigned long n);

unsigned long slk_copy_to_user(void *to, const void *from, unsigned long n);

void slk_print_local(const char* head,unsigned char* msg, int len, int seq);

void slk_print_default(const char* head,const char* msg, int len, int seq);

void slk_print_bin(const char* head,unsigned char* msg, int len, int seq);

void write_self_file(SLK_FILE p_file,const char* msg, int len);

void slk_error_log(const char* format, ...);

void slk_debug_log(const char* format, ...);

void slk_warning_log(const char* format, ...);

void slk_debug_log_ex(const char* format, ...);

void slk_error_log_ex(const char* format, ...);

void slk_warning_log_ex(const char* format, ...);

void slk_print_default_ex(const char* head,const char* msg, int len, int seq);

void slk_print_local_ex(const char* head,unsigned char* msg, int len, int seq);

void slk_print_bin_ex(const char* head,unsigned char* msg, int len, int seq);

int slk_get_fpga_rx_ring_len_val(void);

int slk_get_fpga_rx_dma_len(void);

int slk_get_tx_pcie_cmd_val(void);

int slk_get_tx_cmd_belength(void);

int slk_get_tx_addr_l_val(void);

int slk_get_rx_pcie_cmd(void);

int slk_get_rx_addr_u(void);

int slk_get_rx_addr_l(void);

int slk_get_tx_pcie_cmd_data1(void);

int slk_get_rx_pcie_cmd_data1(void);

void slk_create_tx_mem_op_list_s10(struct slk_tx_mem_op_list* p_lst, SLK_PTR_PCI_DEV p_dev);

void slk_destroy_tx_mem_op_list_s10(struct slk_tx_mem_op_list* p_lst,SLK_PTR_PCI_DEV p_dev);

void s10_write_dma_addr(struct slk_tx_mem_op_list* p_lst, int addr, int len);

void s10_write_dma_addr(struct slk_tx_mem_op_list* p_lst, int addr, int len);

int slk_push_rx_queue_node(void* p_chan, int node, SLK_SPINLOCK_T lcl_lock);

void slk_record_rx_chan_base(void*	p_head, int chan_count);

int slk_tmo_get_mem_tx(void);

#endif

