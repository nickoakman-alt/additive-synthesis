#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define TAU 6.2831853
#define MAXFREQ 58
#define MAXHARMONIC 100
#define SAMPLERATE 48000
#define LENGTH 10

typedef struct {
	double *s;
} amp;

amp *a;

double curfreq(double t) {
	if (t < 3.0) {
		return MAXFREQ * sin(t*TAU/12.0);
	} else {
		return MAXFREQ;
	}
}

double curphase(double t) {
	static int i = 0;
	static double phase = 0.0;
	static double dt = 1.0 / ((double)SAMPLERATE);

	phase += curfreq(t) * dt;

	i++;
	return phase;
}

void writesample(FILE *f, double d) {
	d *= 4096.0;
	short s = (short)d;
	char *c = (char *)&s;
	fputc(*c, f);
	c++;
	fputc(*c, f);
}

double variamp(int sample, int harmonic) {
	static int first = 1;

	if (first) {
		printf("First run of variamp()\n");
		int i,j;
		a = calloc(MAXHARMONIC+1, sizeof(amp *));
		for (i=0;i<=MAXHARMONIC;i++) {
			a[i].s = calloc(LENGTH*SAMPLERATE*2, sizeof(double));
		}
		first = 0;
	}

	printf("sample: %d harmonic: %d a: %lf\n", sample, harmonic, a[harmonic].s[sample]);
	return a[harmonic].s[sample];
}

int main() {
	int i,j;
	double d, phase, freq, t, amp1, amp2;
	FILE *f = fopen("2t22.raw","wb");

	for (i=0;i<LENGTH*SAMPLERATE;i++) {
		t = ((double)i) / ((double)SAMPLERATE);
		phase = curphase(t);
		freq = curfreq(t);
		d = 0.0;
		for (j=1;j<MAXHARMONIC;j++) {
			if (((double)j) * 10.0 * freq > ((double)SAMPLERATE) / 2.0)
				break;
			amp2 = 1.0;
			if (j < 10) {
				amp1 = (1.0/((double)j));
			}
			else {
				amp1 = (1.0/10.0);
				amp2 = pow(0.9,((double)j)-10.0);
			}
			d += (1.0 - (0.292893*variamp(i,j))) * amp1 * amp2 * sin(10.0*((double)j)*TAU*phase);
		}
		for (j=1;j<MAXHARMONIC;j++) {
			if (((double)j) * 12.0 * freq > ((double)SAMPLERATE) / 2.0)
				break;
			amp2 = 1.0;
			if (j < 10) {
				amp1 = (1.0/((double)j));
			}
			else {
				amp1 = (1.0/10.0);
				amp2 = pow(0.9,((double)j)-10.0);
			}
			if (j % 5 == 0)
				amp2 *= 0.5;
			d += amp1 * amp2 * sin(12.0*((double)j)*TAU*phase);
		}
		for (j=1;j<MAXHARMONIC;j++) {
			if (((double)j) * 12.0 * freq > ((double)SAMPLERATE) / 2.0)
				break;
			amp2 = 1.0;
			if (j < 3) {
				amp1 = (1.0/3.0);
			}
			else if (j < 20) {
				amp1 = (1.0/((double)j));
			}
			else {
				amp1 = (1.0/20.0);
				amp2 = pow(0.95,((double)j)-20.0);
			}
			d += 0.5 * amp1 * amp2 * sin(5.0*((double)j)*TAU*phase);
		}
		for (j=1;j<MAXHARMONIC;j++) {
			if (((double)j) * 12.0 * freq > ((double)SAMPLERATE) / 2.0)
				break;
			amp2 = 1.0;
			if (j < 3) {
				amp1 = (1.0/3.0);
			}
			else if (j < 20) {
				amp1 = (1.0/((double)j));
			}
			else {
				amp1 = (1.0/20.0);
				amp2 = pow(0.95,((double)j)-20.0);
			}
			d += 0.5 * amp1 * amp2 * sin(4.0*((double)j)*TAU*phase);
		}
		writesample(f,d);
	}

	fclose(f);
	return 0;
}
