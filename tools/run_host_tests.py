#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Compile and run host-only C assertions; mock CAN, no device access."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cc', default='gcc')
    parser.add_argument('--output', type=Path, default=Path('build/host-tests'))
    args=parser.parse_args()
    cc=shutil.which(args.cc)
    if not cc: parser.error('Native GCC-compatible C compiler required; not ARM GCC')
    root=Path(__file__).resolve().parents[1]
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    cases={
        'legacy_stop':['tests/test_legacy_stop.c','firmware/Src/control/state_estimator.c'],
        'imu_port':['tests/test_imu_port.c','firmware/MDK-ARM/bsp/imu_port.c'],
        'imu_backend':['tests/test_imu_backend.c','firmware/MDK-ARM/bsp/bsp_imu.c','firmware/MDK-ARM/bsp/imu_ist8310_aux.c','firmware/ThirdParty/LibDriverMPU6500/driver_mpu6500.c','firmware/ThirdParty/Mahony/MahonyAHRS.c'],
        'motor_protocol':['tests/test_motor_protocol.c','firmware/Src/motor/dm4310.c',
                          'firmware/Src/motor/jc_motor.c','firmware/MDK-ARM/bsp/bsp_can.c'],
    }
    flags=['-std=c99','-Wall','-Wextra','-Werror']
    for inc in ['tests/stubs','firmware/Src/control','firmware/Src/motor','firmware/MDK-ARM/bsp','firmware/ThirdParty/LibDriverMPU6500','firmware/ThirdParty/Mahony']:
        flags+=['-I',str(root/inc)]
    for name,sources in cases.items():
        exe=out/(name+('.exe' if os.name=='nt' else ''))
        subprocess.run([cc,*flags,*[str(root/p) for p in sources],'-o',str(exe),'-lm'],check=True)
        subprocess.run([str(exe)],check=True)
    subprocess.run([sys.executable,'-B','-m','unittest','discover','-s',str(root/'tests'),
                    '-p','test_receiver_capture.py'],check=True)
    print('Host checks passed. No firmware was flashed or run on a device.')

if __name__=='__main__':main()
