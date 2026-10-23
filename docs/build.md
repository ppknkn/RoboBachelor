# 构建与下载说明

## Keil 主工程

1. 自行安装并配置有权使用的 Keil MDK/uVision 和 ARM Compiler 5.06 update 7。工程指定 STM32F427IIHx，DFP 为 `Keil.STM32F4xx_DFP.2.15.0`，原 RTE 元数据还引用 `ARM.CMSIS.4.3.0 / CORE 4.1.0`。本轮成功构建实际解析到 `ARM.CMSIS.6.3.0 / CORE 6.2.0`，DFP 为 2.15.0。需关注 pack 解析版本漂移；本次没有安装新工具、替用户接受安装协议，也没有验证一个全新的 pack 环境。
2. 打开 `firmware/MDK-ARM/Imu.uvprojx`，选择目标 `Imu`。检查器件包解析和 include 顺序，再执行 Rebuild。保留工程名 Imu 以避免破坏已有相对路径；发布项目名是 RoboBachelor。
3. 原启动文件是 ARMCC 汇编格式，工程使用 MicroLIB。不要直接把这个工程描述为可用 GCC 链接的工程，也不要将 ARM GCC 检查目标文件拿去刷写。

在已具备可运行 Keil 环境的 Windows 上，可用以下命令仅重建：

```powershell
& '<Keil安装目录>\UV4\UV4.exe' -r 'firmware\MDK-ARM\Imu.uvprojx' -t 'Imu' -j0 -o 'build-keil.log'
```

查看实际编译及链接日志。本轮独立副本使用现有MDK Plus5.34 / ARMCC5.06u7，38C+启动汇编+链接，0错误/0警告；Code32360、RO652、RW184、ZI3712字节。工程未生成HEX，链接产物为 `firmware/MDK-ARM/Imu/Imu.axf`，本仓库不分发本地产物。

CubeMX 文件用于记录外设配置，不代表重新生成不会覆盖手写改动。重新生成前另存整个工程，并核对 USER CODE 区域、外设初始化和工程文件列表。

## ARM GCC 附加编译检查

Python 3.9+ 与可独立运行的 `arm-none-eabi-gcc` 需要自行安装。在仓库根目录运行（另可用本机 GCC 执行 `python tools/run_host_tests.py`，这是IMU/端口/原STOP/电机协议等离线测试）：

```powershell
python tools/check_firmware.py
```

脚本从 uVision 工程读取 C 文件、include 和宏，将 C 文件编译为 `build/gcc-check/*.o`，生成 `compile.log` 与 `summary.json`。它不会运行源码、连接调试器、刷写或触发电机。它不处理 ARMCC 启动汇编、不重现所有 Keil 单文件编译选项、不链接，因此不能代替 Keil 构建、RAM/Flash 布局验证或实物测试。

## 下载（由操作者另行执行，本轮未执行）

原工程配置指向 ST-Link。先按[上电步骤](bring-up.md)断开动力、核对实际调试接口、电压和接地，再使用本地有效 Keil 环境编译自己的 AXF，检查器件、Flash 地址及下载算法。当前资料没有经过核实的调试连接器针脚图，因此这里不提供猜测的接线或一键刷写命令。连接后不要自动进入运行态。本版本沿原启动流程在IMU初始化及收敛后请求使能，下载/检查时必须断开动力；无ARM命令和永久禁能宏。原STOP及HOLD限制见软件边界。
