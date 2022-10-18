#ifndef FPGA_NET_ETHTOOL_H_20201102
#define FPGA_NET_ETHTOOL_H_20201102

#include <linux/netdevice.h>   /* struct device, and other headers */

extern void slnic_net_set_ethtool_ops(struct net_device* dev);
extern int  slnic_net__set_phys_id(struct net_device *netdev,enum ethtool_phys_id_state state);

#define SLK_DRV_VERSION	"0.0.01"
#define SLK_RTL_REGS_VER 1

/* The forced speed, 10Mb, 100Mb, gigabit, [2.5|5|10|20|25|40|50|56|100]GbE. */
#define SLK_SPEED_10					10
#define SLK_SPEED_100					100
#define SLK_SPEED_1000					1000
#define SLK_SPEED_2500					2500
#define SLK_SPEED_5000					5000
#define SLK_SPEED_10000					10000
#define SLK_SPEED_20000					20000
#define SLK_SPEED_25000					25000
#define SLK_SPEED_40000					40000
#define SLK_SPEED_50000					50000
#define SLK_SPEED_56000					56000
#define SLK_SPEED_100000				100000

#define SLK_SPEED_UNKNOWN				-1
#define SLK_RTL_NUM_STATS				4

#define SLK_SUPPORTED_10baseT_Half		(1 << 0)
#define SLK_SUPPORTED_10baseT_Full		(1 << 1)
#define SLK_SUPPORTED_100baseT_Half		(1 << 2)
#define SLK_SUPPORTED_100baseT_Full		(1 << 3)
#define SLK_SUPPORTED_1000baseT_Half	(1 << 4)
#define SLK_SUPPORTED_1000baseT_Full	(1 << 5)
#define SLK_ADVERTISED_Autoneg			(1 << 6)
#define SLK_SUPPORTED_Autoneg			(1 << 6)
#define SLK_SUPPORTED_TP				(1 << 7)
#define SLK_ADVERTISED_TP				(1 << 7)

#define SLK_PORT_TP						0x00
/* Which transceiver to use. */
#define SLK_XCVR_INTERNAL				0x00 /* PHY and MAC are in the same package */
#define SLK_XCVR_EXTERNAL				0x01 /* PHY and MAC are in different packages */
#define SLK_XCVR_DUMMY1					0x02
#define SLK_XCVR_DUMMY2					0x03
#define SLK_XCVR_DUMMY3					0x04

/* Duplex, half or full. */
#define SLK_DUPLEX_HALF					0x00
#define SLK_DUPLEX_FULL					0x01
#define SLK_DUPLEX_UNKNOWN				0xff

/* Enable or disable autonegotiation. */
#define SLK_AUTONEG_DISABLE				0x00
#define SLK_AUTONEG_ENABLE				0x01

/* MDI or MDI-X status/control - if MDI/MDI_X/AUTO is set then
* the driver is required to renegotiate link
*/
#define SLK_ETH_TP_MDI_INVALID			0x00 /* status: unknown; control: unsupported */
#define SLK_ETH_TP_MDI					0x01 /* status: MDI;     control: force MDI */
#define SLK_ETH_TP_MDI_X				0x02 /* status: MDI-X;   control: force MDI-X */
#define SLK_ETH_TP_MDI_AUTO				0x03 /*                   control: auto-select */

#define SLK_ETH_GSTRING_LEN				32
#define SLK_FLAG_HAS_HW_TIMESTAMP		(1 << 14)

#define SLK_FPGA_MAX_TXD				4096
#define SLK_FPGA_MIN_TXD				64
#define SLK_FPGA_MAX_RXD				4096
#define SLK_FPGA_MIN_RXD				64
#define SLK_FPGA_DEFAULT_RXD			256
#define SLK_FPGA_DEFAULT_TXD			256


/* Number of Transmit and Receive Descriptors must be a multiple of 8 */
#define REQ_TX_DESCRIPTOR_MULTIPLE  8
#define REQ_RX_DESCRIPTOR_MULTIPLE  8


/* Flow Control Settings */
typedef enum {
	FPGA_FC_NONE = 0,
	FPGA_FC_RX_PAUSE = 1,
	FPGA_FC_TX_PAUSE = 2,
	FPGA_FC_FULL = 3,
	FPGA_FC_DEFAULT = 0xFF
} fpga_fc_type;

/* Structure containing variables used by the shared code (e1000_hw.c) */
struct fpga_hw {
	fpga_fc_type fc;
};


#define LED_ON	(1)
#define LED_OFF (0)

#endif
