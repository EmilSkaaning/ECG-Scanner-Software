#include "detecting.h"
#include <stdio.h>
#include <stdlib.h>

int find_peak(int* filtered_mem){
    int peak_pos = 0;
    if (filtered_mem[0] < filtered_mem[1] && filtered_mem[1] > filtered_mem[2]){
        peak_pos = 1;
    }
    return peak_pos;
}

int calc_avg(int* arr, int counter){
    int sum = 0;
    int avg;
    int div;
    if (counter<8){
        div = counter;
    }
    else{
        div = 8;
    }
    for (int i=0; i<div;i++){
        sum += arr[7-i];
    }
    avg = sum/div;
    return avg;
}
