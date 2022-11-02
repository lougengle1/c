#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "user_irq_define.h"
#include <sys/ioctl.h>


#define WRITE_REG(a, reg, val) \
        (*((volatile uint32_t*)(a + reg))) = (uint32_t)(val)

#define READ_REG(a, reg) \
        (uint32_t)(*((volatile uint32_t*)(a + reg)))

#define SLNIC_DEV_NAME "/dev/slnic/slnic"
#define SLNIC_REG_NAME "/dev/slnic/register"
#define REG_MAP_SIZE	10
#define PORT_NUM		2
#define IOCTL_SLN		1
#define PAGE_SIZE		4096

struct slnic_device_info
{
	unsigned int	port;
	int				rx_enable;
	unsigned long	rx_packets;
	unsigned long	tx_packets;
	unsigned long	rx_bytes;
	unsigned long	tx_bytes;
	unsigned long	rx_errors;
	unsigned long	tx_errors;
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
};

#pragma pack(push, 1)


struct sl_version_info
{
	unsigned short production;
	unsigned short edition;
};



#pragma pack(pop)


static int show_all_device()
{
	int reg_fd;
	int sln_fd;
	char device_slnicl_file[64];
	char device_register_file[64];
	void *reg_map;
	uint32_t temp;
	uint32_t version;
	int port_num;
	struct sl_version_info version_data;
	uint32_t sys_uinfo1;

	snprintf(device_slnicl_file, sizeof(device_slnicl_file), SLNIC_DEV_NAME);

	if (access(device_slnicl_file, F_OK) != 0)
	{
		printf("access is failed\n");
		return 0;
	}


	printf("Device %s\n", device_slnicl_file + 11);

	if ((sln_fd = open(SLNIC_DEV_NAME, O_RDWR)) < 0)
	{
		printf("open sln_fg is failed\n");
		return 0;
	}

	snprintf(device_register_file, sizeof(device_register_file), SLNIC_REG_NAME);
	if (access(device_register_file, F_OK) != 0)
	{
		printf("access if failed\n");
		return 0;
	}

	if ((reg_fd = open(SLNIC_REG_NAME, O_RDWR)) < 0)
	{
		printf("open reg_fd is failed\n");
		return 0;
	}

	reg_map = mmap(NULL, 64 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, reg_fd, 0);

	if (reg_map == MAP_FAILED)
	{
		printf("reg mmap error\n");
		return 0;
	}

	temp = READ_REG(reg_map, BOARD_TEMPERATURE_DATA);
	printf("  Temperature: %u\n", temp);
	//version_data = READ_REG(reg_map, SYS_ID);
	version = READ_REG(reg_map, SYS_ID);
	version_data = *((struct sl_version_info*)(reg_map + SYS_ID));
	sys_uinfo1 = READ_REG(reg_map, SYS_UINFO1);
	printf("	sys_uinfo1:%x \n", sys_uinfo1);
	printf("	production:%d Edition%x\n", version_data.production, ntohs(version_data.edition));
	printf("	version_data is %x\n", version);
	//printf("  hardware_version:V%d.%d.%d\n", version_data.version1, version_data.version2, version_data.version3);
	//printf("  edition id:20%d%d.%d%d.%d%d\n", version_data.year1, version_data.year2, version_data.month1, version_data.month2, version_data.days1, version_data.days2);

	for (port_num = 0; port_num < PORT_NUM; port_num++)
	{
		struct slnic_device_info info;
		info.port = port_num;
		if (ioctl(sln_fd, IOCTL_SLN, &info) != 0)
		{
			printf("slnic ioctl error\n");
			return 0;
		}

		printf("  Port %u\n", info.port);
		printf("	Interface: slnic%d\n", port_num);
		if (info.rx_enable)
		{
			printf("	Port status:enable\n");
		}
		else
		{
			printf("	Port status:false\n");
		}

		printf("	RX packets:%lu  RX bytes:%lu\n", info.rx_packets, info.rx_bytes);
		printf("	RX error:%lu\n", info.rx_errors);
		printf("	TX packets:%lu	TX bytes:%lu\n", info.tx_packets, info.tx_bytes);
		printf("	TX error:%lu\n", info.tx_errors);
		printf("	TX dropped:%lu\n", info.tx_dropped);
	}
	munmap(reg_map, 64 * PAGE_SIZE);
	close(sln_fd);
	close(reg_fd);
}

static int set_mac_reg(const char *mac,const char *mac_value)
{
	int reg_fd;
	void* reg_map;
	char port_num;
	uint32_t mac_h;
	uint32_t mac_l;
	unsigned char mac_addr[6];
	if ((reg_fd = open(SLNIC_REG_NAME, O_RDWR)) < 0)
	{
		printf("open reg_fd is failed\n");
		return 0;
	}

	reg_map = mmap(NULL, 64 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, reg_fd, 0);

	if (reg_map == MAP_FAILED)
	{
		printf("reg mmap error\n");
		return 0;
	}

	port_num = atoi(&mac[3]);
	sscanf(mac_value, "%x:%x:%x:%x:%x:%x", &mac_addr[0], &mac_addr[1], &mac_addr[2], &mac_addr[3], &mac_addr[4], &mac_addr[5]);
	mac_h = (mac_addr[0]) | (mac_addr[1]) << 8;
	mac_l = (mac_addr[2]) | mac_addr[3] << 8 | mac_addr[4] << 16 | mac_addr[5] << 24;

	WRITE_REG(reg_map, MAC0_ADDR_H + port_num * MAC_OFFSET, mac_h);
	WRITE_REG(reg_map, MAC0_ADDR_L + port_num * MAC_OFFSET, mac_l);
	munmap(reg_map, 64 * PAGE_SIZE);
	close(reg_fd);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		show_all_device();
		return 0;
	}

	if (argc == 4 && strcmp(argv[1], "set") == 0 && strncmp(argv[2], "mac", 3) == 0)
	{
		set_mac_reg(argv[2],argv[3]);
		return 0;
	}

	return 0;
}
