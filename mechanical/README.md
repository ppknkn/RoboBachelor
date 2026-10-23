[简体中文](README.md) | [English](README.en.md)

# 机械模型

这里提供 RoboBachelor 的机身、腿部、传动和轮端模型：`native/` 中有 **2 个 SolidWorks 装配文件和 9 个零件文件**。

![单侧相似连杆腿的结构与几何布局。](../media/leg-mechanism.png)

## 从总装开始

1. 将整个 `mechanical/` 目录保存到本地，保持 `native/` 内的文件名和相对位置。
2. 在 SolidWorks 中打开 [!总装_tiny.SLDASM](native/!总装_tiny.SLDASM)，从整机查看部件关系；单看腿部时打开[右腿装配](native/串联右腿260406-tiny.SLDASM)。
3. 进入对应零件编辑尺寸或结构。另存修改版时，在 CAD 内管理装配引用；编辑前先确认文档单位。

## 文件导航

| 文件 | 用途 |
| --- | --- |
| [!总装_tiny.SLDASM](native/!总装_tiny.SLDASM) | 整机装配入口 |
| [串联右腿260406-tiny.SLDASM](native/串联右腿260406-tiny.SLDASM) | 右腿装配 |
| [车架260412_tiny.SLDPRT](native/车架260412_tiny.SLDPRT) | 车架零件 |
| [串联腿_260406-tiny.SLDPRT](native/串联腿_260406-tiny.SLDPRT) | 腿部零件 |
| [电机架260412_tiny.SLDPRT](native/电机架260412_tiny.SLDPRT) | 电机安装零件 |
| [轮.SLDPRT](native/轮.SLDPRT) | 轮零件 |
| [主动齿轮_tiny.SLDPRT](native/主动齿轮_tiny.SLDPRT) | 主动齿轮 |
| [DM-J4310-2EC-V1.1_3d_20260228_1.SLDPRT](native/DM-J4310-2EC-V1.1_3d_20260228_1.SLDPRT) | DM 电机外形模型 |
| [JC4310尺寸.SLDPRT](native/JC4310尺寸.SLDPRT) | JC 电机外形模型 |
| [rline1050.SLDPRT](native/rline1050.SLDPRT) | rline1050 组件模型 |
| [Robomaster_A_board.SLDPRT](native/Robomaster_A_board.SLDPRT) | RoboMaster A 板外形模型 |

`.SLDASM` 是装配文件，`.SLDPRT` 是零件文件。[native-manifest.json](native-manifest.json) 提供文件大小与 SHA-256，便于核对下载内容。

## 继续阅读

[装配、格式与制造资料说明](../docs/mechanical-notes.md) · [统一许可与来源](../LICENSE.md) · [项目介绍](../README.md)
