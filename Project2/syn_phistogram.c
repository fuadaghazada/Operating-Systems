/*
    Operating Systems - Project 2 : Synchronized MultiProcesses (Part A)

    author: Fuad Aghazada
    date: 29/10/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

// Constants
#define MAX_NUM_OF_FILES 10000
#define MAX_SIZE_FILENAME 100
#define DEBUG 0

#define SHM_NAME "/histogram_data"
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"

// Syscall prototypes
pid_t waitpid(pid_t pid, int *status, int options);
int shm_open(const char *name, int oflag, mode_t mode);
int shm_unlink(const char *name);

// Function Prototypes
void handleChild(int k, double min_value, double max_value, int bin_count, char file_name[MAX_SIZE_FILENAME], int* hist_data);
void handleProcesses(double min_value, double max_value, int bit_count, int num_of_files, char in_file_names [MAX_NUM_OF_FILES][MAX_SIZE_FILENAME], char out_file_name [MAX_SIZE_FILENAME]);
void *createSharedMemory(int size);
sem_t *createSemaphore(char name[MAX_SIZE_FILENAME], int value);

// Main for executing
int main(int argc, char **argv)
{
    // Checking arguments
    if(argc < 6)
    {
        printf("Insufficient number of parameters. \nRequired format: \nsyn_phistogram minvalue maxvalue bincount N file1 … fileN outfile\n");
        return -1;
    }

    // Input values - got/parse/process
    double min_value = atof(argv[1]);
    double max_value = atof(argv[2]);
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

    // ------- Calling primary function ----------------
    struct timeval start_time, end_time;
    double elapsed_time;

    gettimeofday(&start_time, NULL);
    handleProcesses(min_value, max_value, bin_count, num_of_files, in_file_names, out_file_name);
    gettimeofday(&end_time, NULL);

    // Calculating elapsed time in microseconds
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000;
    elapsed_time += (end_time.tv_usec - start_time.tv_usec);

    printf("\n\nElapsed time for syn-multi-process application: %f ms\n", elapsed_time);

    return 0;
}

/*
    Creates a shared memory with the given size
*/
void *createSharedMemory(int size)
{
    // Shared Mem variables
    int shm_fd;                 // File Descriptor for shared mem
    struct stat sbuf;           // SHM info
    void *shm_start;            // Pointer to start of SHM

    // Cleaning SHM
    shm_unlink(SHM_NAME);

    // Creating SHM
    shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
    if(shm_fd < 0)
    {
        perror("ERROR: Cannot create a Shared Memory\n");
        exit(1);
    }

    // Setting an enough size to SHM
    ftruncate(shm_fd, size * sizeof(int));
    fstat(shm_fd, &sbuf);
    shm_start = mmap(NULL, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if(shm_start < 0)
    {
        perror("ERROR: Cannot map the Shared Memory\n");
        exit(1);
    }

    close(shm_fd);

    return shm_start;
}

/*
    Creates a semaphore with the given name and initial value
*/
sem_t *createSemaphore(char name[MAX_SIZE_FILENAME], int value)
{
    sem_unlink(name);
    sem_t *semaphore = sem_open(name, O_RDWR | O_CREAT, 0660, value);

    if(semaphore < 0)
    {
        perror("ERROR: Cannot create a semaphore");
        exit(1);
    }

    return semaphore;
}

/*
    Handles the parents and child processes
*/
void handleProcesses(double min_value, double max_value, int bin_count, int num_of_files, char in_file_names [MAX_NUM_OF_FILES][MAX_SIZE_FILENAME], char out_file_name [MAX_SIZE_FILENAME])
{
    // Final data will be put in this array - array is filled with 0's
    int final_histogram [bin_count];
    for(int i = 0; i < bin_count; i++)
    {
        final_histogram[i] = 0;
    }

    // Creating SHM ------------------------
    struct shared_data
    {
        int buf[bin_count];
        int count;
        int in;
        int out;
    };
    struct shared_data *shm_data_ptr = (struct shared_data *) createSharedMemory(bin_count);

    // Setting properties for Shared data
    for(int i = 0; i < bin_count; i++)
    {
        shm_data_ptr->buf[i] = 0;
    }
    shm_data_ptr->count = 0;
    shm_data_ptr->in = 0;

    // Creating Semaphores ------------------------
    sem_t *sem_mutex;   // Lock for protecting SHM between parent and child
    sem_t *sem_empty;   // Lock for protecting SHM among children

    sem_mutex = createSemaphore(SEM_MUTEX, 0);
    sem_empty = createSemaphore(SEM_EMPTY, 1);

    // Creating child processes of main process ------------------------
    pid_t child_pids[num_of_files];

    for(int i = 0; i < num_of_files; i++)
    {
        child_pids[i] = fork();
        if(child_pids[i] < 0)
        {
            perror("ERROR: Cannot create child process");
            exit(1);
        }
        else if(child_pids[i] == 0)
        {
            int result [bin_count]; // histogram data of child

            handleChild(i, min_value, max_value, bin_count, in_file_names[i], result);

            if(DEBUG)
            {
                printf("Child+ %d: ", i);
                for(int k = 0; k < bin_count; k++)
                    printf("%d\t", result[k]);
                printf("\n");
            }

            int counter = 0;
            while(1)
            {
                sem_wait(sem_empty);
                while(counter < bin_count)
                {
                    // Critical Section
                    shm_data_ptr->buf[shm_data_ptr->in] = result[counter];
                    shm_data_ptr->in = (shm_data_ptr->in + 1) % bin_count;
                    counter++;
                }
                sem_post(sem_mutex);
                break;
            }
            exit(0);
        }
    }

    // Parent
    for(int i = 0; i < num_of_files; i++)
    {
        if(DEBUG)
            printf("Child- %d: ", i);

        int counter = 0;
        while(1)
        {
            sem_wait(sem_mutex);    // Should wait for child
            while(counter < bin_count)
            {
                // Critical Section
                int value = shm_data_ptr->buf[shm_data_ptr->out];
                final_histogram[counter] += value;      // Putting the value into the final array
                if(DEBUG)
                    printf("%d\t", value);
                shm_data_ptr->out = (shm_data_ptr->out + 1) % bin_count;
                counter++;
            }
            if(DEBUG)
                printf("\n");
            sem_post(sem_empty);
            break;
        }
    }

    // Result txt in results folder
    char out_path[MAX_SIZE_FILENAME] = "results/";
    strcat(out_path, out_file_name);

    FILE *f = fopen(out_path, "w");
    for(int j = 0; j < bin_count; j++)
    {
        fprintf(f, "%d: %d\n", (j + 1), final_histogram[j]);
    }
    fclose(f);

    // Cleaning: Closing/Unlinking mutex and SHM
    sem_close(sem_mutex);
    sem_close(sem_empty);

    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_EMPTY);

    sem_destroy(sem_mutex);
    sem_destroy(sem_empty);

    shm_unlink(SHM_NAME);

    // End
    printf("\nProcesses are finished. Check the '%s'\n", out_file_name);
}

/*
    Handles a child process
*/
void handleChild(int k, double min_value, double max_value, int bin_count, char file_name[MAX_SIZE_FILENAME], int *hist_data)
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
    double bin_width = (max_value - min_value) / bin_count;

    for(int i = 0; i < bin_count; i++)
        hist_data[i] = 0;

    while ((read = getline(&line, &len, f)) != -1)
    {
        double num = atof(line);

        for(int i = 0; i < bin_count; i++)
        {
            double lower = (min_value + i * bin_width);
            double upper = (min_value + (i + 1) * bin_width);

            if((num >= lower && num < upper) || ((i == bin_count - 1) && (num >= lower && num <= upper) ))
            {
                hist_data[i]++;
                break;
            }
        }
    }
}
