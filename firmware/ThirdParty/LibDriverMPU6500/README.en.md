[简体中文](README.md) | [English](README.en.md)

# MPU6500 raw-register and auxiliary-bus interface

This directory integrates the initialization, raw-register transport and AUX I2C configuration functions used from [LibDriver MPU6500](https://github.com/libdriver/mpu6500/tree/a66ceb7a5f18473f4807daebf0c63afdc077cc4f). Pinned revision: `a66ceb7a5f18473f4807daebf0c63afdc077cc4f`.

| File | Purpose |
| --- | --- |
| [driver_mpu6500.c](driver_mpu6500.c) | The initialization, register-transport and AUX subset used here; retained function bodies follow upstream |
| [driver_mpu6500.h](driver_mpu6500.h) | The unmodified upstream interface header |
| [driver_mpu6500_registers.h](driver_mpu6500_registers.h) | Register definitions separated from the upstream implementation, retaining its notice |
| [provenance.json](provenance.json) | Upstream revision, file hashes and retained-function list |
| [LICENSE](LICENSE) | The full upstream license and copyright notice |

## Project integration

[imu_port.c](../../MDK-ARM/bsp/imu_port.c) connects the interface to SPI5 and PF6 chip select. [imu_ist8310_aux.c](../../MDK-ARM/bsp/imu_ist8310_aux.c) accesses the magnetometer through MPU6500's auxiliary bus.

The compiled raw-register/AUX subset does not require DMP firmware. The header retains the complete upstream declarations; check `driver_mpu6500.c` and the retained-function list before calling additional APIs.

[IMU backend guide](../../../docs/imu-backend.md) · [License and attribution](../../../LICENSE.md)
