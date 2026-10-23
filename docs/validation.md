# 验证记录 — 2026-10-23

以下针对完整源码新IMU版。未连接机器人、刷写或操作电机；本地编译产物不分发。

| 检查 | 实际结果 | 边界 |
| --- | --- | --- |
| 原始归档 | 5673文件、598231595字节，原始完整备份有逐文件SHA-256；迁移前候选另有385文件/81469792字节快照 | 两阶段均只在公共目录外；不将候选称为原硬件调通版本 |
| Keil全量重建 | MDK Plus5.34、ARMCC5.06u7 build960；38个C文件+startup汇编+链接，0错误/0警告 | 无硬件验证；实际CMSIS6.3/CORE6.2、DFP2.15 |
| 链接大小 | Code32360、RO652、RW184、ZI3712字节 | 非运行内存峰值测量 |
| ARM GCC | GCC13.3.0，38个C目标，0失败/0警告 | 不执行ARMCC启动汇编或GCC链接 |
| IMU栈主机测试 | 真实新后端、LibDriver子集与Mahony配寄存器总线mock：配置、300样本校准、AUX ID/NACK/有界超时、缺磁、单位/符号、已知倾角、tick回绕、已知旋转、归一化通过 | 合成数据，非真实传感器录制；不验证SPI/AUX物理时序 |
| HAL端口测试 | 真实imu_port配mock HAL，检查SPI读写位、片选收尾与失败传播 | 不连接SPI设备 |
| 原STOP/比例 | 实际robot_logic_update的STOP、退出STOP、状态和目标缩放测试 | 不证明原RF209S拨杆正确解析；未测试电机实物 |
| 电机协议编码 | 真实DM/JC/bsp_can配mock HAL，FD/FC、A0/A2动作值1、入队失败传播 | 不发送CAN，入队不等于ACK |
| 离线帧工具 | 6项固定字节SBUS/DBUS假设解码测试通过 | 人工向量，不证明实际协议/端口模式 |
| 正常PID保持 | 默认增益、balance控制计算、电机ID/方向和消费者单位保持；新后端数值差异另列 | 不保证旧/新固件等价 |
| 模型与ST文件 | 11模型与原件SHA-256一致；284个ST库文件与本机CubeF4 1.28.3一致 | CAD未成功打开/重建/导出；新电脑pack解析未复现 |

运行 `python tools/run_host_tests.py` 重现主机测试，`python tools/check_firmware.py` 重现附加ARM C编译；Keil见[build.md](build.md)。云端CI使用运行器预装的GCC/Python执行主机离线测试，不安装额外工具链；ARM GCC与Keil链接是上述实际本机检查，CI不能替代这些检查或硬件试验。
