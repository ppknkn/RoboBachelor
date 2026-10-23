# 第三方来源核实

## 完整源码IMU后端

旧无明确许可的A板实现已经移出公共分发，保留在作者本地归档。当前同名bsp_imu文件为新兼容适配，实际依赖MIT LibDriver raw/AUX子集和有GPL授权的C板Mahony代码，固定提交、许可与差异见[imu-backend.md](imu-backend.md)。不是给旧代码补标GPL，也不声称历史上从C板迁移。

## ST 工具链来源

Imu.ioc 记录 CubeMX 6.16.1 / DB.6.0.161 / STM32Cube FW_F4 V1.28.3。284 个候选 HAL/CMSIS 路径文件与本机该包字节一致；HAL 源版本宏为 1.8.5。

包级 [Package_license 副本](../LICENSES/ST-STM32CubeF4-1.28.3-Package_license.txt) 的 SBOM 明确列 HAL 为 BSD-3-Clause、CMSIS/Core/Device 为 Apache-2.0；不能把该包其他中间件的 SLA 条款套用到这些已明确列出的开源组件。对应原版权、子目录条款和完整文本保留。

生成的主程序/初始化文件保留 ST 版权。已安装 CubeMX 模板 tpl_main_c.ftl 从 license.tmp 引入声明，本机 BSD_License.ftl 产生的声明与候选文件头相同；包中基础 Templates 的 [LICENSE.txt](../LICENSES/ST-STM32CubeF4-Templates-LICENSE.txt) 给出 BSD-3-Clause 回退条款。[ST 官方社区对生成代码的说明](https://community.st.com/stm32cubeide-mcus-28/sources-from-st-licensed-as-is-what-does-it-mean-17807) 也说明保留 ST 版权并为自己的项目提供许可证。这里记录生成链和依据，不删除 ST 声明，也不把项目 GPL 当作对 ST 库的重新授权。未安装新工具或代用户接受新协议。

## 机械来源与格式

机械文件的授权信息集中在 [LICENSE.md](../LICENSE.md)、[机械许可范围](../mechanical/LICENSE_SCOPE.md)和 [DM 模型通知](../mechanical/DM-MIT-NOTICE.md)。装配入口与零件用途见[模型导航](../mechanical/README.md)，格式、依赖与检查边界见[机械说明](mechanical-notes.md)。
