[简体中文](README.md) | [English](README.en.md)

# MPU6500 原始寄存器与辅助总线接口

本目录集成 [LibDriver MPU6500](https://github.com/libdriver/mpu6500/tree/a66ceb7a5f18473f4807daebf0c63afdc077cc4f) 中用于设备初始化、原始寄存器读写及 AUX I2C 配置的函数。固定版本：`a66ceb7a5f18473f4807daebf0c63afdc077cc4f`。

| 文件 | 用途 |
| --- | --- |
| [driver_mpu6500.c](driver_mpu6500.c) | 本项目使用的初始化、寄存器传输和 AUX 函数子集；保留的函数体沿用上游实现 |
| [driver_mpu6500.h](driver_mpu6500.h) | 未改的上游接口头文件 |
| [driver_mpu6500_registers.h](driver_mpu6500_registers.h) | 从上游实现中单独整理出的寄存器定义，保留原通知 |
| [provenance.json](provenance.json) | 上游版本、文件哈希与保留函数清单 |
| [LICENSE](LICENSE) | 上游完整许可与版权通知 |

## 在项目中的连接

[imu_port.c](../../MDK-ARM/bsp/imu_port.c) 将接口连接到 SPI5 与 PF6 片选；[imu_ist8310_aux.c](../../MDK-ARM/bsp/imu_ist8310_aux.c) 通过 MPU6500 的辅助总线访问磁力计。

这里编译的是原始寄存器/AUX 子集，不依赖 DMP 固件。头文件保留上游的完整声明；准备调用其他接口时，先对照 `driver_mpu6500.c` 和保留函数清单确认实现。

[IMU 后端说明](../../../docs/imu-backend.md) · [统一许可与署名](../../../LICENSE.md)
