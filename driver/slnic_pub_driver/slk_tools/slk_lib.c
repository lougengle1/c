#include <linux/string.h>
#include <linux/jhash.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/jhash.h>
#include <linux/version.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <net/tcp.h>
#include <linux/skbuff.h>
#include <linux/atalk.h>
#include <linux/rtc.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/kernel.h> /* printk() */
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/inet.h>
#include "slk_lib.h"
#include "slk_define.h"
#include "slk_map.h"
#include "slk_tcp.h"
#include "slk_kernel.h"
#include "slk_rx_chan.h"

struct slk_file
{
	mm_segment_t		old_fs;
	struct file*		filp;
};

/// 默认日志路径
#define SLK_DEFAULT_FILE_NAME "/var/log/message_shengli"

void* slk_kmalloc(int size)
{
	return kmalloc(size,GFP_KERNEL);
}

void slk_kfree(void* memory_addr)
{
    kfree(memory_addr);
}

uint64_t slt_get_gfp_kernel(void)
{
	return GFP_KERNEL;
}

uint64_t slt_get_kernel_action(void)
{
	return (GFP_KERNEL | __GFP_COMP);
}

void* slk_dma_alloc_coherent(SLK_PTR_PCI_DEV dev,int size,SLK_PTR_DAM_ADDR_T dam_addr, uint64_t memory_antion)
{
	struct pci_dev* p_dev = dev;

    return dma_alloc_coherent(&(p_dev->dev),size,dam_addr,memory_antion);
}

void slk_dma_free_coherent(SLK_PTR_PCI_DEV dev,int len,void* vis_addr,SLK_PTR_DAM_ADDR_T dam_addr)
{
	struct pci_dev* p_dev = dev;

	dma_free_coherent(&(p_dev->dev),len,vis_addr,(dma_addr_t)dam_addr);
}

int slk_pci_set_dma_mask(SLK_PTR_PCI_DEV dev)
{
	int result;
	struct pci_dev* p_dev = dev;

	result = pci_set_dma_mask(p_dev, DMA_BIT_MASK(64));
	if (result)
    {
        return -1;
    }

	return 0;
}

int slk_pci_set_consistent_dma_mask(SLK_PTR_PCI_DEV dev)
{
	int result;
	struct pci_dev* p_dev = dev;

	result = pci_set_consistent_dma_mask(p_dev, DMA_BIT_MASK(64));
	if (result)
    {
        return -1;
    }

	return 0;
}

void slk_SetPageReserved(unsigned long page)
{
    SetPageReserved(virt_to_page(page));
}

void slk_ClearPageReserved(unsigned long page)
{
    ClearPageReserved(virt_to_page(page));
}

void slk_spin_lock_init(SLK_SPINLOCK_T rx_chan)
{
	spin_lock_init((spinlock_t*)rx_chan);
}

void slk_spin_lock(SLK_SPINLOCK_T lcl_lock)
{
    spin_lock((spinlock_t*)lcl_lock);
}

void slk_spin_unlock(SLK_SPINLOCK_T lcl_lock)
{
	spin_unlock((spinlock_t*)lcl_lock);
}

int slt_get_page_size()
{
	return PAGE_SIZE;
}

void slk_strcpy(char* info,const char* src)
{
	strcpy(info,src);
}

void slk_memset(void* buff,int num,int size)
{
	memset(buff,num,size);
}

void slk_pci_write_config_word(const SLK_PTR_PCI_DEV dev, int where,u16 val)
{
	pci_write_config_word((struct pci_dev*)dev,where,val);
}

void slk_pci_read_config_word(const SLK_PTR_PCI_DEV dev, int where, u16 *val)
{
	pci_read_config_word((struct pci_dev*)dev, where, val);
}

void slk_msleep(unsigned int msecs)
{
	msleep(msecs);
}

void slk_pci_set_master(SLK_PTR_PCI_DEV dev_pci)
{
	pci_set_master((struct pci_dev*)dev_pci);
}

unsigned long  slk__get_free_page()
{
	return __get_free_page(GFP_KERNEL);
}

void slk_free_page(unsigned long addr)
{
	free_page(addr);
}

void slk_memcpy(void *dest, const void *src, unsigned int count)
{
	memcpy(dest,src,count);
}

void slk_wmb()
{
	wmb();
}

bool slk_unlikely(bool counter)
{
	return unlikely(counter);
}

bool slk_likely(bool counter)
{
	return likely(counter);
}


void* slk_kmalloc_g(int size)
{
	return kmalloc(size,GFP_ATOMIC);
}

void slk_atomic_set(SLK_ATOMIC_T i_tx_hw_count,int num)
{
	atomic_set(i_tx_hw_count,num);
}

void slk_atomic_dec(SLK_ATOMIC_T i_tx_hw_count)
{
	atomic_dec(i_tx_hw_count);
}

void slk_atomic_inc(SLK_ATOMIC_T i_tx_hw_count)
{
	atomic_inc(i_tx_hw_count);
}

u32 slk_jhash_1word(u32 a, u32 initval)
{
	return jhash_1word(a, initval);
}

void SLK_INIT_LIST_HEAD(struct slk_list_head* list)
{
	INIT_LIST_HEAD((struct list_head*)list);
}

void slk_list_for_each_entry(struct slk_map_elem* stream, struct slk_list_head* list)
{
	list_for_each_entry(stream, list, list);
}

void slk_list_add_rcu(struct slk_list_head* new, struct slk_list_head* head)
{
	list_add_rcu((struct list_head*)new, (struct list_head*)head);
}

void slk_list_del_rcu(struct slk_list_head* entry)
{
	list_del_rcu((struct list_head*)entry);
}

int slk_list_empty(struct slk_list_head* head)
{
	return list_empty((struct list_head*)head);
}

struct slk_map_elem* slk_list_entry(struct slk_list_head* next, struct slk_map_elem* stream)
{
	return list_entry(next, typeof(*stream), list);
}

void slk_list_del(struct slk_list_head* next)
{
	list_del((struct list_head*)next);
}

void slk_free_pages(unsigned long addr, unsigned int order)
{
	free_pages(addr,order);
}

void slk_dma_unmap_single(SLK_DEVICE dev,void* handle,unsigned int size,int dir)
{
	dma_unmap_single((struct device*)dev,(dma_addr_t)handle,size,dir);
}

int slk_get_dma_from_device()
{
	return DMA_FROM_DEVICE;
}

int slk_get_dma_to_device()
{
	return DMA_TO_DEVICE;
}

unsigned long slk_get_free_pages(int order)
{
	return __get_free_pages(GFP_KERNEL,order);
}

void* slk_dma_map_single(SLK_DEVICE p_pci, void* cpu_addr, unsigned int size, int dir)
{
	return (void*)dma_map_single((struct device*)p_pci,cpu_addr,size,dir);
}

u32 slk_ntohs(uint16_t x)
{
	return ntohs(x);
}

int slk_atomic_read(SLK_ATOMIC_T dma_count)
{
	return atomic_read(dma_count) ;
}

void slk_dma_sync_single_for_cpu(SLK_DEVICE p_pci, SLK_PTR_DAM_ADDR_T dma_handle, unsigned int size, long dir)
{
	dma_sync_single_for_cpu(p_pci, (dma_addr_t)dma_handle, size, dir);
}

void slk_dma_sync_single_for_device(SLK_DEVICE p_pci, SLK_PTR_DAM_ADDR_T handle,unsigned int size, long dir)
{
	dma_sync_single_for_cpu(p_pci, (dma_addr_t)handle, size, dir);
}

void* get_pcie_dev_bus_self(SLK_PTR_PCI_DEV p_dev)
{
	struct pci_dev* pcie_dev = p_dev;

	return pcie_dev->bus->self;

}

int slk_pci_find_capability(SLK_PTR_PCI_DEV dev, int cap)
{
	return pci_find_capability((struct pci_dev*)dev, cap);
}

int slk_pci_find_ext_capability(SLK_PTR_PCI_DEV dev, int cap)
{
	return pci_find_ext_capability((struct pci_dev*)dev, cap);
}

void slk_pci_read_config_byte(SLK_PTR_PCI_DEV dev, int where, u8 *val)
{
	pci_read_config_byte((struct pci_dev*)dev, where, val);
}

void slk_pci_read_config_dword(SLK_PTR_PCI_DEV dev, int where, u32 *val)
{
	pci_read_config_dword((struct pci_dev*)dev, where, val);
}

void slk_pci_write_config_byte(SLK_PTR_PCI_DEV dev, int where, u8 val)
{
	pci_write_config_byte((struct pci_dev*)dev, where, val);
}

void slk_pci_write_config_dword(SLK_PTR_PCI_DEV dev, int where, u32 val)
{
	pci_write_config_dword((struct pci_dev *)dev, where, val);
}

void slk_udelay(uint64_t loops)
{
	udelay(loops);
}

void slk_msleep_interruptible(uint64_t msecs)
{
	msleep_interruptible(msecs);
}

void slk_wake_up_interruptible(SLK_WAIT_QUEUE_HEAD_T head_t)
{
	wake_up_interruptible((wait_queue_head_t*)head_t);
}

void slk_init_waitqueue_head(SLK_WAIT_QUEUE_HEAD_T head_t)
{
	init_waitqueue_head((wait_queue_head_t*)head_t);
}

void slk_sema_init(SLK_SEMAPHORE sem, int val)
{
	sema_init((struct semaphore*)sem, 1);
}

int slk_down_interruptible(SLK_SEMAPHORE sem)
{
	return down_interruptible((struct semaphore*)sem);
}

void slk_up(SLK_SEMAPHORE sem)
{
	up((struct semaphore*)sem);
}

void* slk_filp_open(const char *filename, int flags, uint16_t mode)
{
	return filp_open(filename, flags, mode);
}

bool SLK_IS_ERR(SLK_FILE filp)
{
	return IS_ERR(filp);
}

int open_hd_file(const char* file_name, int flag, SLK_OP_FILES files)
{
	struct file* filp=0;
	struct slk_file* hd_file = files;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	hd_file->old_fs = get_fs();
	set_fs (KERNEL_DS);
#else
	hd_file->old_fs = force_uaccess_begin();
#endif

	filp = filp_open(file_name,flag, 0);

	if (IS_ERR(filp) || !(filp->f_op) )
	{	
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
		set_fs (hd_file->old_fs);
#else
		force_uaccess_end(hd_file->old_fs);
#endif

		return PTR_ERR(filp);
	}

	hd_file->filp = filp;
	return 0;
}

long slk_vfs_read(SLK_FILE filp, char* buf, unsigned int count, long long pos)
{
	struct slk_file* files = filp;

	if (!(files->filp))
	{
		return -1;
	}

	return SLK_KERNEL_READ(files->filp, pos, buf, count);
}

long slk_vfs_write(SLK_FILE filp, char* buf, unsigned int count, long long pos)
{
	struct slk_file* files = filp;
	
	if (!(files->filp))
		{
			return -1;
		}

	return SLK_KERNEL_WRITE(files->filp, buf, count, pos);
}

void close_hd_file(SLK_OP_FILES hd_file)
{
	struct slk_file* files = hd_file;

	filp_close(files->filp,0);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	set_fs (files->old_fs);
#else
	force_uaccess_end(files->old_fs);
#endif
}

void slk_filp_close(SLK_FILE fild, void* id)
{
	filp_close((struct file*)fild, id);
}

void slk_pci_restore_state(SLK_PTR_PCI_DEV dev)
{
	pci_restore_state((struct pci_dev*)dev);
}

u8 slk_get_pm_cap_offset(SLK_PTR_PCI_DEV		 dev)
{
	struct pci_dev* dev_pci= dev;
	
	return dev_pci->pm_cap + PCI_PM_CTRL;
}

u16 get_slk_pci_d0()
{
	return PCI_D0;
}

void slk_pci_save_state(SLK_PTR_PCI_DEV dev_pci)
{
	pci_save_state((struct pci_dev*)dev_pci);
}

u16 get_slk_pci_d3hot()
{
	return PCI_D3hot;
}

SLK_PTR_PCI_DEV slk_get_dev_bus_self(SLK_PTR_PCI_DEV dev)
{
	struct pci_dev* dev_pci= dev;
	
	return dev_pci->bus->self;
}

int get_utc_time(int* year,int* mon,int* day,int* hour,int* min,int* sec,int* usec)
{
	struct STRUCT_TIMEX		txc;
	struct rtc_time			tm;

	GET_KERNEL_TIME(&(txc.time));
	RTC_TIME_TO_TM(txc.time.tv_sec,&tm);

	(*year) = tm.tm_year + 1900;
	(*mon)  = tm.tm_mon + 1;
	(*day)  = tm.tm_mday;
	(*hour) = tm.tm_hour;
	(*min)  = tm.tm_min;
	(*sec)  = tm.tm_sec;
	(*usec) = txc.time.tv_usec;

	return 0;
}

int get_local_time(int* year, int* mon, int* day, int* hour, int* min, int* sec, int* usec)
{
	struct STRUCT_TIMEX		txc;
	struct rtc_time			tm;

	GET_KERNEL_TIME(&(txc.time));
	txc.time.tv_sec -= sys_tz.tz_minuteswest * 60;
	RTC_TIME_TO_TM(txc.time.tv_sec, &tm);

	(*year) = tm.tm_year + 1900;
	(*mon) = tm.tm_mon + 1;
	(*day) = tm.tm_mday;
	(*hour) = tm.tm_hour;
	(*min) = tm.tm_min;
	(*sec) = tm.tm_sec;
	(*usec) = txc.time.tv_usec;

	return 0;
}

uint64_t slk_get_o_trunc()
{
	return O_TRUNC;
}

uint64_t slk_get_o_sync()
{
	return O_SYNC;
}

uint64_t slk_get_o_creat()
{
	return O_CREAT;
}

bool slk_kthread_should_stop()
{
	return kthread_should_stop();
}

void slk_wake_up_process(SLK_PTR_TASK_STRUCT td_write)
{
	wake_up_process((struct task_struct*)td_write);
}

u16 checksum(u16 *buffer, int size)  
{  
	unsigned long cksum=0;  
	while(size >1)  
	{  
		cksum+=*buffer++;  
		size -=sizeof(u16);  
	}  
	if(size)  
	{  
		cksum += *(u8*)buffer;  
	}  

	cksum = (cksum >> 16) + (cksum & 0xffff);  
	cksum += (cksum >>16);  
	return (u16)(~cksum);  
} 

//#if 0
SLK_SK_BUFF slk_sl_tcp_build_send_skb_base(uint32_t sip,uint32_t dip,struct sl_tcp_params* param)
{
	struct iphdr*	ih;
	struct tcphdr*	tcph; 
	u16				data_len;
	uint8_t			ch_check_buff[1024];
	struct psd_head		ptcp_psd_header;
	struct sk_buff*	skb;
	int				i_count;

	skb = dev_alloc_skb(sizeof(struct iphdr) + sizeof(struct tcphdr) +param->len + 2);
	if (!skb)
	{
		printk(KERN_ALERT "alloc_skb error");
		return 0;
	}
	/// 移动data、tail向后2字节
	skb_reserve(skb, 2); /* align IP on 16B boundary */  
	/// 获得ip头
	ih = (struct iphdr *)(skb->data);
	/// 赋值ip
	ih->saddr = sip;
	ih->daddr = dip;
	ih->ihl   = 5;
	ih->id	  = 0;
	ih->version = 4;
	ih->tos = 0;
	ih->frag_off = htons(0x4000);
	ih->ttl = 64;
	ih->protocol = IPPROTO_TCP;
	ih->tot_len = htons(param->len + sizeof(struct iphdr) + sizeof(struct tcphdr));
	/// ip check
	ih->check = 0;         /* and rebuild the checksum (ip needs it) */
	ih->check = ip_fast_csum((unsigned char *)ih,ih->ihl);
	/// tcp处理
	tcph = (struct tcphdr *)(ih+1);
	//tcph->doff	 = 0x5;			// 头部长度
	tcph->res1   = 0;			// 保留
	tcph->cwr	 = 0;
	tcph->ece	 = 0;
	tcph->doff	 = 5;			// tcp头长度（32位）
	tcph->fin    = 0;			// 断开连接
	tcph->syn    = 0;			// 连接请求
	tcph->rst    = 0;			// 重连请求
	if(param->len)
		tcph->psh= 1;			// 接收方收到数据后,立即交给上一层程序 
	else
		tcph->psh= 0;
	tcph->ack    = 1;			// 确认序号
	tcph->urg    = 0;			// 紧急指针标记
	tcph->window = htons(0x16d0);
	tcph->urg_ptr= 0;			// 紧急指针
	/// 端口
	tcph->source = htons(param->sport);
	tcph->dest	 = htons(param->dport);
	/// 初始序号ISN
	tcph->seq    = htonl(param->seq);	
	/// 确认序号(对端ISN+1)
	tcph->ack_seq = htonl(param->ack);
	/// 复制数据
	if(param->len)
		memcpy(tcph+1,param->data,param->len);
	///  校验和
	data_len = 0;
	data_len = (data_len | ih->ihl)*4;
	data_len = ntohs(ih->tot_len)- data_len;
	memset(ch_check_buff,0,1024);
	//填充伪TCP头
	ptcp_psd_header.sourceip= ih->daddr;
	ptcp_psd_header.destip  = ih->saddr;
	ptcp_psd_header.mbz=0;
	ptcp_psd_header.ptcl=IPPROTO_TCP;
	ptcp_psd_header.plen=htons(data_len);
	//计算TCP校验和
	tcph->check=0;
	memcpy(ch_check_buff, &ptcp_psd_header, sizeof(struct psd_head));
	memcpy((unsigned char *)ch_check_buff+sizeof(struct psd_head),
		(unsigned char *)tcph,data_len);
	tcph->check=checksum((unsigned short *)ch_check_buff, sizeof(struct psd_head)+data_len);

	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb_reset_network_header(skb);
	skb->dev = (struct net_device*)param->dev;
	skb->transport_header = skb->network_header + sizeof(struct iphdr);
	skb->protocol = 0x8;

	skb_put(skb, sizeof(struct iphdr) + sizeof(struct tcphdr) + param->len);

	printk(KERN_ALERT "send seq: %08x, ack:%08x",tcph->seq,tcph->ack_seq);

	for(i_count = 0;i_count < ntohs(ih->tot_len)/4; i_count++)
	{
		printk(KERN_ALERT "data: %08x", *(uint32_t*)&skb->data[i_count*4]);
	}

	return skb;
}

SLK_SK_BUFF slk_sl_answer_synack_skb(SLK_SK_BUFF q_skb, uint32_t new_seq, uint32_t* ack)
{
	struct sk_buff* seq_skb = (struct sk_buff*)q_skb;

	struct iphdr*	ih;
	struct tcphdr*	tcph; 
	u32				addr;
	u16				port;
	struct sk_buff *skb;
	u16				data_len;
	uint8_t			ch_check_buff[1024];
	struct psd_head		ptcp_psd_header;
	int				hd_shake_pkt;
	//struct fpga_priv *	 priv = netdev_priv(fpga_net_dev);


	if (seq_skb->len < sizeof(struct iphdr))
	{
		printk("snull: Hmm... packet too short (%i octets)",seq_skb->len);
		return 0;
	}

	ih = (struct iphdr *)(seq_skb->data);
	if(ih->protocol != IPPROTO_TCP)
	{
		printk(KERN_ALERT "the pkt is not TCP: %d", ih->protocol);
		return 0;
	}
	tcph = (struct tcphdr *)(ih+1);
	hd_shake_pkt = (tcph->syn == 0x1 && tcph->ack == 0);
	if(!hd_shake_pkt)
	{
		printk(KERN_ALERT "the pkt is not connet pkt");
		return 0;
	}

	skb = skb_clone(seq_skb, GFP_ATOMIC);
	if(!skb)
	{
		printk(KERN_ALERT "clone skb error");
		return 0;
	}

	/// 重组握手包
	/// 获取ip地址
	ih = (struct iphdr *)(skb->data);

	/// 调换ip
	addr	  = ih->saddr;
	ih->saddr = ih->daddr;
	ih->daddr = addr;

	/// ip check
	ih->check = 0;         /* and rebuild the checksum (ip needs it) */
	ih->check = ip_fast_csum((unsigned char *)ih,ih->ihl);

	/// tcp处理
	tcph = (struct tcphdr *)(ih+1);
	/// 调换端口
	port		 = tcph->source;
	tcph->source = tcph->dest;
	tcph->dest	 = port;
	/// 确认序号(对端ISN+1)
	//priv->seq = new_seq;
	//priv->ack = htonl(ntohl(tcph->seq) + 0x1);
	*ack = ntohl(tcph->seq) + 0x1;
	/// 初始序号ISN
	tcph->seq    =  htonl(new_seq);
	tcph->ack_seq = htonl(*ack);

	/// 标志位，ACK=1, SYN=1
	tcph->syn = 1;
	tcph->ack = 1;
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	///  校验和
	data_len = 0;
	data_len = (data_len | ih->ihl)*4;
	data_len = ntohs(ih->tot_len)- data_len;
	memset(ch_check_buff,0,1024);
	//填充伪TCP头
	ptcp_psd_header.destip  = ih->saddr;
	ptcp_psd_header.sourceip=ih->daddr;
	ptcp_psd_header.mbz=0;
	ptcp_psd_header.ptcl=IPPROTO_TCP;
	ptcp_psd_header.plen=htons(data_len);

	//计算TCP校验和
	tcph->check=0;
	memcpy(ch_check_buff, &ptcp_psd_header, sizeof(struct psd_head));
	memcpy((unsigned char *)ch_check_buff+sizeof(struct psd_head),
		(unsigned char *)tcph,data_len);
	tcph->check=checksum((unsigned short *)ch_check_buff, sizeof(struct psd_head)+data_len);

	printk(KERN_ALERT "answer seq: %08x, ack:%08x",tcph->seq,tcph->ack_seq);

	return skb;
}
// #endif
unsigned slk_in_aton(unsigned char *str)
{
	return in_aton(str);
}

void slk_sl_rcv_ack(SLK_SK_BUFF skb, uint32_t* seq, uint32_t* ack)
{
	struct sk_buff* ack_skb = (struct sk_buff*)skb;

	struct tcphdr*	tcph;
	tcph = (struct tcphdr *)(ack_skb->data + sizeof(struct iphdr));

	*seq = ntohl(tcph->ack_seq);
	//*ack = ntohl(tcph->seq);
}

void slk_sl_rcv_data(SLK_SK_BUFF skb, uint32_t* seq, uint32_t* ack)
{
	struct sk_buff* data_skb = (struct sk_buff*)skb;
	struct iphdr*	ih;
	struct tcphdr*	tcph;
	ih	 = (struct iphdr*)(data_skb->data);
	tcph = (struct tcphdr *)(data_skb->data + sizeof(struct iphdr));

	//is_32_over(ntohl(tcph->ack_seq),)
	//这里有可能用户在数据中带有ack
	*seq = ntohl(tcph->ack_seq);
	*ack = ntohl(tcph->seq) + ntohs(ih->tot_len)- (ih->ihl | 0x0)*4 - (tcph->doff | 0x0)*4;
	printk(KERN_ALERT "old seq:%08x, old ack:%08x, seq:%08x, ack:%08x",tcph->seq,tcph->ack_seq,*seq,*ack);
}

void slk_debug( uint8_t dir, const char* msg,int subtype/*=-1*/ )
{
	printk(KERN_ALERT "[" MODULE_NAME "][D][%c][] %s",dir,msg);
}

void slk_warning( uint8_t dir, const char* msg, int subtype/*=-1*/ )
{
	printk(KERN_ALERT "[" MODULE_NAME "][W][%c][] %s",dir,msg);
}

void slk_error( uint8_t dir, const char* msg, int subtype/*=-1*/ )
{
	printk(KERN_ALERT "[" MODULE_NAME "][E][%c][] %s",dir,msg);
}

void slk_fail( uint8_t dir,const char* msg, int subtype/*=-1*/ )
{
	printk(KERN_ALERT "[" MODULE_NAME "][F][%c][] %s",dir,msg);
}

void slk_short( uint8_t dir,const char* msg, int subtype/*=-1*/ )
{
	printk(KERN_ALERT "[" MODULE_NAME "][S][%c][%d] %s",dir,subtype,msg);
}

void slk_register( unsigned int addr,unsigned value, uint8_t flag)
{
	printk(KERN_ALERT "[" MODULE_NAME "][R][][%c] add:%08x val:%08x",flag,addr,value);
}

void slk_printk_len_msg(uint8_t dir, int len)
{
	printk(KERN_ALERT "[" MODULE_NAME "][B][%c][H] %d",dir,len);
}


void slk_printk_sum(uint8_t dir, uint8_t p_print7,uint8_t p_print6, uint8_t p_print5, uint8_t p_print4,
						  uint8_t p_print3, uint8_t p_print2, uint8_t p_print1, uint8_t p_print)
{
	printk(KERN_ALERT "[" MODULE_NAME "][B][%c][X] %02x%02x%02x%02x %02x%02x%02x%02x\n",
			 dir,
			(p_print7),
			(p_print6),
			(p_print5),
			(p_print4),
			(p_print3),
			(p_print2),
			(p_print1),
			(p_print));
}

void slk_printk_len(uint8_t dir)
{
	printk(KERN_ALERT "[" MODULE_NAME "][B][%c][T]",dir);
}

void slk_prink_seq(const char* head, int seq, int len)
{
	printk(KERN_ALERT "[" MODULE_NAME "][data][%s][%08d][%08d]\n", head, seq, len);
}

void slk_prink_p_print(const char* head, int seq, int i, uint8_t p_print, uint8_t p_print1, uint8_t p_print2, uint8_t p_print3, 
							  uint8_t p_print4, uint8_t p_print5, uint8_t p_print6, uint8_t p_print7, uint8_t p_print8,
							  uint8_t p_print9, uint8_t p_print10, uint8_t p_print11, uint8_t p_print12, uint8_t p_print13, 
							  uint8_t p_print14, uint8_t p_print15)
{
	printk( KERN_ALERT "[" MODULE_NAME "][data][%s][%08d][%04d] " SLK_LOG_LINE_FMT "\n", 
			head,
			seq,
			i + 1,
			(p_print),
			(p_print1),
			(p_print2),
			(p_print3),
			(p_print4),
			(p_print5),
			(p_print6),
			(p_print7),
			(p_print8),
			(p_print9),
			(p_print10),
			(p_print11),
			(p_print12),
			(p_print13),
			(p_print14),
			(p_print15));
}

void slk_printk_dev_status(int proj, uint32_t status, uint32_t right_status)
{
	printk("\nproj:%u;status:0x%08x;right_status:0x%08x", proj, status, right_status);
}

void slk_printk_error_buff(char* error_buff)
{
	printk("%s", error_buff);
}


void slk_snprintf(char* ch_fmtn, int size, uint8_t np_print)
{
	snprintf(ch_fmtn, size, "%02x ", np_print );
}

void slk_printk_ch(const char* head, int seq, int num, char* ch_fmt)
{
	printk( KERN_ALERT "[" MODULE_NAME "][data][%s][%08d][%04d] %s\n",head, seq, num, ch_fmt);
}

void slk_snprintf_s(char* ch_file_path, int size, uint32_t file_indx)
{	
	snprintf(ch_file_path, size, "%s_%u", SLK_DEFAULT_FILE_NAME, file_indx);
}

void slk_snprintf_ss(char* ch_msg, int size, char* ch_tm, const char* head, int seq, int num, char* ch_fmt)
{
	snprintf(ch_msg,size, "[%s][" MODULE_NAME "][data][%s][%08d][%04d] %s\n", ch_tm, head, seq, num, ch_fmt);
}

void slk_kthread_stop(SLK_PTR_TASK_STRUCT td_write)
{
	kthread_stop((struct task_struct*)td_write);
}

void slk_snpritf_msg(char* ch_msg, int size, char* ch_tm, const char* head, int seq, int len)
{
	snprintf(ch_msg, size, "[%s][" MODULE_NAME "][data][%s][%08d][%08d]\n", ch_tm, head, seq, len);
}

void slk_snprintf_p_print(char *ch_msg, int size, char* ch_tm, const char* head, int seq, int i, uint8_t p_print, uint8_t p_print1, uint8_t p_print2, uint8_t p_print3, 
							  uint8_t p_print4, uint8_t p_print5, uint8_t p_print6, uint8_t p_print7, uint8_t p_print8,
							  uint8_t p_print9, uint8_t p_print10, uint8_t p_print11, uint8_t p_print12, uint8_t p_print13, 
							  uint8_t p_print14, uint8_t p_print15)
{
	snprintf( ch_msg,sizeof(ch_msg), "[%s][" MODULE_NAME "][data][%s][%08d][%04d] " SLK_LOG_LINE_FMT "\n",
			ch_tm,
			head,
			seq,
			i + 1,
			(p_print),
			(p_print+1),
			(p_print+2),
			(p_print+3),
			(p_print+4),
			(p_print+5),
			(p_print+6),
			(p_print+7),
			(p_print+8),
			(p_print+9),
			(p_print+10),
			(p_print+11),
			(p_print+12),
			(p_print+13),
			(p_print+14),
			(p_print+15));
}

uint32_t slk_strlen(char* ch_msg)
{
	return strlen(ch_msg);
}

void slk_snprintf_sum(char* ch_tm, int size, int sys_year, int sys_mon, int sys_day,
							int sys_hour, int sys_min, int sys_sec, int sys_usec)
{
	snprintf(ch_tm, size, SLK_LOG_TIME_FMT, sys_year, sys_mon, sys_day, sys_hour, sys_min, sys_sec, sys_usec);
}

SLK_PTR_TASK_STRUCT slk_kthread_create(int(*threadfn)(void* data), void* data)
{
	return kthread_create(threadfn, data, "%s", "default_log_wt_task");
}

unsigned long slk_copy_from_user(void *to, const void *from, unsigned long n)
{
	return copy_from_user(to,from,n);
}

unsigned long slk_copy_to_user(void *to, const void *from, unsigned long n)
{
	return copy_to_user(to, from, n);
}

void write_self_file(SLK_FILE file,const char* msg, int len)
{
	struct file* p_file = file;
	mm_segment_t			old_fs;

	if( p_file == 0 ||  IS_ERR(p_file) )
	{
		sl_error("slk_write_self_file log file open error\n");
		return;
	}	

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	old_fs = get_fs();
	set_fs(KERNEL_DS);
#else
	old_fs = force_uaccess_begin();
#endif
	
	SLK_KERNEL_WRITE(p_file, msg, len, &p_file->f_pos);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	set_fs(old_fs);
#else
	force_uaccess_end(old_fs);
#endif
}

void slk_error_log(const char* format, ...)
{
#if SLK_ERROR_LOG
	va_list arg;
	char printk_buf[1024]={0};

	va_start(arg, format);
	vsnprintf(printk_buf, 1024,format, arg);
	va_end(arg);
	printk(KERN_ALERT "["MODULE_NAME"][error]%s\n", printk_buf);
#endif
}

void slk_error_log_ex(const char* format, ...)
{
#if SLK_ERROR_LOG
	va_list arg;
	char printk_buf[1024]={0};

	va_start(arg, format);
	vsnprintf(printk_buf, 1024,format, arg);
	va_end(arg);
	printk(KERN_ALERT "["MODULE_NAME"][error]%s\n", printk_buf);
#endif
}

void slk_debug_log(const char* format, ...)
{
#if SLK_DEBUG_LOG
	va_list arg;
	char printk_buf[1024]={0};

	va_start(arg, format);
	vsnprintf(printk_buf, 1024,format, arg);
	va_end(arg);
	printk(KERN_ALERT "["MODULE_NAME"][debug]%s\n", printk_buf);
#endif
}

void slk_debug_log_ex(const char* format, ...)
{
#if SLK_DEBUG_LOG
	va_list arg;
	char printk_buf[1024]={0};

	va_start(arg, format);
	vsnprintf(printk_buf, 1024,format, arg);
	va_end(arg);
	printk(KERN_ALERT "["MODULE_NAME"][debug]%s\n", printk_buf);
#endif

}

void slk_warning_log(const char* format, ...)
{
#if SLK_WARNING_LOG
	va_list arg;
	char printk_buf[1024]={0};

	va_start(arg, format);
	vsnprintf(printk_buf, 1024,format, arg);
	va_end(arg);
	printk(KERN_ALERT "["MODULE_NAME"][warning]%s\n", printk_buf);
#endif
}

void slk_warning_log_ex(const char* format, ...)
{
#if SLK_WARNING_LOG
	va_list arg;
	char printk_buf[1024]={0};
	
	va_start(arg, format);
	vsnprintf(printk_buf, 1024,format, arg);
	va_end(arg);
	printk(KERN_ALERT "["MODULE_NAME"][warning]%s\n", printk_buf);
#endif
}

void slk_print_default_ex(const char* head,const char* msg, int len, int seq)
{
#if	SLK_LOG_DATA
	slk_print_default(head,msg,len,seq);
#endif
}

void slk_print_local_ex(const char* head,unsigned char* msg, int len, int seq)
{
#if	SLK_LOG_LACAL
	slk_print_local(head,msg,len,seq);
#endif
}

void slk_print_session_data_ex(const char* head,unsigned char* msg, int len, int seq)
{
#if	SLK_LOG_SESSION
	slk_print_default(head,msg,len,seq);
#endif
}

void slk_print_bin_ex(const char* head,unsigned char* msg, int len, int seq)
{
#if	SLK_LOG_DATA
	slk_print_bin(head,msg,len,seq);
#endif
}

void slk_create_tx_mem_op_list_s10(struct slk_tx_mem_op_list* p_lst, SLK_PTR_PCI_DEV p_dev)
{
#if SHENGLI_S10_ONLY
	uint64_t slk_memory_action = slt_get_gfp_kernel();
	
	p_lst->m_tx_dma_count.m_vis_addr = slk_dma_alloc_coherent(p_dev,sizeof(int),&(p_lst->m_tx_dma_count.m_dam_addr),slk_memory_action);
	if( !(p_lst->m_tx_dma_count.m_vis_addr) )
	{
		sl_error("alloc tx dma count error");
		return 0;
	}
	p_lst->m_tx_dma_count.m_i_len = sizeof(int);

	WRITE_REG(p_lst->m_p_bar0_base,SLK_TX_COUNT_ADDR,p_lst->m_tx_dma_count.m_dam_addr);

	sl_debug("[slk_create_tx_mem_op_list] write tx mem counter register,reg addr:%08x, value:%08x", SLK_TX_COUNT_ADDR, p_lst->m_tx_dma_count.m_dam_addr);
#endif
}

void slk_destroy_tx_mem_op_list_s10(struct slk_tx_mem_op_list* p_lst,SLK_PTR_PCI_DEV p_dev)
{
#if SHENGLI_S10_ONLY
	slk_dma_free_coherent(p_dev,p_lst->m_tx_dma_count.m_i_len,p_lst->m_tx_dma_count.m_vis_addr,p_lst->m_tx_dma_count.m_dam_addr);
#endif
}

void s10_write_dma_addr(struct slk_tx_mem_op_list* p_lst, int addr, int len)
{
#if !(SHENGLI_S10_ONLY)
	WRITE_REG(p_lst->m_p_bar1_base, addr, len);
#endif
}

int slk_push_rx_queue_node(void* p, int node, SLK_SPINLOCK_T lcl_lock)
{
	const int						i_base = 23;
	unsigned int					i_reg  = 0x01;
	struct slk_rx_channel*			p_chan = p;

	if( node >= SLK_RX_QUE_MAX_COUNT )
	{
		return -1;
	}

#if SHENGLI_S10_ONLY
	if( node >= SLK_RX_QUE_MAX_COUNT )
		{
			return -1;
		}
	
		i_reg = SLK_RX_QUE_BASE_VALUE | (i_reg << (i_base - p_chan->m_i_chan_id));
	
		slk_spin_lock(lcl_lock);
	
		WRITE_REG( p_chan->m_bar0_addr,p_chan->m_p_chanbase->m_i_rx_l_reg_addr,FPGA_LODWORD(p_chan->m_dma_queue.m_dam_addr + (p_chan->m_p_chanbase->m_i_que_node_size * node)) );
		sl_debug("write low dma addr reg:%08x,channel_id:%08x,value:%08x",p_chan->m_p_chanbase->m_i_rx_l_reg_addr,p_chan->m_i_chan_id,p_chan->m_dma_queue.m_dam_addr + (p_chan->m_p_chanbase->m_i_que_node_size * node));
	
		slk_spin_unlock(lcl_lock);
#else
/// 单通道模式
	if( p_chan->m_p_chanbase->m_i_rx_l_reg_addr == SLK_RX_OLD_LADDR_REGISTER0 )
	{
		slk_spin_lock(lcl_lock);

		WRITE_REG( p_chan->m_bar0_addr,p_chan->m_p_chanbase->m_i_rx_l_reg_addr,FPGA_LODWORD((u64)p_chan->m_dma_queue.m_dam_addr + (p_chan->m_p_chanbase->m_i_que_node_size * node)) );
		sl_debug("write low dma addr reg:%08x,channel_id:%08x,value:%08x",p_chan->m_p_chanbase->m_i_rx_l_reg_addr,p_chan->m_i_chan_id,p_chan->m_dma_queue.m_dam_addr + (p_chan->m_p_chanbase->m_i_que_node_size * node));

		WRITE_REG( p_chan->m_bar0_addr,SLK_TRX_QUE_DONE_REG_ADDR,SLK_RX_OLD_QUE_DONE_DATE );
		sl_debug("write enable dma addr reg:%08x,channel_id:%08x,value:%08x",SLK_TRX_QUE_DONE_REG_ADDR,p_chan->m_i_chan_id,SLK_RX_OLD_QUE_DONE_DATE);

		slk_spin_unlock(lcl_lock);
	}
	else
	{
		i_reg = SLK_RX_QUE_BASE_VALUE | (i_reg << (i_base - p_chan->m_i_chan_id));

		slk_spin_lock(lcl_lock);

		WRITE_REG( p_chan->m_bar0_addr,p_chan->m_p_chanbase->m_i_rx_l_reg_addr,FPGA_LODWORD((u64)p_chan->m_dma_queue.m_dam_addr + (p_chan->m_p_chanbase->m_i_que_node_size * node)) );
		sl_debug("write low dma addr reg:%08x,channel_id:%08x,value:%08x",p_chan->m_p_chanbase->m_i_rx_l_reg_addr,p_chan->m_i_chan_id,p_chan->m_dma_queue.m_dam_addr + (p_chan->m_p_chanbase->m_i_que_node_size * node));

		WRITE_REG( p_chan->m_bar0_addr,SLK_TRX_QUE_DONE_REG_ADDR,i_reg );
		sl_debug("write enable dma addr reg:%08x,channel_id:%08x,value:%08x",SLK_TRX_QUE_DONE_REG_ADDR,p_chan->m_i_chan_id,i_reg);

		slk_spin_unlock(lcl_lock);
	}
#endif

	return 0;
}

void slk_record_rx_chan_base(void *p, int chan_count)
{
	struct slk_rx_mmap_head*	p_head = p;

	if( chan_count >= 1)		p_head->m_rx_chan_base[0].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER0;
	if( chan_count >= 2)		p_head->m_rx_chan_base[1].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER1;
	if( chan_count >= 3)		p_head->m_rx_chan_base[2].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER2;
	if( chan_count >= 4)		p_head->m_rx_chan_base[3].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER3;
	if( chan_count >= 5)		p_head->m_rx_chan_base[4].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER4;
	if( chan_count >= 6)		p_head->m_rx_chan_base[5].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER5;
	if( chan_count >= 7)		p_head->m_rx_chan_base[6].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER6;
	if( chan_count >= 8)		p_head->m_rx_chan_base[7].m_i_rx_l_reg_addr = SLK_RX_NEW_LADDR_REGISTER7;

	if( chan_count >= 1)		p_head->m_rx_chan_base[0].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER0;
	if( chan_count >= 2)		p_head->m_rx_chan_base[1].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER1;
	if( chan_count >= 3)		p_head->m_rx_chan_base[2].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER2;
	if( chan_count >= 4)		p_head->m_rx_chan_base[3].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER3;
	if( chan_count >= 5)		p_head->m_rx_chan_base[4].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER4;
	if( chan_count >= 6)		p_head->m_rx_chan_base[5].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER5;
	if( chan_count >= 7)		p_head->m_rx_chan_base[6].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER6;
	if( chan_count >= 8)		p_head->m_rx_chan_base[7].m_i_rx_reg_count	= SLK_RX_NEW_REG_COUNTER7;

#if SHENGLI_S10_ONLY
		if (chan_count >= 1)		p_head->m_i_rx_reg_recove[0] = SLK_RX_NEW_REG_READ0;
		if (chan_count >= 2)		p_head->m_i_rx_reg_recove[1] = SLK_RX_NEW_REG_READ1;
		if (chan_count >= 3)		p_head->m_i_rx_reg_recove[2] = SLK_RX_NEW_REG_READ2;
		if (chan_count >= 4)		p_head->m_i_rx_reg_recove[3] = SLK_RX_NEW_REG_READ3;
		if (chan_count >= 5)		p_head->m_i_rx_reg_recove[4] = SLK_RX_NEW_REG_READ4;
		if (chan_count >= 6)		p_head->m_i_rx_reg_recove[5] = SLK_RX_NEW_REG_READ5;
		if (chan_count >= 7)		p_head->m_i_rx_reg_recove[6] = SLK_RX_NEW_REG_READ6;
		if (chan_count >= 8)		p_head->m_i_rx_reg_recove[7] = SLK_RX_NEW_REG_READ7;
#endif


}

int slk_get_fpga_rx_ring_len_val(void)
{
	return FPGA_RX_RING_LEN;
}

int slk_get_fpga_rx_dma_len(void)
{
	return FPGA_RX_DMA_LEN;
}

int slk_get_tx_pcie_cmd_val(void)
{
	return TX_PCIE_CMD;
}

int slk_get_tx_cmd_belength(void)
{
	return TX_CMD_BELENGTH;
}

int slk_get_tx_addr_l_val(void)
{
	return TX_ADDR_L;
}

int slk_get_rx_pcie_cmd(void)
{
	return RX_PCIE_CMD;
}

int slk_get_rx_addr_u(void)
{
	return RX_ADDR_U;
}

int slk_get_rx_addr_l(void)
{
	return RX_ADDR_L;
}

int slk_get_tx_pcie_cmd_data1(void)
{
	return TX_PCIE_CMD_DATA1;
}

int slk_get_tx_pcie_cmd_data2(void)
{
	return TX_PCIE_CMD_DATA2;
}

int slk_get_rx_pcie_cmd_data1(void)
{
	return RX_PCIE_CMD_DATA1;
}

int slk_tmo_get_mem_tx(void)
{
	return SLK_MEM_TX_NODE_COUNT;
}

