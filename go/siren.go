/*
	Siren tone generator
	2023 Nick Oakman
*/

package main

import(
	"fmt"
	"math"
	"math/rand"
	"os"
)

const SAMPLERATE = 48000 // samples per second
const LENGTH = 60 // total length in seconds
const MAXHARMONICS = 100
const BASEMODFACTOR = 10000 // initial # of samples per random amplitude for interpolation
const MAXAMPCHANGE = 0.9 // maximum random amplitude variance
const MAXFREQ = 58 // peak motor speed rotations per second
const PORTS1 = 10
const PORTS2 = 12
const VANES1 = 5
const VANES2 = 4

func check(e error) {
	if e != nil {
		panic(e)
	}
}

/*
 * Encodes int64 into byte slice of length 8
 */
func MyPutUvarint(buf []byte, x int64) {
	for i := 0; i < 8; i++ {
		buf[i] = byte(x)
		x >>= 8
	}
}

/*
 * Generates cosine interpolated random noise
 */
func CosineInterpolate(harmonic int64) ([]float64) {
	var diff float64
	var amp float64
	var offset float64
	var step float64
	var numrandom int64
	var numsamples int64
	var cursamp int64
	var x float64
	var i int64

	step = 1 / (float64)(BASEMODFACTOR / harmonic)
	numsamples = SAMPLERATE * LENGTH
	numrandom = numsamples / (BASEMODFACTOR / harmonic)

	random := make([]float64, numrandom)
	for i = 0; i < numrandom; i++ {
		random[i] = rand.Float64()
	}

	samples := make([]float64, numsamples)
	for i = 1; i < numrandom; i++ {
		diff = random[i-1] - random[i]
		amp = diff / 2
		if diff < 0 {
			offset = random[i-1] + math.Abs(amp)
		} else {
			offset = random[i] + amp
		}
		x = 0
		for cursamp = 0; cursamp < (BASEMODFACTOR / harmonic); cursamp++ {
			samples[cursamp+(i-1)*(BASEMODFACTOR / harmonic)] = amp * math.Cos(x*math.Pi) + offset
			x += step
		}
	}
	for i = 0; i < numsamples; i++ {
		samples[i] *= MAXAMPCHANGE
		samples[i] += 1 - MAXAMPCHANGE
	}
	return samples
}

/*
 * Determines frequency at time t (in seconds)
 */
func CurFreq(t float64) float64 {
	if t < 3 { return MAXFREQ * math.Sin((t*math.Pi*2)/12) }
	if t < 8 { return MAXFREQ }
	if t < 12 { t -= 8; return MAXFREQ * math.Exp(-t/10) }
	if t < 13.5969 { t -= 10.5969; return MAXFREQ * math.Sin((t*math.Pi*2)/12) }
	if t < 20 { return MAXFREQ }
	if t < 24 { t -= 20; return MAXFREQ * math.Exp(-t/10) }
	if t < 25.5969 { t -= 22.5969; return MAXFREQ * math.Sin((t*math.Pi*2)/12) }
	if t < 30 { return MAXFREQ }
	t -= 30
	return MAXFREQ * math.Exp(-t/10)
}

func main() {
	var t float64 // current time in seconds
	var dt float64 = 1 / (float64)(SAMPLERATE) // time distance in seconds between two samples
	var freq float64 // current frequency at time t
	var phase float64 // running integral of current frequency
	var amp float64 // base amplitude determined from current base frequency and harmonic
	var vhamp float64 // amplitude of the undertone harmonic caused by the rotor vanes

	/*
		Aquire random amplitude samples
	*/
	randamp1 := make([][]float64, MAXHARMONICS)
	for i := 1; i < MAXHARMONICS; i++ {
		randamp1[i] = CosineInterpolate((int64)(i))
	}
	randamp2 := make([][]float64, MAXHARMONICS)
	for i := 1; i < MAXHARMONICS; i++ {
		randamp2[i] = CosineInterpolate((int64)(i))
	}
	randamp3 := make([][]float64, MAXHARMONICS)
	for i := 1; i < MAXHARMONICS; i++ {
		randamp3[i] = CosineInterpolate((int64)(i))
	}
	randamp4 := make([][]float64, MAXHARMONICS)
	for i := 1; i < MAXHARMONICS; i++ {
		randamp4[i] = CosineInterpolate((int64)(i))
	}

	// random starting phase
	randphase := make([]float64, MAXHARMONICS)
	for i := 0; i < MAXHARMONICS; i++ {
		randphase[i] = rand.Float64()
	}

	/*
		Generate the actual sound
	*/
	samples := make([]float64, SAMPLERATE * LENGTH)
	for i := 0; i < len(samples); i++ {
		// update time/frequency/phase for each sample iteration
		t = (float64)(i) / (float64)(SAMPLERATE)
		freq = CurFreq(t)
		phase += freq * dt
		amp = freq / MAXFREQ

		for j := 1; j < MAXHARMONICS; j++ {
			// these are the two main tones determined by the rotor/stator ports
			if (float64)(j) * freq * PORTS1 < SAMPLERATE / 2 { // nyquist theorem...
				samples[i] += amp * randamp1[j][i] * 0.1 * (1/(float64)(j)) * 
					math.Sin(randphase[j] + phase * PORTS1 * math.Pi * 2 * (float64)(j))
			}
			if (float64)(j) * freq * PORTS2 < SAMPLERATE / 2 && 
				j * PORTS2 % PORTS1 != 0 { // it sounds bad if we add the same frequency twice
				samples[i] += amp * randamp2[j][i] * 0.1 * (1/(float64)(j)) * 
					math.Sin(randphase[j] + phase * PORTS2 * math.Pi * 2 * (float64)(j))
			}

			// these are the undertones caused by the rotor vanes
			if j > 4 {
				vhamp = 1 / (float64)(j)
			} else {
				vhamp = 0.25
			}
			if (float64)(j) * freq * VANES1 < SAMPLERATE / 2 {
				samples[i] += amp * vhamp * randamp3[j][i] * 0.05 *
					math.Sin(randphase[j] + phase * VANES1 * math.Pi * 2 * (float64)(j))
			}
			if (float64)(j) * freq * VANES2 < SAMPLERATE / 2 {
				samples[i] += amp * vhamp * randamp4[j][i] * 0.05 *
					math.Sin(randphase[j] + phase * VANES2 * math.Pi * 2 * (float64)(j))
			}
		}
	}

	/*
		Convert float samples to integer, integer to byte, and write to file
	*/
	intsamples := make([]int64, SAMPLERATE * LENGTH)
	bytesamples := make([]byte, SAMPLERATE * LENGTH * 2)
	b := make([]byte, 8)
	for i := 0; i < SAMPLERATE * LENGTH; i++ {
		intsamples[i] = (int64)(32767 * samples[i])
	}
	for i := 0; i < SAMPLERATE * LENGTH; i++ {
		for j := 0; j < 8; j++ {
			b[j] = 0
		}
		MyPutUvarint(b, intsamples[i])
		bytesamples[i*2] = b[0]
		bytesamples[(i*2)+1] = b[1]
	}

	f, err := os.Create("siren.raw")
	check(err)
	defer f.Close()
	n, err := f.Write(bytesamples)
	check(err)
	fmt.Println("Wrote", n, "bytes")
}
