#include "filter.h"
#include <stdio.h>
#include <stdlib.h>



int lowpass(int* raw_mem, int raw_len, int* lowpass_mem, int lowpass_len){
	int y = 2*lowpass_mem[lowpass_len-2]-lowpass_mem[lowpass_len-3]+((raw_mem[raw_len-1]-2*raw_mem[raw_len-7]+raw_mem[raw_len-13])/32);
	return y;
}

int highpass(int* lowpass_mem, int lowpass_len, int* highpass_mem, int highpass_len){
	int y = highpass_mem[highpass_len-2]-((lowpass_mem[lowpass_len-1]-lowpass_mem[lowpass_len-33])/32)+lowpass_mem[lowpass_len-17]-lowpass_mem[lowpass_len-18];
	return y;
}

int derivative(int* highpass_mem, int highpass_len){
	int y = (2*highpass_mem[highpass_len-1]+highpass_mem[highpass_len-2]-highpass_mem[highpass_len-4]-2*highpass_mem[highpass_len-5])/8;
	return y;
}

int square(int deriv_PH){
	int y = deriv_PH*deriv_PH;
	return y;
}

int MWI(int* square_mem, int square_len){
	int sum = 0;
	for(int i = 0; i < square_len; i++){
		sum += square_mem[i];
	}

	int y = sum/square_len;
	return y;
}

void rotate(int* list, int list_len){

	for(int i=0; i<list_len; i++){
		list[i] = list[i+1];
	}

}









