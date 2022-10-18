#ifndef SLK_DEV_STATUS_H_20180511
#define SLK_DEV_STATUS_H_20180511

#include "slk_define.h"
#include "slk_type.h"

#define SLK_BIT_00	0x00000001
#define SLK_BIT_01	0x00000002
#define SLK_BIT_02	0x00000004
#define SLK_BIT_03	0x00000008
#define SLK_BIT_04	0x00000010
#define SLK_BIT_05	0x00000020
#define SLK_BIT_06	0x00000040
#define SLK_BIT_07	0x00000080

#define SLK_BIT_08	0x00000100
#define SLK_BIT_09	0x00000200
#define SLK_BIT_10	0x00000400
#define SLK_BIT_11	0x00000800
#define SLK_BIT_12	0x00001000
#define SLK_BIT_13	0x00002000
#define SLK_BIT_14	0x00004000
#define SLK_BIT_15	0x00008000

#define SLK_BIT_16	0x00010000
#define SLK_BIT_17	0x00020000
#define SLK_BIT_18	0x00040000
#define SLK_BIT_19	0x00080000
#define SLK_BIT_20	0x00100000
#define SLK_BIT_21	0x00200000
#define SLK_BIT_22	0x00400000
#define SLK_BIT_23	0x00800000

#define SLK_BIT_24	0x01000000
#define SLK_BIT_25	0x02000000
#define SLK_BIT_26	0x04000000
#define SLK_BIT_27	0x08000000
#define SLK_BIT_28	0x10000000
#define SLK_BIT_29	0x20000000
#define SLK_BIT_30	0x40000000
#define SLK_BIT_31	0x80000000

/// 项目定义
#define SLK_PROJ_REM_FUT_NORMAL				10				/// rem期货普通版
#define SLK_PROJ_REM_FUT_ULTRA				11				/// rem期货极致版
#define SLK_PROJ_REM_ONE_FUT_U50            12              /// rem期货整合一期u50版
#define SLK_PROJ_REM_OPTION_NORMAL			20				/// rem期权普通版
#define SLK_PROJ_REM_HK_NORMAL				31				/// rem香港普通版
#define SLK_PROJ_GQS_SSE					40				/// 行情机上海版
#define SLK_PROJ_SAAP_NORMAL				50				/// SAAP普通版

/// 定义项目的目标值
#define SLK_PROJ_REM_FUT_NORMAL_STATUS		0x00000132			/// rem期货普通版
#define SLK_PROJ_REM_FUT_ULTRA_STATUS		0x00000132			/// rem期货极致版
#define SLK_PROJ_REM_ONE_FUT_U50_STATUS     0x00000111          /// rem期货整合一期u50版
#define SLK_PROJ_REM_OPTION_NORMAL_STATUS	0x00000132			/// rem期权普通版
#define SLK_PROJ_REM_HK_NORMAL_STATUS		0x00000132			/// rem香港普通版
#define SLK_PROJ_GQS_SSE_STATUS				0x0001F333			/// 行情机上海版
#define SLK_PROJ_SAAP_NORMAL_STATUS			0x0001F333			/// SAAP普通版
/// 处理函数
/// 打印硬件状态信息
extern uint32_t slk_print_dev_status_error(int proj,uint32_t status, uint32_t right_status);
/// 打印硬件状态信息
/// 直接打印无视error开关
extern uint32_t slk_printk_dev_status_error(int proj, uint32_t status, uint32_t right_status);

/// 获取硬件设备状态出错的bit位,返回一个包含多个出错位并置高
extern uint32_t slk_get_dev_error_bit(uint32_t status, uint32_t right_status);

/// 获取不同项目每一位的含义
extern int slk_get_dev_bit_info(int proj,uint32_t bit,char* info,int len);

#endif