[简体中文](README.md) | [English](README.en.md)

# Mahony 姿态融合

本目录集成并适配 [RoboMaster C 板示例中的 Mahony 实现](https://github.com/RoboMaster/Development-Board-C-Examples/tree/59d12b1adcd321dbf1f9e9166aef5eb95ab657bf/18.ins_task/components/algorithm)，保留 SOH Madgwick 的原作者说明和 RoboMaster 的授权通知。固定版本：`59d12b1adcd321dbf1f9e9166aef5eb95ab657bf`。

| 文件 | 用途 |
| --- | --- |
| [MahonyAHRS.c](MahonyAHRS.c) | 四元数更新与陀螺、加速度、磁场反馈 |
| [MahonyAHRS.h](MahonyAHRS.h) | 更新、采样周期和复位接口 |
| [LICENSE](LICENSE) | 上游完整许可与版权通知 |

## 在项目中使用

[bsp_imu.c](../../MDK-ARM/bsp/bsp_imu.c) 负责传感器数据与输出坐标的适配：初始化时调用 `MahonyReset`，更新前通过 `MahonySetPeriod` 提供实际时间间隔，再调用 `MahonyAHRSupdate`。

项目适配包括显式采样间隔、仅偏航的磁反馈、积分限幅、`sqrtf` 归一化与复位接口。修改点记录在源文件头；数学和接口说明见 [IMU 后端文档](../../../docs/imu-backend.md)，离线用例见 [test_imu_backend.c](../../../tests/test_imu_backend.c)。

[统一许可与署名](../../../LICENSE.md)
