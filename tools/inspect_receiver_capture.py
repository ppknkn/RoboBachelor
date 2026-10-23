#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Inspect an existing capture file offline. Never opens a serial/device port.

The selected protocol is a hypothesis, not receiver identification or evidence
that the radio link/failsafe works. Do not use this tool as an arming decision.
"""
import argparse
import json
from pathlib import Path
import re

def parse_hex(text):
    tokens = text.replace(',', ' ').split()
    if any(not re.fullmatch(r'(?:0[xX])?[0-9a-fA-F]{2}', t) for t in tokens):
        raise ValueError('Expected byte tokens only, e.g. 0F 01 FF; remove timestamps/labels')
    return bytes(int(t, 16) for t in tokens)

def decode_sbus_frame(frame):
    if len(frame) != 25 or frame[0] != 0x0F:
        raise ValueError('SBUS hypothesis requires a complete 25-byte frame with 0F header')
    if frame[24] != 0x00 and (frame[24] & 0x0F) != 0x04:
        raise ValueError('Unrecognised SBUS/SBUS2 footer')
    packed = int.from_bytes(frame[1:23], 'little')
    channels = [(packed >> (11 * i)) & 0x7FF for i in range(16)]
    return {
        'channels_raw': channels,
        'channels_minus_1024': [ch - 1024 for ch in channels],
        'ch17': bool(frame[23] & 0x01), 'ch18': bool(frame[23] & 0x02),
        'lost_frame': bool(frame[23] & 0x04), 'failsafe': bool(frame[23] & 0x08),
        'flags_hex': f'{frame[23]:02X}', 'footer_hex': f'{frame[24]:02X}',
        'legacy_sw1_from_ch5_low_bits': (frame[6] >> 4) & 3,
        'legacy_sw2_from_ch5_low_bits': (frame[6] >> 6) & 3,
    }

def sbus_candidates(data, maximum=12):
    found=[]
    offset=0
    while offset + 25 <= len(data) and len(found) < maximum:
        try:
            decoded=decode_sbus_frame(data[offset:offset+25])
        except ValueError:
            offset += 1
            continue
        found.append({'offset':offset, **decoded})
        offset += 25
    return found

def decode_dbus_frame(frame):
    if len(frame) != 18:
        raise ValueError('DBUS hypothesis requires an already aligned 18-byte frame')
    packed=int.from_bytes(frame[:6], 'little')
    channels=[(packed >> (11*i)) & 0x7FF for i in range(4)]
    return {
        'channels_raw':channels,
        'channels_minus_1024':[ch-1024 for ch in channels],
        'byte5_switch_high2':(frame[5] >> 6) & 3,
        'byte5_switch_low2':(frame[5] >> 4) & 3,
        'alignment_and_physical_switches_confirmed':False,
    }

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('capture', type=Path)
    parser.add_argument('--format', choices=['raw','hex'], required=True)
    parser.add_argument('--protocol', choices=['sbus','dbus'], required=True)
    parser.add_argument('--max-frames', type=int, default=12)
    args=parser.parse_args()
    if not 1 <= args.max_frames <= 1000:
        parser.error('--max-frames must be 1..1000')
    if args.capture.stat().st_size > 4*1024*1024:
        parser.error('Capture must be at most 4 MiB')
    try:
        data=(args.capture.read_bytes() if args.format=='raw'
              else parse_hex(args.capture.read_text(encoding='utf-8-sig')))
        if args.protocol=='sbus':
            frames=sbus_candidates(data,args.max_frames)
        else:
            if len(data)%18:
                raise ValueError('DBUS has no 0F framing here: supply pre-aligned 18-byte records')
            frames=[{'offset':i,**decode_dbus_frame(data[i:i+18])}
                    for i in range(0,min(len(data),args.max_frames*18),18)]
    except (ValueError, UnicodeError) as exc:
        parser.error(str(exc))
    print(json.dumps({'hypothesis':args.protocol,'bytes':len(data),'frames':frames,
                      'warning':'Offline layout inspection only; receiver mode, radio loss and physical STOP are not verified'},
                     ensure_ascii=False,indent=2))

if __name__=='__main__':main()
