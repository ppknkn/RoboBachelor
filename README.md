# RoboBachelor

RoboBachelor 是 SparkR 轮腿机器人的 PID 固件与机械设计开源整理版，面向 STM32F427 主控。

**English overview:** RoboBachelor provides STM32F427 wheel-legged robot firmware and native SolidWorks models. The controller uses cascaded PI/P/PD loops. The complete-source IMU backend uses MIT LibDriver MPU6500 transport and an adapted GPL Mahony implementation; it is not the old A-board example. Full Keil compilation/linking and offline tests pass. The new backend and mechanical assemblies have not been validated on hardware or rebuilt in CAD.

本版本是 PID 系列控制，不含 VMC-LQR 或强化学习。原来调通的项目另外完整归档在本地，不随公共仓库发布。新 IMU 后端保持控制接口的轴向、单位和 PID 默认值，但不保证与旧固件数值或动态行为完全一致，见[后端变更](docs/imu-backend.md)。

## 使用前

按作者最终要求，保留原 STOP 拨杆和原启动使能流程，**没有新增 ARM/DISARM 操作或遥控失联保护，也没有永久禁能宏**。初始化成功后会请求使能电机；下载和检查时应物理断开动力。STOP 分支抑制新的关节位置/速度目标并发送零轮力矩，不代表物理断电或电机卸力。接收机 RF209S 接 CH9、HOLD，实际端口模式及 STOP 解析尚未在本轮实测。先读[控制与软件边界](docs/safety.md)、[已知问题](docs/known-issues.md)和[上电检查](docs/bring-up.md)。

## 目录

```text
firmware/MDK-ARM/Imu.uvprojx  Keil 主工程，目标 Imu
firmware/MDK-ARM/bsp/         IMU兼容接口、HAL/AUX适配、原遥控解析和调参
firmware/Src/control/        PI/P/PD、状态估计与腿部运动学
firmware/Src/motor/          DM4310 / JC 协议
firmware/ThirdParty/         许可明确的 LibDriver 子集与 Mahony 源码
firmware/Drivers/            工程需要的 ST HAL / CMSIS
mechanical/native/          2个SLDASM + 9个SLDPRT，原文件名/二进制保留
docs/                       硬件依据、构建、控制、变更与验证
tests/、tools/              离线检查，无刷写或设备访问
LICENSES/                   许可证全文
```

构建入口见[build.md](docs/build.md)；硬件、代码可核实的接线与不完整 BOM 见[hardware.md](docs/hardware.md)；模型入口见[mechanical/README.md](mechanical/README.md)。没有猜测的连接器针脚、整机采购清单、性能指标或支持硬件承诺。模型未做打开、重建或导出检查，现有资料没有STEP/STL。

## 实际控制

`main → robot_logic_update / state_estimator_update → balance_ctrl_update`：位置保持PI修正目标线速度，速度P输出倾角修正，直立PD换算轮力矩，偏航角速度P合成左右差速力矩，腿部逆运动学生成位置目标；起身使用MIT速度目标。参数和单位见[control.md](docs/control.md)。

## 已执行验证

完整 Keil ARMCC 5.06u7 重建 **38个C文件、启动汇编及链接，0错误/0警告**；ARM GCC 13.3.0编译38个C目标文件 **0失败/0警告**。主机测试覆盖新IMU寄存器模拟、SPI端口、Mahony数学、原STOP状态/缩放、电机帧编码及6项离线帧工具测试。详见[validation.md](docs/validation.md)。没有连接机器人、刷写、驱动电机或取得实物性能结果；不分发本地编译二进制。

## 许可和贡献

原创代码/工具 **GPL-3.0-only**，原创CAD **CERN-OHL-S-2.0**；LibDriver保留MIT，Mahony保留上游GPL-3.0-or-later及作者通知，ST/Arm保留原许可，DM模型按作者向厂家确认的MIT单列。范围见[LICENSE.md](LICENSE.md)及[第三方通知](THIRD_PARTY_NOTICES.md)。贡献前请读[CONTRIBUTING.md](CONTRIBUTING.md)，说明实际验证范围，避免提交凭据、个人配置、构建产物或无权发布的素材。
