#ifndef SENSOR_H
#define SENSOR_H
#include <stdio.h>
#include <stdlib.h>


FILE* openfile(const char* filename);

void getNextData(FILE *file, int* value_pointer, int* file_boolean_pointer);

#endif // SENSOR_H
