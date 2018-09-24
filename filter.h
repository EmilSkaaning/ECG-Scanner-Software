#ifndef FILTER_H_
#define FILTER_H_
#include <stdio.h>
#include <stdlib.h>

int lowpass(int* raw, int raw_len, int* y_mem, int mem_len);

int highpass(int* lowpass_mem, int lowpass_len, int* highpass_mem, int highpass_len);

int derivative(int* highpass_mem, int highpass_len);

int square(int deriv_PH);

int MWI(int* memory_x, int filter_placeholder);

void rotate(int* list, int list_len);

#endif
