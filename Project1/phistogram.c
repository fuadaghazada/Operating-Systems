/*
    Operating Systems - Project 1 : Processes and Threads (Part 1)

    author: Fuad Aghazada
    date: 08/10/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

// Constants
#define MAX_NUM_OF_FILES 1000
#define MAX_SIZE_FILENAME 100

// Syscall prototypes
pid_t waitpid(pid_t pid, int *status, int options);

// Function Prototypes
void handleChild(int k, float min_value, float max_value, int bin_count, char file_name[MAX_SIZE_FILENAME]);
void handleProcesses(float min_value, float max_value, int bit_count, int num_of_files, char in_file_names [MAX_NUM_OF_FILES][MAX_SIZE_FILENAME], char out_file_name [MAX_SIZE_FILENAME]);
void handleParent(int s, int bin_count, int num_of_files, char out_file_name[MAX_SIZE_FILENAME]);


// Main for executing
int main(int argc, char **argv)
{
    // Checking arguments
    if(argc < 6)
    {
        printf("Insufficient number of parameters. \nRequired format: \nphistogram minvalue maxvalue bincount N file1 … fileN outfile\n");
        return -1;
    }

    // Input values - got/parse/process
    float min_value = atof(argv[1]);
    float max_value = atof(argv[2]);
    int bin_count = atoi(argv[3]);
    int num_of_files = atoi(argv[4]);

    char in_file_names [num_of_files][MAX_SIZE_FILENAME];
    char out_file_name [MAX_SIZE_FILENAME];

    // Putting filenames coming from input into the array
    for(int i = 0; i < num_of_files; i++)
    {
        strcpy(in_file_names[i], argv[5 + i]);
    }
    strcpy(out_file_name, argv[num_of_files + 5]);

    // ---- Cleaning out file
    char out_path[MAX_SIZE_FILENAME] = "results/";
    strcat(out_path, out_file_name);

    FILE *f = fopen(out_path, "w");
    for(int j = 0; j < bin_count; j++)
        fprintf(f, "%d\n", 0);

    fclose(f);

    // ------- Calling primary function ----------------
    struct timeval start_time, end_time;
    float elapsed_time;

    gettimeofday(&start_time, NULL);
    handleProcesses(min_value, max_value, bin_count, num_of_files, in_file_names, out_file_name);
    gettimeofday(&end_time, NULL);

    // Calculating elapsed time in microseconds
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000;
    elapsed_time += (end_time.tv_usec - start_time.tv_usec);

    printf("\n\nElapsed time for multi-process application: %f ms\n", elapsed_time);

    return 0;
}

/*
    Handles the parents and child processes
*/
void handleProcesses(float min_value, float max_value, int bin_count, int num_of_files, char in_file_names [MAX_NUM_OF_FILES][MAX_SIZE_FILENAME], char out_file_name [MAX_SIZE_FILENAME])
{
    pid_t child_pids[num_of_files];

    // Creating child processes of main process
    for(int i = 0; i < num_of_files; i++)
    {
        child_pids[i] = fork();
        if(child_pids[i] < 0)
        {
            perror("ERROR");
            exit(1);
        }
        else if(child_pids[i] == 0)
        {
            handleChild(i, min_value, max_value, bin_count, in_file_names[i]);
            exit(0);
        }
    }

    // Waiting for childs to terminate
    for(int i = 0; i < num_of_files; i++)
    {
        waitpid(child_pids[i], NULL, 0);
    }

    // Combining the child result from
    for(int i = 0; i < num_of_files; i++)
    {
        handleParent(i, bin_count, num_of_files, out_file_name);
    }
    printf("\nProcesses are finished. Check the '%s'\n", out_file_name);
}

/*
    Handles a child process
*/
void handleChild(int k, float min_value, float max_value, int bin_count, char file_name[MAX_SIZE_FILENAME])
{
    // File components
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char path[MAX_SIZE_FILENAME] = "input/";
    strcat(path, file_name);

    // File open
    f = fopen(path, "r");
    if (f == NULL)
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
        float num = atof(line);

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

    // Wrting to a file
    char str_index[10];
    char w_path[MAX_SIZE_FILENAME] = "output/output";
	sprintf(str_index, "%d", (k + 1));
    strcat(str_index, ".txt");
	strcat(w_path, str_index);
    f = fopen(w_path, "w");

    if(f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    for(int i = 0; i < bin_count; i++)
    {
        fprintf(f, "%d\n", hist_data[i]);
    }

    // After finishing our job with file
    fclose(f);
    if (line) free(line);
}

/*
    Handles parent process
*/
void handleParent(int s, int bin_count, int num_of_files, char out_file_name[MAX_SIZE_FILENAME])
{
    // File components
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Child output file names from input
    char str_index[10];
    char path[MAX_SIZE_FILENAME] = "output/output";
    sprintf(str_index, "%d", (s + 1));
    strcat(str_index, ".txt");
    strcat(path, str_index);

    // Child output File open
    f = fopen(path, "r");
    if (f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    // Reading input file
    int input_data[bin_count];
    int k = 0;
    while ((read = getline(&line, &len, f)) != -1)
    {
        input_data[k++] = atoi(line);
    }

    // ----------------------------------------
    char out_path[MAX_SIZE_FILENAME] = "results/";
    strcat(out_path, out_file_name);

    f = fopen(out_path, "r");
    if (f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    // Reset read properties
    ssize_t read1;
    line = NULL;
    len = 0;

    k = 0;
    int cur[bin_count];
    while ((read1 = getline(&line, &len, f)) != -1)
    {
        cur[k++] = atoi(line);
    }

    fclose(f);
    if (line) free(line);

    // Result txt in results folder
    f = fopen(out_path, "w");
    for(int j = 0; j < bin_count; j++)
    {
        if(s == num_of_files -  1)
            fprintf(f, "%d : %d\n", (j + 1), cur[j] + input_data[j]);
        else
            fprintf(f, "%d\n", cur[j] + input_data[j]);
    }
    fclose(f);
}
