[简体中文](README.md) | [English](README.en.md)

# Mechanical models

This directory contains RoboBachelor's chassis, legs, transmission and wheel-end models. `native/` holds **two SolidWorks assemblies and nine part files**.

![Structure and geometric layout of a single similar-linkage leg.](../media/leg-mechanism.png)

## Start with the assembly

1. Save the whole `mechanical/` directory locally, keeping filenames and relative locations inside `native/` together.
2. Open [!总装_tiny.SLDASM](native/!总装_tiny.SLDASM) in SolidWorks to explore the robot's components. For a leg-only view, open the [right-leg assembly](native/串联右腿260406-tiny.SLDASM).
3. Edit dimensions or geometry in the relevant part. Manage assembly references inside CAD when saving a modified version, and check document units before editing.

## File guide

| File | Purpose |
| --- | --- |
| [!总装_tiny.SLDASM](native/!总装_tiny.SLDASM) | Full robot assembly entry |
| [串联右腿260406-tiny.SLDASM](native/串联右腿260406-tiny.SLDASM) | Right-leg assembly |
| [车架260412_tiny.SLDPRT](native/车架260412_tiny.SLDPRT) | Chassis part |
| [串联腿_260406-tiny.SLDPRT](native/串联腿_260406-tiny.SLDPRT) | Leg part |
| [电机架260412_tiny.SLDPRT](native/电机架260412_tiny.SLDPRT) | Motor mount |
| [轮.SLDPRT](native/轮.SLDPRT) | Wheel part |
| [主动齿轮_tiny.SLDPRT](native/主动齿轮_tiny.SLDPRT) | Drive gear |
| [DM-J4310-2EC-V1.1_3d_20260228_1.SLDPRT](native/DM-J4310-2EC-V1.1_3d_20260228_1.SLDPRT) | DM motor geometry |
| [JC4310尺寸.SLDPRT](native/JC4310尺寸.SLDPRT) | JC motor geometry |
| [rline1050.SLDPRT](native/rline1050.SLDPRT) | rline1050 component model |
| [Robomaster_A_board.SLDPRT](native/Robomaster_A_board.SLDPRT) | RoboMaster A-board geometry |

`.SLDASM` files are assemblies; `.SLDPRT` files are parts. [native-manifest.json](native-manifest.json) records file sizes and SHA-256 hashes for checking downloads.

## Further reading

[Assembly, format and manufacturing notes](../docs/mechanical-notes.md) · [License and sources](../LICENSE.md) · [Project overview](../README.en.md)
