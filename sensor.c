#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>

void getNextData(FILE *file, int* value_pointer, int* file_boolean_pointer){
	int value;
	int file_boolean = fscanf(file, "%i", &value);
	*value_pointer=value;
	*file_boolean_pointer = file_boolean;
}

FILE* openfile(const char* filename)
{
	FILE *file = fopen(filename, "r");

	return file;
}


