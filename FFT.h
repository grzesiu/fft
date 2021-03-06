#ifndef FFT_H
#define FFT_H

typedef struct _complex
{
	double Re, Im;
} complex;

/**  	
 *Calculates value A = 2^n (n = 0, 1, 2, 3, ...) which is equal or bigger than |num|.
 *
 *@param num - number for which value A will be calculated.
 *
 *@return value A.
 *	  	
**/
unsigned int next_pow_2(unsigned int num);

/**
 *Calculates number which has reversed binary representation of number |num| with length equal to |bits|.
 *
 *@param num - number for which reversed binary calulation will be calculated
 *@param bits - length of binary representation of numbers
 *
 *@return number equal to reversed binary representation of |num| on success, otherwise -1
 *
**/
int reverse(unsigned int num, unsigned int bits);

/**
 *Puts values from array pointed by |input| to array pointed by |output| to positions calculated by reverse function.
 *
 *@param input - source array
 *@param output - target array
 *@param bits - length of binary representation of numbers
 *
 *@return 0 on success, otherwise -1
 *
**/
int fft_bit_reverse_copy(const complex *input, complex *output, const unsigned int bits);

/**
 *Calculates DFT of values in array stored in |input| and stores the output in array pointed by |output|.
 *
 *@param input - array of values for which DFT will be calculated
 *@param output - array where output of DFT will be stored
 *@param bits - length of binary representation of numbers
 *
 *@return 0 on success, otherwise -1
 *
**/
int fft_iterative(const complex *input, complex *output, const unsigned int bits, const char dir);

/**
 *Calculates DFT on values stored in |input| and stores output in |output|.
 *If lenght of values in |input| is not equal to 2^n (where n = 0, 1, 2, 3, ...) trailing zeros are added.
 *
 *@param input - array where values for which DFT will be caluculated
 *@param output - array where output of DFT will be stored
 *@param bits - length of binary representation of numbers
 *
 *@return 0 on success, otherwise -1
 *
**/
int fft_to_frequency_domain(complex **input, complex **fft, const unsigned int length);

int fft_to_time_domain(complex *dft_input, complex **output, const unsigned int length);

/**
 *Gets main frequency of signal (i.e. frequency component with highest amplitude).
 *
 *@param dft - signal's DFT
 *@param length - length of DFT or signal
 *@param f_sampling - signal sampling frequency
 *
 *@return main frequency of signal
 *
**/
double fft_get_main_frequency(const complex *dft, const unsigned int length, const double f_sampling);

#endif //FFT_H