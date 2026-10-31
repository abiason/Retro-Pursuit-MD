"""Check the SGDK 1.90 ROM header and its SDK-specific XOR checksum."""
from pathlib import Path
import hashlib
import struct
import sys

path = Path(sys.argv[1] if len(sys.argv) > 1 else 'out/rom.bin')
data = bytearray(path.read_bytes())
assert len(data) >= 512 and len(data) % 131072 == 0, 'Invalid ROM size'
assert data[0x100:0x110] == b'SEGA MEGA DRIVE ', 'Invalid console header'
assert data[0x120:0x150].rstrip() == b'RETRO PURSUIT', 'Invalid title'
stored = struct.unpack_from('>H', data, 0x18E)[0]
data[0x18E:0x190] = bytes(2)
checksum = 0
for (word,) in struct.iter_unpack('>H', data):
    checksum ^= word
assert checksum == stored, 'Invalid SGDK XOR checksum'
print(f'PASS: {len(data)} bytes; SGDK XOR checksum {stored:04X}')
print(f'SHA256: {hashlib.sha256(path.read_bytes()).hexdigest()}')
