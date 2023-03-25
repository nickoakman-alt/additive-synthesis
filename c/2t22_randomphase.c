/*
 * 2T22 siren sound generator using additive synthesis
 *
 * 2020 Nick Oakman
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define TWO_PI 6.2831853
#define SAMPLE_RATE 48000
#define LENGTH 250
#define FREQUENCY 55.95
#define HARMONICS 1024

double current_frequency(double time)
{
	if (time < 10.0)
		return 56.0;
	time -= 10.0;
	if (time < 8.0)
		return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
	time -= 8.0;
	if (time < 4.0)
		return FREQUENCY * 1.0/((time/6.0)+1.0);
	time -= 4.0;
	if (time < 8.0)
	{
		time += 1.91446;
		return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
	}
	time -= 8.0;
	if (time < 4.0)
		return FREQUENCY * 1.0/((time/6.0)+1.0);
	time -= 4.0;
	if (time < 8.0)
	{
		time += 1.91446;
		return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
	}
	time -= 8.0;
	if (time < 4.0)
		return FREQUENCY * 1.0/((time/6.0)+1.0);
	time -= 4.0;
	if (time < 8.0)
	{
		time += 1.91446;
		return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
	}
	time -= 8.0;
	if (time < 4.0)
		return FREQUENCY * 1.0/((time/6.0)+1.0);
	time -= 4.0;
	if (time < 8.0)
	{
		time += 1.91446;
		return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
	}
	time -= 8.0;


        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;
        if (time < 4.0)
                return FREQUENCY * 1.0/((time/6.0)+1.0);
        time -= 4.0;
        if (time < 8.0)
        {       
                time += 1.91446;
                return FREQUENCY * (1.0 - exp(-((time/2.0) * (time/2.0))));
        }
        time -= 8.0;


	return FREQUENCY * 1.0/((time/6.0)+1.0);
}

int main()
{
	FILE *f = fopen("2t22.raw","wb");
	int i,j;
	double d;
	double phase = 0;
	double amp;
	double inharm_factor[HARMONICS];
	short s;

	srand(time(0));

	for (i=0;i<HARMONICS;i++)
	{
		inharm_factor[i] = (rand() % 10 / 10000);
		inharm_factor[i] += 1.001;
	}

	for (i=1;i<LENGTH*SAMPLE_RATE;i++)
	{
		d = 0;
		phase += TWO_PI * ((0.2 * sin(TWO_PI * 0.1 * ((double)i/(double)SAMPLE_RATE))) + current_frequency((double)i/(double)SAMPLE_RATE) ) * 1.0/(double)SAMPLE_RATE;
		for (j=1;j<HARMONICS;j++)
		{
			if (j == 4)
				d += 0.125 * current_frequency((double)i/(double)SAMPLE_RATE)/FREQUENCY * (1.0/pow((double)j,1.5)) * sin(phase * pow((double)j,inharm_factor[j]));
			if (j == 5)
				d += 0.25 * current_frequency((double)i/(double)SAMPLE_RATE)/FREQUENCY * (1.0/pow((double)j,1.5)) * sin(phase * pow((double)j,inharm_factor[j]));
			if (j < 10)
				continue;
			if (j * FREQUENCY > SAMPLE_RATE/2)
				break;
			amp = 1.0;
			if (j > 50)
				amp = pow(0.99,(double)j-50.0);
			if ( !(j % 10) || !(j % 12) )
				d += amp * 20.0 * current_frequency((double)i/(double)SAMPLE_RATE)/FREQUENCY * (1.0/pow((double)j,1.5)) * sin(phase * pow((double)j,inharm_factor[j]));
			else if ( !(j % 4) || !(j % 5) )
				d += amp * 3.0 * current_frequency((double)i/(double)SAMPLE_RATE)/FREQUENCY * (1.0/pow((double)j,1.5)) * sin(phase * pow((double)j,inharm_factor[j]));
			else if (j < 36)
				d += amp * 1.0 * current_frequency((double)i/(double)SAMPLE_RATE)/FREQUENCY * (1.0/pow(36.0,1.5)) * sin(phase * pow((double)j,inharm_factor[j]));
			else
				d += amp * 1.0 * current_frequency((double)i/(double)SAMPLE_RATE)/FREQUENCY * (1.0/pow((double)j,1.5)) * sin(phase * pow((double)j,inharm_factor[j]));
		}
		d = 16384.0 * atan(d);
		s = (short)d;
		fwrite(&s, 2, 1, f);
	}

	fclose(f);
	return 0;
}
