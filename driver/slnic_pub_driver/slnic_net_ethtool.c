#include <linux/pci.h>
#include "slnic_net_ethtool.h"
#include <linux/netdevice.h>
#include "sl_nic_define.h"
#include "sl_nic_netdev.h"
//#include <uapi/linux/ethtool.h>
#include <linux/version.h>
#include "user_irq_define.h"


static struct {
        const char str[ETH_GSTRING_LEN];
} ethtool_stats_keys[] = {
        { "rx_packets" },
        { "tx_packets" },
        { "rx_bytes" },
        { "tx_bytes" },
};

static void slnic_net_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct slk_net_priv *priv = netdev_priv(dev);

	strlcpy(info->driver, KIWI_POLL_DRIVER_NAME, sizeof(info->driver));
	strlcpy(info->version, SLK_DRV_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, pci_name(priv->pci_dev), sizeof(info->bus_info));
}

static inline void slnic_ethtool_cmd_speed_set(struct ethtool_cmd *ep, __u32 speed)
{
	ep->speed = (__u16)speed;
	ep->speed_hi = (__u16)(speed >> 16);
}

static int slnic_net_get_regs_len(struct net_device *dev)
{
	struct slk_net_priv *priv = netdev_priv(dev);

	return priv->regs_len;
}

static int slnic_net_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	u32 speed;

	cmd->supported = (SLK_SUPPORTED_10baseT_Half |
		SLK_SUPPORTED_10baseT_Full |
		SLK_SUPPORTED_100baseT_Half |
		SLK_SUPPORTED_100baseT_Full |
		SLK_SUPPORTED_1000baseT_Full |
		SLK_SUPPORTED_Autoneg |
		SLK_SUPPORTED_TP);
	cmd->advertising = (SLK_SUPPORTED_10baseT_Half |
		SLK_SUPPORTED_10baseT_Full |
		SLK_SUPPORTED_100baseT_Half |
		SLK_SUPPORTED_100baseT_Full |
		SLK_SUPPORTED_1000baseT_Full |
		SLK_SUPPORTED_Autoneg |
		SLK_SUPPORTED_TP);
	cmd->port = SLK_PORT_TP;
	cmd->transceiver = SLK_XCVR_INTERNAL;
	cmd->duplex = SLK_DUPLEX_UNKNOWN;
	speed = SLK_SPEED_10000;
	cmd->duplex = SLK_DUPLEX_FULL;

	slnic_ethtool_cmd_speed_set(cmd, speed);

	cmd->autoneg = SLK_AUTONEG_DISABLE;
	cmd->eth_tp_mdix = SLK_ETH_TP_MDI;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32)
	cmd->eth_tp_mdix_ctrl = SLK_ETH_TP_MDI_AUTO;
#endif
	return 0;
}

static void slnic_net_get_regs(struct net_device *dev, struct ethtool_regs *regs, void *regbuf)
{
	// struct slk_net_priv *priv = netdev_priv(dev);

	regs->version = SLK_RTL_REGS_VER;

}

static u32 slnic_net_get_link(struct net_device *dev)
{
	return netif_carrier_ok(dev) ? 1 : 0;
}

static u32 slnic_net_get_msglevel(struct net_device *dev)
{
	struct slk_net_priv *priv = netdev_priv(dev);

	return priv->msg_enable;
}

static void slnic_net_set_msglevel(struct net_device *dev, u32 datum)
{
	struct slk_net_priv *priv = netdev_priv(dev);

	priv->msg_enable = datum;
}

static void slnic_net_get_strings(struct net_device *dev, u32 stringset, u8 *data)
{
	memcpy(data, ethtool_stats_keys, sizeof(ethtool_stats_keys));
}

static int slnic_net_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return SLK_RTL_NUM_STATS;
	default:
		return -EOPNOTSUPP;
	}
}

static void slnic_net_get_ethtool_stats(struct net_device *dev,
	struct ethtool_stats *stats, u64 *data)
{
	struct slk_net_priv *priv = netdev_priv(dev);
	//printk("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

	data[0] = priv->stats.tx_packets;
	data[1] = priv->stats.rx_packets;
	data[2] = priv->stats.tx_bytes;
	data[3] = priv->stats.rx_bytes;
}

static void slnic_net_get_pauseparam(struct net_device *netdev,
	struct ethtool_pauseparam *pause)
{
	struct slk_net_priv *priv = netdev_priv(netdev);
	struct fpga_hw *hw = &priv->hw;

	pause->autoneg = (priv->fc_autoneg ? SLK_AUTONEG_ENABLE : SLK_AUTONEG_DISABLE);

	if (hw->fc == FPGA_FC_RX_PAUSE) {
		pause->rx_pause = 1;
	}
	else if (hw->fc == FPGA_FC_TX_PAUSE) {
		pause->tx_pause = 1;
	}
	else if (hw->fc == FPGA_FC_FULL) {
		pause->rx_pause = 1;
		pause->tx_pause = 1;
	}
}

static int slnic_net_set_pauseparam(struct net_device *netdev,
	struct ethtool_pauseparam *pause)
{
	struct slk_net_priv *priv = netdev_priv(netdev);
	struct fpga_hw *hw = &priv->hw;

	if (pause->rx_pause && pause->tx_pause)
		hw->fc = FPGA_FC_FULL;
	else if (pause->rx_pause && !pause->tx_pause)
		hw->fc = FPGA_FC_RX_PAUSE;
	else if (!pause->rx_pause && pause->tx_pause)
		hw->fc = FPGA_FC_TX_PAUSE;
	else if (!pause->rx_pause && !pause->tx_pause)
		hw->fc = FPGA_FC_NONE;

	return 0;
}

static int slnic_net_get_coalesce(struct net_device *netdev, struct ethtool_coalesce *ec)
{
	struct slk_net_priv *priv = netdev_priv(netdev);

	if (priv->itr_setting <= 4)
		ec->rx_coalesce_usecs = priv->itr_setting;
	else
		ec->rx_coalesce_usecs = 1000000 / priv->itr_setting;

	return 0;
}

static void slnic_net_get_ringparam(struct net_device *netdev,
	struct ethtool_ringparam *ring)
{
	struct slk_net_priv *priv = netdev_priv(netdev);

	ring->rx_max_pending = SLK_FPGA_MAX_RXD;
	ring->tx_max_pending = SLK_FPGA_MAX_TXD;
	ring->rx_pending = priv->rx_ring_count;
	ring->tx_pending = priv->tx_ring_count;
}

static int slnic_net_set_ringparam(struct net_device *netdev,
	struct ethtool_ringparam *ring)
{
	struct slk_net_priv *priv = netdev_priv(netdev);
	u16 new_rx_count;
	u16 new_tx_count;
	//printk("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

	if ((ring->rx_mini_pending) || (ring->rx_jumbo_pending))
		return -EINVAL;

	new_rx_count = clamp_t(u32, ring->rx_pending, SLK_FPGA_MIN_RXD,
		SLK_FPGA_MAX_RXD);
	new_rx_count = ALIGN(new_rx_count, REQ_RX_DESCRIPTOR_MULTIPLE);

	new_tx_count = clamp_t(u32, ring->tx_pending, SLK_FPGA_MIN_TXD,
		SLK_FPGA_MAX_TXD);
	new_tx_count = ALIGN(new_tx_count, REQ_TX_DESCRIPTOR_MULTIPLE);

	if ((new_tx_count == priv->tx_ring_count) &&
		(new_rx_count == priv->rx_ring_count))
	{
		/* nothing to do */
		return 0;
	}

	priv->tx_ring_count = new_tx_count;
	priv->rx_ring_count = new_rx_count;

	return 0;
}


static void slnic_net_setup_led(struct slk_net_priv* priv)
{
	sl_debug(KERN_ALERT "fpga set up led! \r\n");
}

static void slnic_net_led_on(struct slk_net_priv* priv)
{
	if (priv->port == 0)
	{
		WRITE_REG(g_driver_data.m_mem_base0, MAC0_LED, LED_ON);
		sl_debug("Mac0 LED ON");
	}
	else if (priv->port == 1)
	{
		WRITE_REG(g_driver_data.m_mem_base0, MAC1_LED, LED_ON);
		sl_debug("Mac0 LED ON");
	}
}

static void slnic_net_led_off(struct slk_net_priv* priv)
{
	if (priv->port == 0)
	{
		WRITE_REG(g_driver_data.m_mem_base0, MAC0_LED, LED_OFF);
		sl_debug("Mac0 LED OFF");
	}
	else if (priv->port == 1)
	{
		WRITE_REG(g_driver_data.m_mem_base0, MAC1_LED, LED_OFF);
		sl_debug("Mac1 LED OFF");
	}
}

static void slnic_net_cleanup_led(struct slk_net_priv* priv)
{
	sl_debug(KERN_ALERT "fpag clean up led! \r\n");
}

int slnic_net__set_phys_id(struct net_device *netdev,
			     enum ethtool_phys_id_state state)
{
	struct slk_net_priv* priv = netdev_priv(netdev);
	switch (state) {
	case ETHTOOL_ID_ACTIVE:
		slnic_net_setup_led(priv);
		return 2;

	case ETHTOOL_ID_ON:
		slnic_net_led_on(priv);
		break;

	case ETHTOOL_ID_OFF:
		slnic_net_led_off(priv);
		break;

	case ETHTOOL_ID_INACTIVE:
		slnic_net_cleanup_led(priv);
		break;
	}

	return 0;
}

static const struct ethtool_ops slnic_net_ethtool_ops = {
	.get_drvinfo = slnic_net_get_drvinfo,
	.get_settings = slnic_net_get_settings,
	.get_regs_len = slnic_net_get_regs_len,
	.get_regs = slnic_net_get_regs,
	.get_link = slnic_net_get_link,
	.get_msglevel = slnic_net_get_msglevel,
	.set_msglevel = slnic_net_set_msglevel,
	.get_strings = slnic_net_get_strings,
	.get_sset_count = slnic_net_get_sset_count,
	.get_ethtool_stats = slnic_net_get_ethtool_stats,
	.get_pauseparam = slnic_net_get_pauseparam, 
	.set_pauseparam = slnic_net_set_pauseparam,
	.get_coalesce = slnic_net_get_coalesce,
	.get_ringparam = slnic_net_get_ringparam,
	.set_ringparam = slnic_net_set_ringparam,
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,0,0)
	.get_ts_info = ethtool_op_get_ts_info,
	.set_phys_id = slnic_net__set_phys_id,
#endif
};

void slnic_net_set_ethtool_ops(struct net_device* dev)
{
	dev->ethtool_ops = &slnic_net_ethtool_ops;
}
