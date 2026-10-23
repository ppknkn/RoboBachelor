# 许可证范围

本项目不存在覆盖所有文件的统一许可证。以下许可仅涉及项目作者有权授权的原创内容；第三方作品、模板、工具链及厂商零件保持各自许可。完整条款存于 LICENSES；本说明不改写条款。

| 路径或内容 | 许可 / 状态 |
| --- | --- |
| `firmware/Src/control/*`、`firmware/Src/motor/*` | 原创实现 GPL-3.0-only |
| `firmware/MDK-ARM/bsp/bsp_can.[ch]`、`bsp_dbus.[ch]`、`bsp_tune.[ch]` | 原创实现 GPL-3.0-only |
| `tools/*.py`、`tests/*` 及测试桩 | GPL-3.0-only |
| 项目原创配置及原创代码说明文档 | GPL-3.0-only；嵌入的第三方格式、引用和声明不被重新授权 |
| ST 生成文件的项目原创 USER CODE 部分 | GPL-3.0-only；原 ST 模板版权和许可声明保留，不能按此表重新授权 ST 模板 |
| `firmware/Drivers/CMSIS/*` | 保留随附 Apache-2.0 等原声明；见第三方通知 |
| `firmware/Drivers/STM32F4xx_HAL_Driver/*` | 随附组件许可说明指出无 Package_license 时适用 BSD-3-Clause；保留原说明 |
| 新 `firmware/MDK-ARM/bsp/bsp_imu.[ch]`、`imu_port.[ch]`、`imu_ist8310_aux.[ch]` | 新兼容门面/板级适配 GPL-3.0-only，旧未许可A板实现及辅助头已移出分发 |
| `firmware/ThirdParty/LibDriverMPU6500/*` | MIT，Copyright (c) 2015 - present LibDriver；保留原通知，明确列出删减范围与固定提交 |
| `firmware/ThirdParty/Mahony/*` | GPL-3.0-or-later，保留RoboMaster及SOH Madgwick通知，实际C板源码改造，修改说明在文件头 |
| `firmware/MDK-ARM/startup_stm32f427xx.s` | 文件内完整 ST BSD-3-Clause 风格条款，保留原版权与原声明 |
| 其他 ST 生成模板 | 保留原版权与声明；生成链、基础 Templates 的 BSD 回退条款和 ST 官方社区说明已记录于 docs/upstream-provenance.md |
| 原创机械设计 | CERN-OHL-S-2.0，具体文件见 `mechanical/LICENSE_SCOPE.md` |
| DM-J4310-2EC 模型 | MIT，依据作者向达妙官方确认；原始版权通知待补，见 mechanical/DM-MIT-NOTICE.md |
| 装配内其他第三方数据 | 不因本项目声明被重新授权；见模型范围表 |

[GPL-3.0-only 全文](LICENSES/GPL-3.0-only.txt) · [CERN-OHL-S-2.0 全文](LICENSES/CERN-OHL-S-2.0.txt) · [Apache-2.0 全文](LICENSES/Apache-2.0.txt) · [BSD-3-Clause 全文](LICENSES/BSD-3-Clause.txt)

版权署名使用各原文件既有声明和实际贡献者身份；未凭空填写个人姓名或把厂商版权改成项目作者。DM模型许可依据作者向厂家确认，其原始通知未独立取得，限制如实记录；模型可交换格式/装配验证尚未完成。完整源码后端与实际依赖见 docs/imu-backend.md。
