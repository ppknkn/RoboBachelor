# 完整源码 IMU 后端

旧A板示例没有找到覆盖其DJI代码的许可，因此旧 bsp_imu、myiic、mytype 和 MPU/IST寄存器头不在本次分发中。它们在作者本地归档保留。新文件使用相同接口名以兼容调用方，**不代表旧实现获重新授权**，也不称 clean-room。

## 实际来源

- [LibDriver MPU6500](https://github.com/libdriver/mpu6500/tree/a66ceb7a5f18473f4807daebf0c63afdc077cc4f)，MIT，Copyright (c) 2015 - present LibDriver，作者 Shifeng Li；随附固定来源、原文件哈希及保留函数清单。保留 init、原始寄存器传输和AUX所需函数，函数体来自上游，寄存器定义移到单独头；完整上游头文件仍含被省略API的声明，不表示这些功能被链接。没有DMP镜像、AHRS.lib或下载时补依赖的要求。
- [RoboMaster C板 MahonyAHRS.c/h](https://github.com/RoboMaster/Development-Board-C-Examples/tree/59d12b1adcd321dbf1f9e9166aef5eb95ab657bf/18.ins_task/components/algorithm)，保留2019 RoboMaster授权与 SOH Madgwick 作者说明，GPL-3.0-or-later。确实从此源码改造，见文件内修改说明。
- 新写板级SPI/AUX事务、校准、兼容输出，GPL-3.0-only。器件寄存器定义基于MIT驱动；IST地址/DRDY解码可对照同一GPL C板 ist8310driver.c。未复制旧A板函数或辅助头文件作为新实现。

## 硬件路径和配置

继续使用SPI5、PF6片选、MPU6500及其AUX I2C上的IST8310（地址0x0e）；没有改接MCU I2C3。保留项目明确的±2000dps/±8g、CONFIG=4、ACCEL_CONFIG2=2、PWR_MGMT_1=3等配置并读回检查。初始化检查设备ID和传输错误，不把库示例默认值当本板配置。

新AUX模块使用有界SLV4事务完成磁传感器寄存器读写/校验；再以SLV1触发单次测量、SLV0自动读取。IST平均0x41=0x24、脉冲配置0x42=0xc0、单次触发0x0a=1、辅助延迟字段3保持为显式板级参数。SLV0改为读从0x02开始的7字节（DRDY+XYZ），不再只读6字节或用数值是否变化推断新样本。DRDY是MPU外部寄存器中的自动采样快照，读取它不证明实物刷新周期或每次CPU读取都是不同转换；须实物验证。初始化有界等待首个有效磁样本，缺磁不报告九轴初始化成功。

## 接口及有意差异

- 保留main的6个IMU入口、imu字段名和消费者方向：rol/pit/yaw为度，wx/wy/wz为rad/s，原始加速度/磁场为有符号计数；主控仍从rol/wx取平衡轴，从yaw/wz取偏航。
- 保留陀螺raw/16.384/57.3的项目缩放，没有直接换成LibDriver的raw/16.4物理输出；PID/PI默认增益、运动学、CAN映射不变。
- 校准由新代码按300个样本、每样本5ms、32位累计计算陀螺均值，不减掉重力加速度。偏置减法用32位，避免极端raw差值在16位中回绕。
- 真正GPL Mahony核心改为HAL tick可变dt，不硬套C板1000Hz；保持仅yaw磁反馈及±0.5积分限幅。C板半误差定义下twoKp=0.8、twoKi=0.005，对应原项目全误差比例/半步积分约定；这些是融合反馈系数，不是机器人PID增益。
- 归一化改用sqrtf而非类型穿透快速倒数平方根；新初姿使用标准倾斜补偿磁航向和ZYX四元数。零加速度、非有限四元数、总线失败和大于100ms积分间隔有明确无效状态。
- 新增IMU健康状态：采样20ms、磁DRDY快照200ms阈值；它不是无线失联保护。初始化失败时在请求使能电机前退出；运行健康失败沿用原倾倒停机的锁存路径，需重新启动。不能将这些软件条件描述为独立安全系统。

运算顺序、归一化、初始姿态、传输时延及磁采样策略不同；新后端未真机调试，不能声称和旧调通版本逐位或动态等价。完整源码和编译成功不等于硬件可直接闭环。原STOP/启动策略另见[safety.md](safety.md)。
