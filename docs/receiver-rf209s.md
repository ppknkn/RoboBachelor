# RF209S 接收机核实记录

用户已确认接收机为天地飞 WFLY RF209S，信号线接 CH9，失控保护设为 HOLD；实际端口模式与停止开关通道未知。以下资料不能替代实际端口配置或实物测试，本版按作者要求保留原STOP逻辑，不新增遥控失联保护。

## 厂家资料确认的范围

[天地飞 RF209S 技术参数](https://www.wflysz.com/control/13.html?type=%E6%8A%80%E6%9C%AF%E5%8F%82%E6%95%B0) 列明 W.BUS 兼容 S.BUS，同时支持 PWM、PPM、可配置端口和预设/保持/关闭失控保护。当前网页把 W.BUS2 描述为非标准 S.BUS、频率不同。

[厂家两页中文简易说明书](https://www.wflysz.com/static/upload/file/20200903/1599115279962725.pdf) 的端口图显示：CH9 可以配置 W.BUS2/W.BUS/PPM/PWM，CH8 可以配置 W.BUS/PPM/PWM；正常工作指示灯绿色为 W.BUS，紫色为 PWM，蓝色为 PPM。其较早版参数页又把 W.BUS2 写为遥测传感器输入。网页与说明书的 W.BUS2 描述存在版本差异，不能假设所接端口就是标准 S.BUS 输出。

该说明书只说明支持失控保护，没有给出字节级 W.BUS 失效标志规范。不能据“支持失控保护”推断收到持续帧就代表无线链路正常；预设/保持模式可能继续提供通道值。

## 当前代码的具体冲突

以收到的 0x0F 为 p[0]，当前 bsp_dbus.c：

| 项目 | 现代码 | 标准协议对照 |
| --- | --- | --- |
| 帧界定 | 寻找 0x0F，最少余下 18 字节就接受候选 | 标准 SBUS 为 25 字节；DJI 官方 DBUS 例程没有这一个起始字节偏移 |
| 四通道 | 由 p[1] 开始连续解包 11 bit，中心减 1024 | 与 SBUS 前四模拟通道的布局一致 |
| sw1/sw2 | 从 p[6] 高 4 bit 各取 2 bit | 在标准 SBUS 中这 4 bit 是第 5 模拟通道的低 4 bit，不是两个独立拨杆 |
| 失效标志 | 未读取完整帧末部或 lost-frame/failsafe | 常见标准 SBUS 实现在 byte 23 读取 bit2/bit3；需要针对 RF209S 实际模式核对 |
| 串口 | 100000，HAL WordLength=8B，Even parity，2 stop | STM32F4 HAL 的 parity 占 word 的最高位，此配置实际仅有 7 个数据位；标准 SBUS 需要 8 数据位加 parity，应核对 9B+Even 及实际反相电路 |

SBUS 对照依据为实现作者维护的 [Bolder Flight SBUS](https://github.com/bolderflight/sbus) 及其源文件；DBUS 对照为 [RoboMaster 官方 RemoteControl 例程](https://github.com/RoboMaster/DevelopmentBoard-Examples/blob/ce2cc57325effa15b7f53c2b82119723ba4abd16/RemoteControl/MDK-ARM/bsp/bsp_uart.c)。这些只是协议布局对照，不是本硬件接收成功的证据。没有把 DJI 原始实现复制进新解析器。

## HOLD 与停止开关

HOLD 可能持续输出保留的通道帧，所以“超过 100 ms 没有新串口帧”只检测串口帧停更，不能证明检测到无线失联。RF209S 是否仍在这种模式正确置 SBUS lost-frame/failsafe 标志，需要实际完整帧验证。不得将当前超时门控描述为已完成 RF 失联保护；本版不新增遥控失联保护。

原版 robot_logic_update 只在 g_rc.sw1==0 时进入 ROBOT_ESTOP；头文件却注释拨杆值为1/2/3，二者并不一致。sw2未参与停止决策。按 SBUS 假设，sw1取的是CH5最低两位，用户记得“拨某个拨杆会STOP”不足以证明已正确解码该开关。另一个原停机条件是机体倾角超过1 rad，和遥控开关无关。

## 最小仍需确认的实物信息

先只查看发射机的接收机端口设置，确认 CH9 当前显示 W.BUS、W.BUS2、PPM 还是 PWM，不要为排查而直接改配置。若暂时找不到菜单，正常连接时的指示灯颜色可作线索（绿色常亮对应说明书的W.BUS工作模式），但不替代可自定义CH9的实际设置。随后再核实发射机中哪个通道对应停止开关。主控信号路径还需确认是否有反相/转换电路，不能仅从 MCU PB7 代码推出连接器接线。

收到这些信息后，再决定解析后端及 UART 配置，测试整帧、错帧、丢帧、failsafe、停止开关和断联恢复。若确认 W.BUS，再在动力电源物理断开条件下记录停止/运行拨杆各一组完整原始帧，以及关闭发射机后 HOLD 状态的完整帧，比较通道与失效标志；记录应保留完整字节，避免被现有7数据位UART配置截断。厂商资料中的“关闭”不应未经核实就等同于无脉冲或无帧输出。本轮未改变解析器或UART配置；原启动请求使能已恢复，未新增解锁/超时门控。

## 发射机暂时没电时能做什么

可以先运行离线测试和检查已有记录。项目目录中的现有日志是编译日志，没有找到能确定 CH9 设置或 STOP 通道的真实原始帧；源码注释和通用接收机能力也不能恢复存储在接收机/发射机中的实际设置。接收机无信号时的红灯不能证明端口模式，不要为了查看而按 SET 进入模式切换。

新增 `tools/inspect_receiver_capture.py` **只读字节文件，不打开串口、USB、CAN 或其他设备**。它要求人工选择协议假设，输出通道和标志；结果不识别实际接收机，也不控制电机。

```powershell
# 现在即可运行的固定字节离线测试，无需发射机或接收机供电
python -B -m unittest discover -s tests -p test_receiver_capture.py

# 日后已有采集文件时检查；hex 文件只允许 0F 01 FF 这样的字节，不含时间戳
python tools/inspect_receiver_capture.py capture.hex --format hex --protocol sbus
# DBUS 假设要求事先对齐的18字节记录，不会凭0F猜帧边界
python tools/inspect_receiver_capture.py capture.bin --format raw --protocol dbus
```

工具固定帧测试覆盖位边界、完整帧要求、重同步、失效标志读取和原 sw1 实际取 CH5 低位的问题；测试向量是人工构造的，**不是 RF209S 实测记录**。

日后进行实际采集时，先物理断开电机动力，不要运行会请求使能的固件。用已有的合适记录设备捕获 CH9 的完整原始字节，并保留采样配置；若确认标准 W.BUS/SBUS，应核对100000、8数据位、Even、2 stop及真实反相路径，不使用现有7数据位的截断结果下结论。依次记录“已知停止档”“另一档”“关闭发射机后HOLD”，比较完整帧中的通道和失效标志。若无发射机时接收机仍输出帧，单独捕获可帮助判断帧布局，但不足以确认停止通道或失效标志的前后变化。未验证之前不能称 RF 失联保护完成，不据此声称修复了接收机协议。
