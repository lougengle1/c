#include <linux/pci.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h> /* sl_debug() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> /* mark_bh */
#include <linux/time.h>
#include <linux/rtc.h>
#include "sl_nic_define.h"
#include "control_cdev.h"
#include "slk_tools/slk_kernel.h"
#include "slk_tools/slk_define.h"


static
loff_t control_llseek(struct file *filp, loff_t off, int whence)
{
	return 0;
}

static
ssize_t control_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
	return 0;
}

static
ssize_t control_write(struct file * pfile, const char __user * ch_user, size_t count, loff_t * pos)
{
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static
int	control_open(struct inode *pnode, struct file* pfile)
{
	sl_debug("[slnic  control_open] open\n");

	struct cherry_kiwi_adapter *adapter = container_of(pnode->i_cdev, struct cherry_kiwi_adapter, m_dev_reg);

	pfile->private_data = adapter;

	return 0;
}

static
int control_release(struct inode * pnode, struct file * pfile)
{
	return 0;
}

static
int control_data_mmap( struct file * filp, struct vm_area_struct *vma )
 {
	struct cherry_kiwi_adapter *p_driver_data = filp->private_data;

	 int						page_count = 0;
	 volatile unsigned long		flags;
	 unsigned long				rest = 0;
	 unsigned long				reg_addr_base0;
	 unsigned int				reg_base_len;

	 vma->vm_flags |= VM_IO;
	 vma->vm_flags |= SLK_VM_RESERVED;

	 sl_debug("come in mmap");

	 reg_addr_base0 = pci_resource_start(p_driver_data->m_dev_pci, 0);
	 reg_base_len   = pci_resource_len(p_driver_data->m_dev_pci, 0);

	 flags = task_thread_info(current)->flags;
	 rest = test_bit(TIF_SIGPENDING, &flags);
	 if( rest )
	 {
		 sl_error("mmap recv error flag0");
		 return -EAGAIN;
	 }

	 page_count = (reg_base_len % PAGE_SIZE == 0 ) ? (reg_base_len / PAGE_SIZE) : (reg_base_len / PAGE_SIZE + 1);

	 if( (vma->vm_end - vma->vm_start)/PAGE_SIZE < page_count )
	 {
		 sl_error("mmap the len is short:%d", (int)(vma->vm_end - vma->vm_start));
		 return -EAGAIN;
	 }

	return remap_pfn_range(vma, vma->vm_start, 
			reg_addr_base0 >> PAGE_SHIFT, 
			PAGE_SIZE * page_count,
			pgprot_noncached(vma->vm_page_prot));

	 return 0;
 }

static
long control_data_ioctl(struct file * pfile, unsigned int cmd, unsigned long arg)
{
	//sl_debug("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

 struct file_operations g_fop_control =
 {
	 .owner = THIS_MODULE,
	 .llseek = control_llseek,
	 .read = control_read,
	 .write = control_write,
	 .open = control_open,
	 .release = control_release,
	 .mmap = control_data_mmap,
	 .unlocked_ioctl = control_data_ioctl,
 };
