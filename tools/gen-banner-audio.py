#!/usr/bin/env python3
"""Génère la courte mélodie de bannière de NDS-Shop (WAV 16 bits stéréo).

Composition originale : deux notes douces (do5 puis sol5) avec une enveloppe
progressive, uniquement des sinusoïdes — aucun échantillon tiers.
"""
import math
import struct
import sys
import wave

RATE = 44100
DURATION = 2.0
NOTES = [(523.25, 0.00, 1.20), (783.99, 0.30, 1.20)]
AMPLITUDE = 0.12


def envelope(t, start, length):
    if t < start or t > start + length:
        return 0.0
    local = t - start
    attack = 0.02
    if local < attack:
        return local / attack
    return math.exp(-3.2 * (local - attack) / max(length - attack, 1e-6))


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "BannerAudio.wav"
    frames = int(RATE * DURATION)
    data = bytearray()

    for i in range(frames):
        t = i / RATE
        value = 0.0
        for freq, start, length in NOTES:
            env = envelope(t, start, length)
            if env > 0.0:
                value += env * math.sin(2.0 * math.pi * freq * t)
        value = max(-1.0, min(1.0, value * AMPLITUDE))
        sample = int(value * 32767)
        data += struct.pack("<hh", sample, sample)

    with wave.open(path, "wb") as out:
        out.setnchannels(2)
        out.setsampwidth(2)
        out.setframerate(RATE)
        out.writeframes(bytes(data))


if __name__ == "__main__":
    main()
