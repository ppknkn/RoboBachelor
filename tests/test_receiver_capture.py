# SPDX-License-Identifier: GPL-3.0-only
"""Golden wire-bit fixtures for the offline inspector, no device access."""
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import inspect_receiver_capture as capture

class CaptureTests(unittest.TestCase):
    def golden(self):
        # Independent bit fixture: CH1=max (payload bits0..10), CH5=1
        # (payload bit44), CH16=max (payload bits165..175), flags lost+failsafe.
        return bytes.fromhex('0F FF 07 00 00 00 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 FF 0C 00')

    def test_channel_boundaries_and_flags(self):
        d=capture.decode_sbus_frame(self.golden())
        self.assertEqual(d['channels_raw'],[2047,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2047])
        self.assertTrue(d['lost_frame']);self.assertTrue(d['failsafe'])
        self.assertFalse(d['ch17']);self.assertFalse(d['ch18'])

    def test_legacy_switch_is_channel_data(self):
        frame=bytearray(self.golden())
        frame[6]=0x00
        a=capture.decode_sbus_frame(frame)
        frame[6]=0x10
        b=capture.decode_sbus_frame(frame)
        self.assertEqual((a['channels_raw'][4],a['legacy_sw1_from_ch5_low_bits']),(0,0))
        self.assertEqual((b['channels_raw'][4],b['legacy_sw1_from_ch5_low_bits']),(1,1))

    def test_reject_incomplete_and_bad_footer(self):
        with self.assertRaises(ValueError):capture.decode_sbus_frame(self.golden()[:-1])
        with self.assertRaises(ValueError):capture.decode_sbus_frame(self.golden()[:-1]+b'\xFF')

    def test_resynchronise_without_inventing_missing_bytes(self):
        noise=b'\x99\x0F'+b'\xAA'*27
        found=capture.sbus_candidates(noise+self.golden()+self.golden())
        self.assertEqual([f['offset'] for f in found],[len(noise),len(noise)+25])
        self.assertEqual(capture.sbus_candidates(self.golden()[:20]),[])

    def test_dbus_has_different_offset_and_switch_fields(self):
        frame=bytes.fromhex('FF 07 00 00 00 D0 00 00 00 00 00 00 00 00 00 00 00 00')
        d=capture.decode_dbus_frame(frame)
        self.assertEqual(d['channels_raw'],[2047,0,0,0])
        self.assertEqual((d['byte5_switch_high2'],d['byte5_switch_low2']),(3,1))
        self.assertFalse(d['alignment_and_physical_switches_confirmed'])

    def test_strict_hex_input(self):
        self.assertEqual(capture.parse_hex('0F, 0x01 FF'),b'\x0f\x01\xff')
        with self.assertRaises(ValueError):capture.parse_hex('12:30 RX 0F 01')

if __name__=='__main__':unittest.main()
