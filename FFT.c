#define _COMPLEX_DEFINED
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "FFT.h"

int reverse(int num, int bits)
{
	int *stack = NULL, *tmp;
	int height = 0, max_heigth = 0;
	int i;
	int output = 0;

	assert(num >= 0 && bits > 0);

	while(num != 0 || height != bits)
	{
		if(height >= max_heigth)
		{
			max_heigth = 2 * height + 2;
			tmp = (int *)realloc(stack, max_heigth * sizeof(int));
			if(tmp == NULL)
			{
				free(stack);
				return -1;
			}
			stack = tmp;
		}

		stack[height++] = num % 2;
		num /= 2;
	}

	for(i = 0; height != 0; i++)
	{
		output += stack[--height] * 1<<i;
	}

	free(stack);
	return output;
}

int fft_bit_reverse_copy(const double *input, complex *output, int bits)
{
	int i, rev;

	assert(input != NULL && output != NULL && bits > 0);

	for(i = 0; i < 1<<bits; i++)
	{
		rev = reverse(i, bits);
		if(rev == -1)
		{
			return -1; //reverse function failed
		}

		output[rev].Re = input[i];
	}
	return 0;
}

complex complex_multiply(complex A, complex B)
{
	complex ret;

	ret.Re = A.Re * B.Re - A.Im * B.Im;
	ret.Im = A.Re * B.Im + A.Im * B.Re;

	return ret;
}

int fft_iterative(const double *input, complex *output, int bits)
{
	int s, k, j, m;
	int i;
	double unity_root, w;
	complex t, u;
	complex num;

	assert(input != NULL && output != NULL && bits > 0);

	if(fft_bit_reverse_copy(input, output, bits) == -1)
	{
		return -1; //fft_bit_reverse_copy failed
	}

	for(s = 1; s <= bits; s++)
	{
		m = 1<<s; //number of elements for which DFT is computed
		unity_root = -(2 * M_PI) / m; //exponent of e - expotential form of complex number

		for(k = 0; k < 1<<bits; k += m)
		{
			num.Re = 1;
			num.Im = 0;
			for(j = 0; j <= m/2-1; )
			{
				t = complex_multiply(num, output[k + j + m/2]);
				u = output[k+j];
				
				output[k+j].Re = u.Re + t.Re;
				output[k+j].Im = u.Im + t.Im;
				output[k + j + m/2].Re = u.Re - t.Re;
				output[k + j + m/2].Im = u.Im - t.Im;
				
				w = unity_root * (++j); 
				num.Re = cos(w);
				num.Im = sin(w);
			}
		}
	}

	return 0;
}

int fft_to_frequency_domain(const double *input, int length, int f_sampling, const char *filename)
{
	int bits, i;
	double freq;
	complex *output = NULL;
	FILE *pFile;

	assert(input != NULL && length != 0 && f_sampling > 0);

	output = (complex *)calloc(length, sizeof(complex));
	if(output == NULL)
	{
		return -1; //memory allocation failed
	}

	bits = (int) (log10((float)length) / log10((float)2)); //log2(length)
	if(fft_iterative(input, output, bits) ==  -1)
	{
		return -1; //fft_iterative failed
	}

	pFile = fopen(filename, "w");
	if(pFile != NULL)
	{
		fprintf(pFile, "Frequency [Hz]; |FFT|\n"); //header
		for(i = 0; i < length; ++i)
		{
			freq = i * ((double)f_sampling / length);
			output[i].Re = (sqrt(pow(output[i].Re,2) + pow(output[i].Im,2)))/length; //absolute value calculated & normalization by division by length
			
			fprintf(pFile, "%.4lf;%.4lf\n", freq, output[i].Re);
		}
		fclose(pFile);
	}
	else
	{
		free(output);
		return -1; //file creation failed
	}

	free(output);
	return 0;
}

int fft_get_main_frequency(double *main_freq, const double *input, int length, int f_sampling)
{
	int bits, i;
	double freq;
	double tmp, current_max = 0;
	complex *output = NULL;

	assert(main_freq != NULL && input != NULL);

	output = (complex *)calloc(length, sizeof(complex));
	if(output == NULL)
	{
		return -1; //memory allocation failed
	}

	bits = (int) (log10((float)length) / log10((float)2)); //log2(length)
	if(fft_iterative(input, output, bits) ==  -1)
	{
		return -1; //fft_iterative failed
	}

	for(i = 0; i < length/2 + 1; ++i)
	{
		freq = i * ((double)f_sampling / length);
		tmp = sqrt(pow(output[i].Re,2) + pow(output[i].Im,2));

		if(tmp > current_max)
		{
			current_max = tmp;
			*main_freq = freq;
		}
	}

	free(output);
	return 0;
}

void sine_generator(double *input, double amp, double sine_freq, double noise_amp, int length, int f_sampling)
{
	int i;
	double t;

	assert(input != NULL);
	
	srand(time(NULL));

	for(i = 0; i < length; ++i)
	{
		t = i * ((double)1/f_sampling);
		input[i] = amp * sin(2*M_PI*sine_freq*t) + noise_amp * ((double)rand() / RAND_MAX);
	}
}