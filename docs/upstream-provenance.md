# 第三方来源核实

## 完整源码IMU后端

旧无明确许可的A板实现已经移出公共分发，保留在作者本地归档。当前同名bsp_imu文件为新兼容适配，实际依赖MIT LibDriver raw/AUX子集和有GPL授权的C板Mahony代码，固定提交、许可与差异见[imu-backend.md](imu-backend.md)。不是给旧代码补标GPL，也不声称历史上从C板迁移。

## ST 工具链来源

Imu.ioc 记录 CubeMX 6.16.1 / DB.6.0.161 / STM32Cube FW_F4 V1.28.3。284 个候选 HAL/CMSIS 路径文件与本机该包字节一致；HAL 源版本宏为 1.8.5。

包级 [Package_license 副本](../LICENSES/ST-STM32CubeF4-1.28.3-Package_license.txt) 的 SBOM 明确列 HAL 为 BSD-3-Clause、CMSIS/Core/Device 为 Apache-2.0；不能把该包其他中间件的 SLA 条款套用到这些已明确列出的开源组件。对应原版权、子目录条款和完整文本保留。

生成的主程序/初始化文件保留 ST 版权。已安装 CubeMX 模板 tpl_main_c.ftl 从 license.tmp 引入声明，本机 BSD_License.ftl 产生的声明与候选文件头相同；包中基础 Templates 的 [LICENSE.txt](../LICENSES/ST-STM32CubeF4-Templates-LICENSE.txt) 给出 BSD-3-Clause 回退条款。[ST 官方社区对生成代码的说明](https://community.st.com/stm32cubeide-mcus-28/sources-from-st-licensed-as-is-what-does-it-mean-17807) 也说明保留 ST 版权并为自己的项目提供许可证。这里记录生成链和依据，不删除 ST 声明，也不把项目 GPL 当作对 ST 库的重新授权。未安装新工具或代用户接受新协议。

## 机械来源与可分离性

用户最新更正确认 JC4310尺寸.SLDPRT 为自己绘制，与 rline1050.SLDPRT、Robomaster_A_board.SLDPRT 一同列入原创 CERN-OHL-S 范围。DM-J4310-2EC-V1.1_3d_20260228_1.SLDPRT 则由用户向达妙官方确认 MIT，单独标注，未独立取得厂家原始版权通知。两份模型均已按原名和原二进制恢复。达妙产品可从[厂家公开资料入口](https://github.com/dmBots/dmBot)查找；JC 的开发者/销售方资料见其[快速入门](https://blog.csdn.net/loop222/article/details/142715259)。这些入口不是本地模型再分发许可证，也未证明本地几何就是对应下载版本。

之前的排除操作没有验证装配引用/嵌入几何可分离；恢复模型也不意味着它们已经通过验证。本机发现 SolidWorks 2025；只读自动化启动/依赖查询在正常宿主环境因 COM 类型库错误 TYPE_E_ELEMENTNOTFOUND 未完成，没有打开/保存/导出模型。当前无法证明删除它们不损坏总装，目前11文件已恢复，原装配引用字段不改，仍不声称完整装配已通过验证。

可行的下一步是在用户可用 CAD 会话中核对依赖、权属和嵌入几何：有明确再发布权则保留来源和许可；否则制作明确缺少厂商件的原创设计发布包及 sourcing 清单，或用有权授权的占位/自绘几何在副本中替换并复核装配。厂商来源入口须按实际本地文件确认，不猜测下载链接。不应发布未授权零件，也不应把删件包描述为完整可装配模型。
