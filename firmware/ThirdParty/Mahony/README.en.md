[简体中文](README.md) | [English](README.en.md)

# Mahony attitude fusion

This directory integrates and adapts the [Mahony implementation in RoboMaster's C-board examples](https://github.com/RoboMaster/Development-Board-C-Examples/tree/59d12b1adcd321dbf1f9e9166aef5eb95ab657bf/18.ins_task/components/algorithm), retaining the original SOH Madgwick author information and RoboMaster's grant. Pinned revision: `59d12b1adcd321dbf1f9e9166aef5eb95ab657bf`.

| File | Purpose |
| --- | --- |
| [MahonyAHRS.c](MahonyAHRS.c) | Quaternion updates with gyroscope, accelerometer and magnetic feedback |
| [MahonyAHRS.h](MahonyAHRS.h) | Update, sample-period and reset interfaces |
| [LICENSE](LICENSE) | The full upstream license and copyright notice |

## Project use

[bsp_imu.c](../../MDK-ARM/bsp/bsp_imu.c) adapts sensor data and output coordinates. It calls `MahonyReset` during initialization, supplies the actual interval through `MahonySetPeriod`, then calls `MahonyAHRSupdate`.

Project adaptations include an explicit sample interval, yaw-only magnetic feedback, integral clamping, `sqrtf` normalization and reset interfaces. Changes are recorded in the source header. See the [IMU backend guide](../../../docs/imu-backend.md) for mathematical and interface details and [test_imu_backend.c](../../../tests/test_imu_backend.c) for offline cases.

[License and attribution](../../../LICENSE.md)
