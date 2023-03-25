import numpy
import numpy.random as rand
import scipy

# Siren dual tone single rotor

Fs = 48000      # sample rate
dt = 1/Fs       # time (seconds) between two adjacent samples
Length = 10     # total length in seconds
Frequency = 57 # base frequency in Hz
t = numpy.arange(0, Length, dt) # time array
s = numpy.zeros(Length * Fs)    # sound sample array

ports_low = 9 # low tone frequency multiplier
ports_high = 12 # high tone frequency multiplier
sr_factor = 1.122 # subtractive result factor
ar_factor = 0.5 # additive resultant factor

a = [] # amplitude for each harmonic
p = [] # phase for each harmonic
for i in range(400):
    a.append(0)
    p.append(0)
for i in range(1, int(400/ports_low)):
    a[i*ports_low] = 1/i
for i in range(1, int(400/ports_high)):
    a[i*ports_high] = 1/i
    p[i*ports_high] = numpy.pi / 2
adtv = ports_low + ports_high # additive resultant
sbtv = ports_high - ports_low # subtractive resultant
for i in range(1, int(400/adtv)):
    if a[i*adtv] == 0:
        a[i*adtv] = 1/i
        a[i*adtv] *= ar_factor
        p[i*adtv] = numpy.pi
for i in range(1, int(400/sbtv)):
    if a[i*sbtv] == 0:
        a[i*sbtv] = 1/i
        a[i*sbtv] *= sr_factor
        p[i*sbtv] = 3 * numpy.pi / 2
for i in range(1, 400):
    if a[i] == 0:
        a[i] = 0.01 # very subtle base frequency harmonics
        p[i] = numpy.pi * 2 * rand.ranf()

a[ports_low] *= numpy.power(10,1/20)
a[ports_high] *= numpy.power(10,1/20)

# reduce low-order resultant harmonics
for i in range(1, ports_low):
    if i % sbtv != 0:
        a[i] *= 0.5
    else:
        a[i] *= 0.25 / sr_factor

# reduce high frequencies
for i in range(25, 400):
    a[i] *= numpy.power(0.995, i-24)

for idx, i in enumerate(a):
    if idx != 0 and a[idx] != 0:
        print('Harmonic ' + str(idx) + ' - amp: ' + str(a[idx]) + ' phase: ' + str(p[idx]))
        s += a[idx] * numpy.sin(numpy.pi * 2 * t * idx * Frequency + p[idx])

s *= 16384 / max(s) # normalise volume to -6 dB
scipy.io.wavfile.write('siren_dtsr.wav', Fs, s.astype(numpy.int16))
