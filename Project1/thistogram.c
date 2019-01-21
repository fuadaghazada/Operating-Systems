/*
    Operating Systems - Project 1 : Processes and Threads (Part 2)

    author: Fuad Aghazada
    date: 10/10/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_NUM_OF_FILES 10000
#define MAX_SIZE_FILENAME 100
#define MAX_INPUT_SIZE 100

//*** Global ****
char out_file_name [MAX_SIZE_FILENAME];
char in_file_names [MAX_NUM_OF_FILES][MAX_SIZE_FILENAME];
float in_data[MAX_NUM_OF_FILES][MAX_INPUT_SIZE];
float min_value, max_value;
int bin_count, num_of_files;
pthread_t child_threads[MAX_NUM_OF_FILES];
// **************

// Function Prototypes
void handleThreads(float min_value, float max_value, int bit_count, int num_of_files);
void *handleChild(void *arg);
void handleMain();

// Main for executing
int main(int argc, char **argv)
{
    // Checking arguments
    if(argc < 6)
    {
        printf("Insufficient number of parameters. \nRequired format: \nthistogram minvalue maxvalue bincount N file1 … fileN outfile\n");
        return -1;
    }

    // Input values - got/parse/process
    min_value = atof(argv[1]);
    max_value = atof(argv[2]);
    bin_count = atoi(argv[3]);
    num_of_files = atoi(argv[4]);

    // Putting filenames coming from input into the array
    for(int i = 0; i < num_of_files; i++)
    {
        strcpy(in_file_names[i], argv[5 + i]);
    }
    strcpy(out_file_name, argv[num_of_files + 5]);

    // ------- Calling primary function + (time measuring)----------------
    struct timeval start_time, end_time;
    float elapsed_time;

    gettimeofday(&start_time, NULL);
    handleThreads(min_value, max_value, bin_count, num_of_files);
    gettimeofday(&end_time, NULL);

    // Calculating elapsed time in microseconds
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000;
    elapsed_time += (end_time.tv_usec - start_time.tv_usec);

    printf("\n\nElapsed time for multi-thread application: %f ms\n", elapsed_time);

    return 0;
}

void handleThreads(float min_value, float max_value, int bit_count, int num_of_files)
{
    // Creating the threads
    for(int i = 0; i < num_of_files; i++)
    {
        int res = pthread_create(&child_threads[i], NULL, handleChild, (void *)(intptr_t)i);

        if(res != 0)
        {
            perror("ERROR: Cannot create thread\n");
            exit(1);
        }
    }

    // Waiting the created threads to terminate
    for(int i = 0; i < num_of_files; i++)
    {
        int res = pthread_join(child_threads[i], NULL);

        if(res != 0)
        {
            perror("ERROR: Cannot join thread\n");
            exit(1);
        }
    }
    handleMain();

    // Done
    printf("\nThreads are finished. Check the '%s'\n", out_file_name);
}

void *handleChild(void *arg)
{
    // Thread id
    int index = (intptr_t) arg;

    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char path[MAX_SIZE_FILENAME] = "input/";
    strcat(path, in_file_names[index]);

    // File open for read
    f = fopen(path, "r");
    if(f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    // Reading and putting every line into an array
    int hist_data [bin_count];
    float bin_width = (max_value - min_value) / bin_count;

    for(int i = 0; i < bin_count; i++)
        hist_data[i] = 0;

    while ((read = getline(&line, &len, f)) != -1)
    {
        float num = atoi(line);

        for(int i = 0; i < bin_count; i++)
        {
            float lower = (min_value + i * bin_width);
            float upper = (min_value + (i + 1) * bin_width);

            if((num >= lower && num < upper) || ((i == bin_count - 1) && (num >= lower && num <= upper) ))
            {
                hist_data[i]++;
                break;
            }
        }
    }

    // After finishing our job with file
    fclose(f);
    if (line) free(line);

    // putting results into the global array
    for(int i = 0; i < bin_count; i++)
    {
        in_data[index][i] = hist_data[i];
    }
    pthread_exit(0);
}

void handleMain()
{
    // Combining the results
    int out_data[bin_count];
    for(int i = 0; i < bin_count; i++)
        out_data[i] = 0;

    for(int i = 0; i < num_of_files; i++)
    {
        for(int j = 0; j < bin_count; j++)
        {
            out_data[j] += in_data[i][j];
        }
    }

    // Result txt in results folder
    char out_path[MAX_SIZE_FILENAME] = "results/";
    strcat(out_path, out_file_name);

    // File open for writing
    FILE *f = fopen(out_path, "w");
    if(f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    for(int i = 0; i < bin_count; i++)
    {
        fprintf(f, "%d : %d\n", (i + 1), out_data[i]);
    }

    fclose(f);
}
