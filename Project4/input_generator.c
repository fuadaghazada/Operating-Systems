/*
    Operating Systems: Random Input generator

    author: Fuad Aghazada
    date: 08/10/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "input_generator.h"

int *rand_numbers;

// Seeding -- In order to get different random numbers
void seed()
{
    srand (time(NULL));
}

/**
    Generates a random number between the range

    min: bottom limit
    max: up limit
*/
int generateRandomNumber(int min, int max)
{
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

/**
    Generates random numbers in the given size between the range

    size: the number of random numbers
    min: bottom limit
    max: up limit
*/
int *generateRandomNumbers(int size, int min, int max)
{
    seed();

    free(rand_numbers);
    rand_numbers = malloc (sizeof (int) * size);

    int i;
    for(i = 0; i < size; i++)
    {
        rand_numbers[i] = generateRandomNumber(min, max);
    }

    return rand_numbers;
}

/**
    Clears the allocated memory for random numbers
*/
void clearRandomNumbers()
{
    free(rand_numbers);
}
