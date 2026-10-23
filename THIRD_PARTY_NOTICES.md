# 第三方来源与通知

授权范围入口为 [LICENSE.md](LICENSE.md)。本表提供组件来源；原始许可证、版权和免责声明保留在源文件及相应目录，不能用本项目许可覆盖。

| 组件 | 来源与通知 |
| --- | --- |
| Arm CMSIS | 源文件的 Arm 版权与 Apache-2.0 SPDX，随附 Apache 全文 |
| ST STM32F4 CMSIS Device / HAL | CubeF4 1.28.3 包及组件原条款；包级文本见 [Package_license](LICENSES/ST-STM32CubeF4-1.28.3-Package_license.txt) |
| ST 启动文件与生成模板 | 保留启动文件内条款及模板版权；[Templates 原许可](LICENSES/ST-STM32CubeF4-Templates-LICENSE.txt)与[来源记录](docs/upstream-provenance.md)提供依据 |
| LibDriver MPU6500 | [固定修订 a66ceb7a](https://github.com/libdriver/mpu6500/tree/a66ceb7a5f18473f4807daebf0c63afdc077cc4f)，Copyright (c) 2015 - present LibDriver，作者 Shifeng Li；[原 MIT 许可](firmware/ThirdParty/LibDriverMPU6500/LICENSE)，[保留函数及哈希](firmware/ThirdParty/LibDriverMPU6500/provenance.json) |
| Mahony AHRS | [RoboMaster C 板固定修订 59d12b1](https://github.com/RoboMaster/Development-Board-C-Examples/tree/59d12b1adcd321dbf1f9e9166aef5eb95ab657bf/18.ins_task/components/algorithm)，原 SOH Madgwick 作者说明、2019 RoboMaster 授权与 [GPL-3.0-or-later 原文](firmware/ThirdParty/Mahony/LICENSE)；项目适配在源文件头标注 |
| DM 电机 CAD | [单独模型通知](mechanical/DM-MIT-NOTICE.md)记录具体文件与 MIT 授权来源 |

Keil、STM32CubeMX、编译器运行库、SolidWorks 和调试器工具不随本仓库分发，也不由本项目许可。仓库提供源文件，不分发本地编译固件。ST 模板生成链、实际依赖版本与 IMU 后端来源详见 [upstream-provenance.md](docs/upstream-provenance.md) 和 [imu-backend.md](docs/imu-backend.md)。
