import numpy
import scipy.interpolate as interp
import numpy.random as rand
from scipy.io.wavfile import write

SAMPLE_RATE = 48000
BASE_FREQUENCY = 116

def calc_freq():
    freq = numpy.zeros(SAMPLE_RATE)
    t = numpy.arange(0, 60, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-t))
    t = numpy.arange(0, 10, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-t/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.33758)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.105234)/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451099)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.093875)/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451909)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.093798)/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451914)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.093798)/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451914)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.093798)/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451914)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.093798)/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451914)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.093798)/8))
    t = numpy.arange(0, 4, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451914)))
    t = numpy.arange(0, 8, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.093798)/8))
    t = numpy.arange(0, 2, 1/SAMPLE_RATE)
    freq = numpy.append(freq, 1 - numpy.exp(-(t+0.451914)))
    t = numpy.arange(0, 20, 1/SAMPLE_RATE)
    freq = numpy.append(freq, numpy.exp(-(t+0.720523)/8))
    return freq

def calc_phase(freq):
    phase = numpy.zeros(len(freq))
    phase[0] = freq[0]
    for idx, f in enumerate(freq):
        if idx == 0:
            continue
        phase[idx] = f/SAMPLE_RATE + phase[idx-1]
    return phase

def calc_rand(freq, i):
    t = numpy.linspace(0, len(freq)/SAMPLE_RATE, 500+(i*2))
    r = rand.ranf(500+(i*2))
    rf = interp.interp1d(t, r, kind='cubic')
    rnew = rf(numpy.arange(0, len(freq)/SAMPLE_RATE, 1/SAMPLE_RATE))
    rnew = rnew * 0.5 + 0.5
    return rnew

freq = calc_freq()
phase = calc_phase(freq)
s = numpy.zeros(len(phase))

a = []
p = []
with open('data_amp', 'r') as file:
    while True:
        line = file.readline()
        if len(line) == 0:
            break
        a.append(float(line))
with open('data_phase', 'r') as file:
    while True:
        line = file.readline()
        if len(line) == 0:
            break
        p.append(float(line))

r_amp = 1.1 + numpy.tanh(numpy.cos(numpy.linspace(0,numpy.pi,90)) - 1.5)
r_const = numpy.zeros(90)
r_const += 0.113386
r_amp = numpy.append(r_amp, r_const)
r_amp = numpy.append(r_amp, numpy.flip(r_amp))
r_angle = 345
r_angle_float = 345.0
r_t = numpy.arange(0, len(phase)/SAMPLE_RATE+1/24, 1/24)
r_t_val = numpy.zeros(len(r_t))
for idx, i in enumerate(r_t_val):
    if idx > 48 and idx < 4392:
        x = r_t[idx-48]
        r_angle_float += 1 - numpy.exp(-x*x)
        r_angle = int(r_angle_float)
    else:
        x = r_t[idx-4392]
        r_angle_float += numpy.exp(-x*x)
        r_angle = int(r_angle_float)
    if r_angle_float >= 360.0:
        r_angle_float -= 360.0
    if r_angle >= 360:
        r_angle = 0
    r_t_val[idx] = r_amp[r_angle]
r_t_interp = interp.interp1d(r_t, r_t_val, kind='cubic')
r_t_sr = numpy.arange(0, len(phase)/SAMPLE_RATE, 1/SAMPLE_RATE)
r_t_val_sr = r_t_interp(r_t_sr)

for idx, i in enumerate(a):
    s += i * r_t_val_sr * calc_rand(freq, idx+1) * numpy.sin(numpy.pi * 2 * (idx+1) * BASE_FREQUENCY * phase + p[idx])

s *= 16384/max(s)
write('thunderbolt.wav', SAMPLE_RATE, s.astype(numpy.int16))
