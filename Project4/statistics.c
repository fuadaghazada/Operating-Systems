/**
    Source file for Statistics formulas

    author: Fuad Aghazada
    date: 17/12/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "statistics.h"
/**
    Calculates and returns the mean of values in the array

    results_data: array of values
    size: size of the array
*/
float calc_mean(int result_data[], int size)
{
    int sum = 0;

    int i;
    for(i = 0; i < size; i++)
    {
        sum += result_data[i];
    }

    return sum / size;
}

/**
    Calculates and returns the Standard deviation of values in the array

    results_data: array of values
    size: size of the array
*/
float calc_sd(int result_data[], int size)
{
    float sd = 0.0;
    float mean = calc_mean(result_data, size);

    for(int i = 0; i < size; i++)
	{
		sd += pow(result_data[i] - mean, 2);
	}

    sd = sqrt(sd / size);

    return sd;
}
