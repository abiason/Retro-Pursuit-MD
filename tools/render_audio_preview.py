"""Approximate the PSG requests emitted by tools/audio_dump.c as an offline WAV."""
import array
from pathlib import Path
import sys
import wave

frames = [list(map(int, line.split())) for line in Path('out/audio_frames.txt').read_text().splitlines()]
sample_rate = 44100
phases = [0.0, 0.0, 0.0]
noise_phase = 0.0
lfsr = 0x8000
samples = array.array('h')
for frequencies_and_levels in frames:
    frequencies = frequencies_and_levels[:3]
    levels = frequencies_and_levels[3:]
    amplitudes = [0.0 if level == 15 else 10 ** (-level / 10) for level in levels]
    for _ in range(sample_rate // 60):
        value = 0.0
        for channel, frequency in enumerate(frequencies):
            if frequency:
                phases[channel] = (phases[channel] + frequency/sample_rate) % 1.0
                value += (1.0 if phases[channel] < 0.5 else -1.0) * amplitudes[channel]
        noise_phase += 6991 / sample_rate
        if noise_phase >= 1.0:
            noise_phase -= 1.0
            feedback = (lfsr ^ (lfsr >> 3)) & 1
            lfsr = (lfsr >> 1) | (feedback << 15)
        value += (1.0 if lfsr & 1 else -1.0) * amplitudes[3]
        samples.append(int(value * 0.45 * 32767))
peak = max(abs(value) for value in samples)
assert 0 < peak < 32767
assert any(value for value in samples[:sample_rate*3])
assert all(value == 0 for value in samples[sample_rate*7:int(sample_rate*7.5)])
if sys.byteorder != 'little': samples.byteswap()
with wave.open('out/audio-preview.wav', 'wb') as output:
    output.setnchannels(1)
    output.setsampwidth(2)
    output.setframerate(sample_rate)
    output.writeframes(samples.tobytes())
print(f'PASS: offline PSG preview, {len(frames)/60:.1f}s, peak={peak}, pause is silent, no clipping')
