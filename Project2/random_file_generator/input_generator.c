/*
    Operating Systems - Project 1 : Processes and Threads

    author: Fuad Aghazada
    date: 08/10/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

// Function Protypes
int generateRandomNumber(int min, int max);

// Main for executing
int main(int argc, char **argv)
{
    if(argc != 5)
    {
        printf("ERROR: Check the parameters!\n");
        exit(1);
    }

    int min = atoi(argv[1]);
    int max = atoi(argv[2]);
    int n = atoi(argv[3]);
    char filename [50];

    strcpy(filename, argv[4]);
    char path [100] = "../input/";
    strcat(path, filename);

    // File
    FILE *f = fopen(path, "w");
    if(f == NULL)
    {
        printf("ERROR:");
        exit(1);
    }

    srand ( time(NULL) );

    // Generating random numbers and writing them to the given file
    for(int i = 0; i < n; i ++)
    {
        int r_num = generateRandomNumber(min, max);
        fprintf(f, "%d\n", r_num);
    }

    return 0;
}

/*
    Generates random number between the range

    min: bottom limit
    max: up limit
*/
int generateRandomNumber(int min, int max)
{
    //max--; // dont want to include max
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
