"""Package reviewed sources and the verified ROM; never bundle SDK or test ROMs."""
from pathlib import Path
import hashlib
import json
import shutil
import subprocess
import sys
import zipfile

root = Path(__file__).resolve().parents[1]
release = root / 'dist' / 'Retro-Pursuit-1.0'
receipt = json.loads((root / 'out/release-build.json').read_text(encoding='utf-8-sig'))
rom = root / 'out/rom.bin'
digest = hashlib.sha256(rom.read_bytes()).hexdigest()
if receipt['clean_builds'] != 2 or receipt['sha256'] != digest:
    raise SystemExit('Run tools/release.ps1 successfully before packaging')
subprocess.run([sys.executable, str(root / 'tools/verify_rom.py'), str(rom)], check=True)
release.mkdir(parents=True, exist_ok=False)
for name in ('src', 'tests', 'docs', 'tools'):
    shutil.copytree(root / name, release / name,
                    ignore=shutil.ignore_patterns('__pycache__', '*.pyc', '*.exe', 'emulator'))
for name in ('README.md', 'Makefile', '.gitignore'):
    shutil.copy2(root / name, release / name)
shutil.copy2(rom, release / 'Retro-Pursuit-1.0.bin')
shutil.copy2(root / 'out/release-build.json', release / 'release-build.json')
for index in (1, 2):
    shutil.copy2(root / f'out/release-build-{index}.log', release / f'build-{index}.log')
files = sorted(p for p in release.rglob('*') if p.is_file())
manifest = ''.join(f'{hashlib.sha256(p.read_bytes()).hexdigest()}  {p.relative_to(release).as_posix()}\n'
                   for p in files)
(release / 'SHA256SUMS.txt').write_text(manifest, encoding='utf-8')
archive = release.parent / (release.name + '.zip')
with zipfile.ZipFile(archive, 'x', compression=zipfile.ZIP_DEFLATED) as output:
    for path in sorted(p for p in release.rglob('*') if p.is_file()):
        output.write(path, path.relative_to(release.parent))
with zipfile.ZipFile(archive) as check:
    if check.testzip() is not None:
        raise SystemExit('ZIP integrity failure')
    for line in manifest.splitlines():
        expected, name = line.split('  ', 1)
        if hashlib.sha256(check.read(f'{release.name}/{name}')).hexdigest() != expected:
            raise SystemExit(f'Packaged hash mismatch: {name}')
print(f'PASS: {len(files)} files plus manifest; archive integrity and SHA-256 verified')
print(archive)
print(f'ZIP SHA256: {hashlib.sha256(archive.read_bytes()).hexdigest()}')
