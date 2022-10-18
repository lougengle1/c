#ifndef SLK_PCIE_CFG_SPACE_H_20190123
#define SLK_PCIE_CFG_SPACE_H_20190123

#define			SLK_MSG_TYPE_PCIE_CFG					'P'			/// 配置pcie配置空间消息类型
#define			SLK_PCIE_WRITE_CFG_SPACE				'w'			/// 写配置空间命令
#define			SLK_PCIE_READ_CFG_SPACE					'r'			/// 读配置空间命令
#define			SLK_PCIE_BASE_TYPE						'b'			/// pcie基本配置空间
#define			SLK_PCI_EXT_TYPE						'e'			/// pcie扩展配置空间
#define			SLK_PCI_SELF_TYPE						(0)			/// 设备自己配置空间
#define			SLK_PCI_BUS_TYPE						(1)			/// 设备总线配置空间


#pragma pack(push, 1)
struct slk_pcie_cfg_param
{
	unsigned char				m_type;					/// 消息类型，'M'
	unsigned char				m_pci_type;				/// pcie类型，设备本身，设备的总线
	unsigned char				m_space_type;			/// 读取pci配置类型，基地址配置空间，pci扩展空间， pcie扩展空间
	unsigned char				m_sub_type;				/// 读写标志
	unsigned int				m_i_capability;			/// 扩展空间块地址
	unsigned int				m_i_pos;				/// 读写的位置
	unsigned int				m_i_len;				/// 读写的长度
	unsigned int				m_i_value;				/// 读写的数据
};
#pragma pack(pop)

/// 处理pcie配置空间操作函数
// extern int slk_pcie_space_config(SLK_PTR_PCI_DEV pcie_dev, struct slk_pcie_cfg_param* p_param);


#endif