#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"
#include "filter.h"
#include "detecting.h"

#define raw_len 13
#define lowpass_len 33
#define highpass_len 5
#define square_len 30
#define filtered_len 3
#define PEAK_LEN 8
#define SPKF 0
#define NPKF 0
#define TH1 0
#define TH2 0
#define RR_LEN 8

int main(){
    int deriv_PH;
    int value = 0;
    int *value_pointer = &value;
    int file_boolean = 1;
    int *file_boolean_pointer = &file_boolean;
    int k = 0;

    int pot_peak = 0;
    int spkf = SPKF;
    int npkf = NPKF;
    int th1 = TH1;
    int th2 = TH2;
    int rr_average1 = 0;
    int rr_average2 = 0;
    int rr_low = 0;
    int rr_high = 0;
    int rr_miss = 0 ;
    int rr;
    int rr_placeholder = 0;
    int peak_counter = 0;
    int peak_counter_ok = 0;
    int r_peak = 0;

    int raw_mem[raw_len];
    for(int i=0; i<raw_len; i++){
        raw_mem[i]=0;
    }
    int lowpass_mem[lowpass_len];
    for(int i=0; i<lowpass_len; i++){
        lowpass_mem[i]=0;
    }
    int highpass_mem[highpass_len];
    for(int i=0; i<highpass_len; i++){
        highpass_mem[i]=0;
    }
    int square_mem[square_len];
    for(int i=0; i<square_len; i++){
        square_mem[i]=0;
    }
    int filtered_mem[filtered_len];
    for(int i=0; i<filtered_len; i++){
        filtered_mem[i]=0;
    }
    int peaks[PEAK_LEN];
    for(int i=0; i<PEAK_LEN; i++){
        peaks[i]=0;
    }
    int recentrr[RR_LEN];
    int recentrr_ok[RR_LEN];
    for(int i=0; i<RR_LEN; i++){
        recentrr[i]=0;
        recentrr_ok[i]=0;
    }

    FILE *file = openfile("ECG.txt");
    FILE *w_file = fopen("OUTPUT.txt", "w");
    FILE *mwi_file = fopen("MWI_output.txt", "w");
    FILE *th_file = fopen("th_output.txt", "w");
    FILE *peak_pos_file = fopen("peak_output.txt", "w");

    getNextData(file, value_pointer, file_boolean_pointer);
    while(file_boolean == 1){
        k +=1;

        raw_mem[raw_len-1] = value;

        lowpass_mem[lowpass_len-1] = lowpass(raw_mem, raw_len, lowpass_mem, lowpass_len);

        highpass_mem[highpass_len-1] = highpass(lowpass_mem, lowpass_len, highpass_mem, highpass_len);

        deriv_PH = derivative(highpass_mem, highpass_len);

        square_mem[square_len-1] = square(deriv_PH);

        filtered_mem[filtered_len-1] = MWI(square_mem, square_len);

        pot_peak = find_peak(filtered_mem);
        if (pot_peak == 1){
            peaks[PEAK_LEN-1] = filtered_mem[filtered_len-2];
            fprintf(w_file, "%i) Peak found! int = %i, sample %i \n", k, peaks[PEAK_LEN-1], (k-1));
            rotate(peaks, PEAK_LEN);
            if (peaks[PEAK_LEN-1] < th1){
                fprintf(w_file, "%i) Peak < Threshold1. %i < %i\n\n", k, peaks[PEAK_LEN-2], th1);
                npkf = 0.125 * filtered_mem[filtered_len-2] + 0.875 * npkf;
                th1 = npkf + 0.25 * (spkf - npkf);
                th2 = 0.5 * th1;
            }
            else{
                fprintf(w_file, "%i) Peak > Threshold1. %i > %i\n", k, peaks[PEAK_LEN-2], th1);
                rr = (k-1) - rr_placeholder;
                rr_placeholder = (k-1);
                if (rr_low < rr && rr < rr_high){
                    fprintf(w_file, "%i) True: %i < %i < %i \n\n", k, rr_low, rr, rr_high);
                    peak_counter += 1;
                    peak_counter_ok += 1;
                    spkf = 0.125 * filtered_mem[filtered_len-1] + 0.875 * spkf;
                    recentrr[RR_LEN-1] = rr;
                    recentrr_ok[RR_LEN-1] = rr;
                    rr_average1 = calc_avg(recentrr, peak_counter);
                    rr_average2 = calc_avg(recentrr_ok, peak_counter_ok);
                    rr_low = 0.92 * rr_average2;
                    rr_high = 1.16 * rr_average2;
                    rr_miss = 1.66 * rr_average2;
                    th1 = npkf + 0.25 * (spkf - npkf);
                    th2 = 0.5 * th1;
                    r_peak = filtered_mem[filtered_len-2];
                    rotate(recentrr, RR_LEN);
                    rotate(recentrr_ok, RR_LEN);
                }
                else{
                    fprintf(w_file, "%i) False: %i < %i < %i \n", k, rr_low, rr, rr_high);
                    if(rr > rr_miss){
                        fprintf(w_file, "%i) True: rr > rr miss, %i > %i \n", k, rr, rr_miss);
                        for (int i=0; i<PEAK_LEN-2; i++){
                            if (peaks[PEAK_LEN-(i+2)]>th2){
                                fprintf(w_file, "Peak len %i\n", PEAK_LEN-(i+1));
                                fprintf(w_file, "%i) Search backwards: %i > %i", k, peaks[PEAK_LEN-2], th2);
                                spkf = 0.25 * peaks[PEAK_LEN- (i+2)] + 0.75 * spkf;
                                recentrr[RR_LEN-1] = rr;
                                peak_counter += 1;
                                rr_average1 = calc_avg(recentrr, peak_counter);
                                rr_low = 0.92 * rr_average1;
                                rr_high = 1.16 * rr_average1;
                                rr_miss = 1.66 * rr_average1;
                                th1 = npkf + 0.25 * (spkf - npkf);
                                th2 = 0.5 * th1;
                                r_peak = filtered_mem[filtered_len-2];
                                rotate(recentrr, RR_LEN);
                                fprintf(w_file, "\n");
                                break;
                            }
                        }
                    }
                    else{
                        fprintf(w_file, "%i) False: %i < %i \n", k, rr, rr_miss);
                    }
                }
            }
            fprintf(w_file, "\n");
        }
        rotate(raw_mem, raw_len);
        rotate(lowpass_mem, lowpass_len);
        rotate(highpass_mem, highpass_len);
        rotate(square_mem, square_len);
        rotate(filtered_mem, filtered_len);
        //printf("%i) %i %i %i %i\n", k, lowpass_mem[lowpass_len-2], highpass_mem[highpass_len-2], square_mem[square_len-2], filtered_mem[filtered_len-2]);

        //printf("%i %i %i\n",k, rr, peaks[PEAK_LEN-1]);

        fprintf(mwi_file, "%i\n", filtered_mem[filtered_len-2]);
        fprintf(th_file, "%i\n", th1);
        fprintf(peak_pos_file, "%i\n", pot_peak);
        getNextData(file, value_pointer, file_boolean_pointer);
    }

    return 0;
}
