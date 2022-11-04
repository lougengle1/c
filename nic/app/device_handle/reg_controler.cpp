#include "reg_controler.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

#include "perform_define.h"

reg_controler::reg_controler()
{
	m_device_fd = -1;
	m_bar0_addr = NULL;
}

reg_controler::~reg_controler()
{
	if (m_device_fd >= 0)
		close(m_device_fd);
	if (m_bar0_addr)
		munmap(m_bar0_addr, SLK_BAR0_SIZE);
}

bool reg_controler::init(const char * file_name)
{
	if (!just_init(file_name))
		return false;

	if (ioctl(m_device_fd, SLK_IOCTL_CONTROL_RESET, NULL))
		perror("device reset error!");

	return true;
}

bool reg_controler::just_init(const char * file_name)
{
	m_device_fd = open(file_name, O_RDWR);
	if (m_device_fd < 0)
		return false;

	m_bar0_addr = mmap(NULL, SLK_BAR0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, m_device_fd, 0);
	if (m_bar0_addr == (void *)-1)
	{
		perror("mmap bar0 addr");
		close(m_device_fd);
		m_device_fd = -1;
		m_bar0_addr = NULL;
		return false;
	}
	mlock(m_bar0_addr, SLK_BAR0_SIZE);

	return true;
}

void* reg_controler::get_bar0_address()
{
	if (m_bar0_addr == NULL)
		return NULL;

	return m_bar0_addr;
}

bool reg_controler::exit()
{
	if (m_device_fd >= 0)
		close(m_device_fd);

	if (m_bar0_addr != (void *)-1)
	{
		munmap(m_bar0_addr, SLK_BAR0_SIZE);
		m_bar0_addr = NULL;
	}

	return true;
}

bool reg_controler::read_reg(long addr, unsigned int &val)
{
	if (!m_bar0_addr)
		return false;

	val = *(volatile uint32_t *)((char *)m_bar0_addr + addr);
	return true;
}

bool reg_controler::write_reg(long addr, long val)
{
#if 0
	char write_buff[8];
	*(uint32_t *)(write_buff + 0) = addr;
	*(uint32_t *)(write_buff + 4) = val;

	write(m_device_fd, write_buff, 8);
	return true;
#endif

	if (!m_bar0_addr)
		return false;
	
	*(volatile uint32_t *)((char *)m_bar0_addr + addr) = val;

	return true;
}

