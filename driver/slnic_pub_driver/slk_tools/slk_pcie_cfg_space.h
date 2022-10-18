#ifndef SLK_PCIE_CFG_SPACE_H_20190123
#define SLK_PCIE_CFG_SPACE_H_20190123

#define			SLK_MSG_TYPE_PCIE_CFG					'P'			/// ����pcie���ÿռ���Ϣ����
#define			SLK_PCIE_WRITE_CFG_SPACE				'w'			/// д���ÿռ�����
#define			SLK_PCIE_READ_CFG_SPACE					'r'			/// �����ÿռ�����
#define			SLK_PCIE_BASE_TYPE						'b'			/// pcie�������ÿռ�
#define			SLK_PCI_EXT_TYPE						'e'			/// pcie��չ���ÿռ�
#define			SLK_PCI_SELF_TYPE						(0)			/// �豸�Լ����ÿռ�
#define			SLK_PCI_BUS_TYPE						(1)			/// �豸�������ÿռ�


#pragma pack(push, 1)
struct slk_pcie_cfg_param
{
	unsigned char				m_type;					/// ��Ϣ���ͣ�'M'
	unsigned char				m_pci_type;				/// pcie���ͣ��豸�����豸������
	unsigned char				m_space_type;			/// ��ȡpci�������ͣ�����ַ���ÿռ䣬pci��չ�ռ䣬 pcie��չ�ռ�
	unsigned char				m_sub_type;				/// ��д��־
	unsigned int				m_i_capability;			/// ��չ�ռ���ַ
	unsigned int				m_i_pos;				/// ��д��λ��
	unsigned int				m_i_len;				/// ��д�ĳ���
	unsigned int				m_i_value;				/// ��д������
};
#pragma pack(pop)

/// ����pcie���ÿռ��������
// extern int slk_pcie_space_config(SLK_PTR_PCI_DEV pcie_dev, struct slk_pcie_cfg_param* p_param);


#endif