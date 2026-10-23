# 硬件与接线依据

下面是从代码与文件名核实的接口和部件线索，不是完整 BOM，不保证与任何未检查实物一致。

| 项目 | 代码或文件依据 | 当前能够确认的内容 |
| --- | --- | --- |
| MCU | `firmware/MDK-ARM/Imu.uvprojx`, `Imu.ioc` | STM32F427IIHx，Cortex-M4，硬浮点配置 |
| 控制板 | `Robomaster_A_board.SLDPRT` | 存在该板外形模型；板版本、电源与连接器定义未核实 |
| 关节电机 | `dm4310.h`, `main.c` | DM4310 协议，4 个控制 ID；具体商品变体未核实 |
| 轮毂电机 | `jc_motor.h`, `main.c` | JC 协议，轮子 ID 5 / 6；`JC4310尺寸.SLDPRT` 仅为型号线索 |
| IMU | `bsp_imu.c` | MPU6500 SPI 驱动与 IST8310 磁力计相关实现；有 `BOARD_DOWN=1` 安装配置 |
| 遥控接收 | `bsp_dbus.c`, `usart.c` | USART1 接收，解析包含 0x0F 帧头的通道数据；用户确认 WFLY RF209S；实际端口模式、拨杆/失效位未核实，保留原STOP解析、无新增失联保护，详见 receiver-rf209s.md |
| 结构尺寸 | `leg_kinematics.h` | 固件参数 L1=83 mm、L2=80 mm、轮半径 33 mm；未在 CAD 中量测 |

## MCU 引脚映射

依据是 `firmware/Src/can.c`、`spi.c`、`usart.c` 和 `MDK-ARM/bsp/bsp_imu.c`。这些是 MCU 端口，**不是板上连接器针脚编号**。

| 用途 | 外设与 MCU 管脚 | 代码配置 |
| --- | --- | --- |
| 共用电机 CAN | CAN1 RX PD0，TX PD1 | Prescaler=3，BS1=11TQ，BS2=2TQ；不要脱离时钟配置推断兼容波特率 |
| IMU SPI | SPI5 SCK PF7，MISO PF8，MOSI PF9；片选 PF6 | `bsp_imu.c` 直接操作 PF6 |
| 遥控输入 | USART1 RX PB7，TX PB6 | 100000，HAL WordLength=8B，Even parity，2 stop，RX 模式；有效载荷位宽需结合 STM32 的 parity 规则和接收机核实 |
| 调试和调参 | UART8 TX PE1，RX PE0 | 115200，8B，无校验，1 stop |
| 已初始化备用串口 | USART6 TX PG14，RX PG9；UART5 TX PC12，RX PD2 | 均为 115200、8B、无校验、1 stop；不据此宣称外设支持 |

CAN 必须经适合实物电平的收发器连接电机。当前资料没有收发器型号、终端电阻位置、线束图、电池电压、保险与供电规格，请勿把 MCU 的 CAN TX/RX 当作 CANH/CANL。

## 电机映射和符号

| 物理位置 | 本发布主程序采用的 ID | 向前转动对应关节反馈符号 |
| --- | --- | --- |
| 左前关节 | DM 1 | 正 |
| 左后关节 | DM 2 | 负 |
| 右后关节 | DM 3 | 负 |
| 右前关节 | DM 4 | 正 |
| 左轮 | JC 5 | `main.c` 速度反馈直接换算 |
| 右轮 | JC 6 | `main.c` 速度反馈换算后取负 |

`jc_motor.h` 还保留了历史 JC 关节宏，不应把它们当作当前 DM 接线表。DM 反馈路由要求 StdId=0，并从 data[0] 低 4 位识别关节。JC 反馈为 `0x580 + ID`。JC enable/idle 的动作值已按来源更正为 1；电机端 ID、工作模式、零位、反馈主 ID 和协议固件必须与代码逐项核对；不应直接复制别人的标定偏置。

## 尚缺的复现资料

完整采购 BOM（数量、型号、版本、材料、紧固件）、原理图、连接器定义、供电范围、额定/峰值电流、保护器件、加工图、装配公差、线束走向和实测零位都未随本目录提供。没有这些资料不能声称按文档即可完整复刻，也不能据文件名推荐替代硬件。
