# 第三方内容与授权状态

| 内容 | 可见依据 | 发布处理 |
| --- | --- | --- |
| Arm CMSIS 核心头文件 | 源文件含 Arm 版权与 Apache-2.0 SPDX；原 CMSIS/LICENSE.txt 是 Apache 2.0 全文 | 保留源文件和原许可；本轮未修改库算法 |
| ST STM32F4 CMSIS Device 头文件 | Device/ST/STM32F4xx/LICENSE.txt 指向 Package_license，缺包级条款时回退 Apache-2.0 | 保留原声明与 Apache 全文；原目录缺包级条款，现从相同本机 CubeF4 1.28.3 包补充原样副本 |
| ST STM32F4 HAL | STM32F4xx_HAL_Driver/LICENSE.txt 指向 Package_license，缺条款时回退 BSD-3-Clause | 保留版权、原说明，并附 BSD 全文；GPL 不替换其原许可；284 个库文件与相同本机包逐字节一致 |
| ARMCC 启动文件 | startup_stm32f427xx.s 文件内有 ST 版权、三条再分发条件和完整免责声明 | 保留文件内 BSD-3-Clause 风格条款，未覆盖为 GPL |
| ST 生成模板 | 各文件 ST 版权；多处写明查根 LICENSE，无 LICENSE 则 AS-IS | 已核实 CubeMX 生成链、基础 Templates 的 BSD 回退文件及 ST 官方社区说明；保留版权，见来源文档 |
| 旧A板IMU及myiic/mytype/寄存器头 | 原项目无覆盖DJI代码的明确许可 | 原始/改写实现移出公共分发，私下归档，不是当前依赖 |
| LibDriver MPU6500 | a66ceb7a5f18473f4807daebf0c63afdc077cc4f，MIT | 原通知及LICENSE保留；只取raw/init/AUX所需函数，未带DMP；provenance.json列函数及哈希 |
| C板MahonyAHRS | RoboMaster C例程59d12b1adcd321dbf1f9e9166aef5eb95ab657bf，GPL-3.0-or-later；原SOH Madgwick作者说明 | 实际从有许可源码改造，保留作者/许可并标明dt、反馈、积分、归一化修改 |
| DM/JC 电机驱动 | 当前实现无第三方版权头，按项目原创部分授权 | 电机协议/型号兼容性仍未进行硬件验证；若发现搬入代码须补实际上游条款 |
| 厂商外形模型及装配中的第三方部件 | 文件名含 DM、JC、Robomaster、rline 标识，但没有许可文件 | 用户更正确认 JC/rline1050/Robomaster_A_board 为原创；DM 按用户向达妙官方确认的 MIT 单列并恢复，原始通知待补，装配完整性未验证 |

Keil、STM32CubeMX、编译器运行库、SolidWorks、调试器工具不包含在本项目分发中，也不由本项目许可。正式分发链接后的固件前，还需核对实际使用的工具链运行库及完整对应源要求；本候选版不分发旧二进制。

完整许可文本中 GPL/CERN/BSD 副本来自 SPDX license-list-data 的同名许可文件，CMSIS Apache 副本来自随原源码提供的文件。第三方通知和版权不得删除。当前文档不作所有依赖均已确认兼容的保证。
