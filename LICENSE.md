# 许可与署名入口

本页汇总文件的许可范围和来源入口。第三方文件保留各自版权与条款，项目许可不替代其原授权；完整许可原文位于 [LICENSES/](LICENSES/) 及对应组件目录。

| 文件或内容 | 适用许可 / 来源 |
| --- | --- |
| `firmware/Src/control/`、`firmware/Src/motor/` 的原创代码 | GPL-3.0-only |
| `firmware/MDK-ARM/bsp/` 的项目代码，包括 IMU 兼容接口与板级适配 | GPL-3.0-only；所调用的第三方实现单独列于下方 |
| `tools/`、`tests/`、项目原创配置和代码说明文档 | GPL-3.0-only；嵌入的第三方格式、引用与原声明仍按其原条款 |
| ST 生成文件的原创 USER CODE 部分 | GPL-3.0-only；ST 模板本身保留原版权与条款 |
| `firmware/ThirdParty/LibDriverMPU6500/` | MIT；LibDriver 原版权与通知保留 |
| `firmware/ThirdParty/Mahony/` | GPL-3.0-or-later；保留 RoboMaster 和 SOH Madgwick 作者信息及修改说明 |
| `firmware/Drivers/CMSIS/` | 保留 Arm / ST 的 Apache-2.0 等原声明，详见第三方通知 |
| `firmware/Drivers/STM32F4xx_HAL_Driver/` | 保留组件与包级许可；相应 BSD-3-Clause 条款见原文件及随附文本 |
| `firmware/MDK-ARM/startup_stm32f427xx.s` | 文件内 ST 的完整 BSD-3-Clause 风格条款 |
| 其他 ST 生成模板 | 保留原版权与声明；[来源说明](docs/upstream-provenance.md)记录模板与许可依据 |
| 作者原创机械设计 | CERN-OHL-S-2.0；逐文件范围见[机械许可范围](mechanical/LICENSE_SCOPE.md) |
| DM-J4310-2EC 电机模型 | MIT；授权信息来源见[模型通知](mechanical/DM-MIT-NOTICE.md) |
| 装配内的第三方数据 | 按实际原授权，不因项目范围表而变为原创内容 |

[GPL-3.0-only](LICENSES/GPL-3.0-only.txt) · [CERN-OHL-S-2.0](LICENSES/CERN-OHL-S-2.0.txt) · [Apache-2.0](LICENSES/Apache-2.0.txt) · [BSD-3-Clause](LICENSES/BSD-3-Clause.txt) · [第三方来源与通知](THIRD_PARTY_NOTICES.md)

请保留各文件既有版权、作者信息、修改说明和适用许可。照片与视频的素材来源见[媒体来源记录](docs/media-sources.md)；机械格式与使用说明见[机械文档](docs/mechanical-notes.md)。
