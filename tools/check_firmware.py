#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Compile project C translation units with ARM GCC; never link, flash or run.

This is an additional portability/static check, not a Keil firmware build.
"""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import xml.etree.ElementTree as ET

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--firmware', type=Path, default=Path(__file__).resolve().parents[1] / 'firmware')
    parser.add_argument('--output', type=Path, default=Path('build/gcc-check'))
    parser.add_argument('--cc', default='arm-none-eabi-gcc')
    args = parser.parse_args()
    compiler = shutil.which(args.cc)
    if not compiler:
        parser.error('ARM GCC not found; install it separately or pass --cc')
    fw = args.firmware.resolve()
    project = fw / 'MDK-ARM/Imu.uvprojx'
    tree = ET.parse(project)
    target = tree.find('./Targets/Target')
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    includes = target.findtext('./TargetOption/TargetArmAds/Cads/VariousControls/IncludePath').split(';')
    defines = target.findtext('./TargetOption/TargetArmAds/Cads/VariousControls/Define').split(',')
    flags = ['-mcpu=cortex-m4', '-mthumb', '-mfpu=fpv4-sp-d16', '-mfloat-abi=hard',
             '-std=c99', '-fshort-enums', '-O2', '-Wall', '-Wextra', '-c']
    for inc in includes:
        flags += ['-I', str((project.parent / inc.replace('\\', '/')).resolve())]
    flags += ['-D' + d.strip() for d in defines if d.strip()]
    version = subprocess.run([compiler, '--version'], capture_output=True, text=True).stdout.splitlines()[0]
    results = []
    log = [version, 'C object compilation only; ARMCC startup assembly and final link are not checked.']
    for node in target.findall('./Groups/Group/Files/File'):
        rel = node.findtext('FilePath').replace('\\', '/')
        if not rel.lower().endswith('.c'):
            continue
        source = (project.parent / rel).resolve()
        if not source.is_relative_to(fw):
            raise ValueError(f'Project reference outside firmware: {rel}')
        label = source.relative_to(fw).as_posix()
        obj = output / (label.replace('/', '__') + '.o')
        proc = subprocess.run([compiler, *flags, str(source), '-o', str(obj)], cwd=fw,
                              capture_output=True, text=True, errors='replace')
        log += [label, proc.stdout, proc.stderr]
        results.append({'source': label, 'exit_code': proc.returncode,
                        'warnings': proc.stderr.count('warning:'), 'errors': proc.stderr.count('error:')})
    (output / 'compile.log').write_text('\n'.join(log), encoding='utf-8')
    summary = {'compiler': version, 'scope': 'C objects only; no startup assembly, link, flash or hardware test',
               'translation_units': len(results), 'failed': sum(r['exit_code'] != 0 for r in results),
               'warnings': sum(r['warnings'] for r in results), 'results': results}
    (output / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    print(json.dumps({k:v for k,v in summary.items() if k != 'results'}, indent=2))
    return int(summary['failed'] != 0)

if __name__ == '__main__':
    raise SystemExit(main())
