#ifndef _SL_NIC_DRIVER_H_
#define _SL_NIC_DRIVER_H_


static int sl_pci_probe(struct pci_dev* pdev, const struct pci_device_id* id);
static void sl_pci_remov(struct pci_dev* pdev);
void pci_enable_irq(void);

#endif