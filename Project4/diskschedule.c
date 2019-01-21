/*
    Operating Systems: Disk Scheduler Simulation

    author: Fuad Aghazada
    date: 17/12/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "input_generator.h"
#include "queue.h"
#include "statistics.h"

#define OUT_FILENAME "result.txt"
#define NUM_OF_CYLINDERS 5000
#define NUM_OF_REQUESTS  1000
#define MAX_SIZE_FILENAME 100

// Function Prototypes
void fill_queue_file(char file_name[MAX_SIZE_FILENAME]);
void fill_queue_rand();
void sch_algos(int head_pos, int *fcfs, int *sstf, int *scan, int *c_scan, int *look, int *c_look);

int FCFS(int head_pos);
int SSTF(int head_pos);
int SCAN(int head_pos);
int C_SCAN(int head_pos);
int LOOK(int head_pos);
int C_LOOK(int head_pos);

// Helpers
int sstf_helper(int *array, int size, int key, int *index);
void swap(int *a, int *b);
void sort(int arr[], int size);

// Main to execute
int main(int argc, char **argv)
{
    // Clean up at the beginning
    clear_queue();
    clearRandomNumbers();

    // Checking program parameters
    if(argc <= 1 || argc > 3)
    {
        printf("ERROR: Inproper number of arguments, please check the parameters again.\n");
        return -1;
    }

    // No input file
    if(argc == 2)
    {
        fill_queue_rand();
    }
    else if(argc == 3)
    {
        fill_queue_file(argv[2]);
    }

    int fcfs, sstf, scan, c_scan, look, c_look;
    sch_algos(atoi(argv[1]), &fcfs, &sstf, &scan, &c_scan, &look, &c_look);

    // Clean up at the end
    clear_queue();
    clearRandomNumbers();

    return 0;
}

/**
    Function to call all Disk Scheduling algorithms
*/
void sch_algos(int head_pos, int *fcfs, int *sstf, int *scan, int *c_scan, int *look, int *c_look)
{
    if(list_size <= 1)
        return;

    FILE *f = fopen(OUT_FILENAME, "w");

    *fcfs = FCFS(head_pos);
    *sstf = SSTF(head_pos);
    *scan = SCAN(head_pos);
    *c_scan = C_SCAN(head_pos);
    *look = LOOK(head_pos);
    *c_look = C_LOOK(head_pos);

    // File Write
    fprintf(f, "FCFS: %d\n", *fcfs);
    fprintf(f, "SSTF: %d\n", *sstf);
    fprintf(f, "SCAN: %d\n", *scan);
    fprintf(f, "C-SCAN: %d\n", *c_scan);
    fprintf(f, "LOOK: %d\n", *look);
    fprintf(f, "C-LOOK: %d\n", *c_look);

    // Console Print
    printf("FCFS: %d\n", *fcfs);
    printf("SSTF: %d\n", *sstf);
    printf("SCAN: %d\n", *scan);
    printf("C-SCAN: %d\n", *c_scan);
    printf("LOOK: %d\n", *look);
    printf("C-LOOK: %d\n\n", *c_look);
}

/** DISK SCHEDULING ALGORITHMS **/

// First Come First Served
int FCFS(int head_pos)
{
    struct node *cur = q_head;
    struct node *pre = q_head;

    int total_movement = abs(q_head->value - head_pos);

    while(cur != NULL)
    {
        total_movement += abs(cur->value - pre->value);

        pre = cur;
        cur = cur->next;
    }

    return total_movement;
}

// Shortest Seek Time First
int SSTF(int head_pos)
{
    struct node *cur = q_head;

    int total_movement = 0;

    int i;
    int requests [list_size + 1];
    requests[0] = head_pos;
    for(i = 1; i < list_size + 1; i++, cur = cur->next)
    {
        requests[i] = cur->value;
    }

    i = 0;
    int index = 0;
    int *p = &index;

    while(1)
    {
        int min = sstf_helper(requests, list_size + 1, i, p);
        requests[i] = -1;
        if(min == -1)
            break;
        total_movement += min;
        i = *p;
    }

    return total_movement;
}

// Scan/Elevator
int SCAN(int head_pos)
{
    struct node *cur = q_head;

    int total_movement = 0;

    // Putting every node into array
    int i;
    int requests [list_size + 1];
    requests[0] = head_pos;
    for(i = 1; i < list_size + 1; i++, cur = cur->next)
    {
        requests[i] = cur->value;
    }

    // Sorting the request array
    sort(requests, list_size + 1);

    // Position of head_pos
    int h_index;
    for(h_index = 0; h_index < list_size + 1; h_index++)
        if(requests[h_index] == head_pos)
            break;

    total_movement += abs(requests[h_index] - (NUM_OF_CYLINDERS - 1));
    total_movement = (h_index != 0) ? total_movement + abs(requests[0] - (NUM_OF_CYLINDERS - 1)) : total_movement;

    return total_movement;
}

// Circular Scan
int C_SCAN(int head_pos)
{
    struct node *cur = q_head;

    int total_movement = 0;

    // Putting nodes into array
    int i;
    int requests [list_size + 1];
    requests[0] = head_pos;
    for(i = 1; i < list_size + 1; i++, cur = cur->next)
    {
        requests[i] = cur->value;
    }

    // Sorting
    sort(requests, list_size + 1);

    // Position of head_pos
    int h_index;
    for(h_index = 0; h_index < list_size + 1; h_index++)
        if(requests[h_index] == head_pos)
            break;

    total_movement += abs(requests[h_index] - (NUM_OF_CYLINDERS - 1));

    if(h_index != 0)
    {
        total_movement += (NUM_OF_CYLINDERS - 1);
        total_movement += (abs(requests[h_index - 1]));
    }

    return total_movement;
}

// Look
int LOOK(int head_pos)
{
    struct node *cur = q_head;

    int total_movement = 0;

    // Putting nodes into array
    int i;
    int requests [list_size + 1];
    requests[0] = head_pos;
    for(i = 1; i < list_size + 1; i++, cur = cur->next)
    {
        requests[i] = cur->value;
    }

    // Sorting
    sort(requests, list_size + 1);

    // Position of head_pos
    int h_index;
    for(h_index = 0; h_index < list_size + 1; h_index++)
        if(requests[h_index] == head_pos)
            break;

    total_movement += abs(requests[h_index] - requests[list_size]);
    total_movement = (h_index != 0) ? total_movement + abs(requests[0] - requests[list_size]) : total_movement;

    return total_movement;
}

// Circular Look
int C_LOOK(int head_pos)
{
    struct node *cur = q_head;

    int total_movement = 0;

    // Putting nodes into array
    int i;
    int requests [list_size + 1];
    requests[0] = head_pos;
    for(i = 1; i < list_size + 1; i++, cur = cur->next)
    {
        requests[i] = cur->value;
    }

    // Sorting
    sort(requests, list_size + 1);

    // Position of head_pos
    int h_index;
    for(h_index = 0; h_index < list_size + 1; h_index++)
        if(requests[h_index] == head_pos)
            break;

    total_movement += (abs(requests[h_index] - requests[list_size]));

    if(h_index != 0)
    {
        total_movement += abs(requests[list_size] - requests[0]);
        total_movement += (abs(requests[0] - requests[h_index - 1]));
    }

    return total_movement;
}

/**
    This function will read the value from file and inserts (enqueue) to the queue
*/
void fill_queue_file(char file_name[MAX_SIZE_FILENAME])
{
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // File open
    f = fopen(file_name, "r");
    if (f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    // Reading the file
    while ((read = getline(&line, &len, f)) != -1)
    {
        char *splitted = strtok(line, " ");

        // Request number
        int req_num = (splitted != NULL) ? atoi(splitted) : -1;

        // Error check
        if(req_num == -1)
        {
            printf("ERROR: Wrong format inside the file\n");
            return;
        }

        splitted = strtok(NULL, " ");

        // Request
        int request = (splitted != NULL) ? atoi(splitted) : -1;

        // Error check
        if(request == -1)
        {
            printf("ERROR: Wrong format inside the file\n");
            return;
        }

        enqueue(request);
    }

    fclose(f);
}

/**
    This function will generate random numbers  and inserts them (enqueue) to the queue
*/
void fill_queue_rand()
{
    seed();     // For different random numbers

    int i;
    for(i = 0; i < NUM_OF_REQUESTS; i++)
    {
        int rand_num = generateRandomNumber(0, NUM_OF_CYLINDERS - 1);
        enqueue(rand_num);
    }
}

/********* Helper functions ***********/

/**
    Helper method for SSTF algorithm
*/
int sstf_helper(int *array, int size, int key, int *index)
{
    int diff[size];

    int i;
    for(i = 0; i < size; i++)
    {
        if(array[i] != -1)
        {
            diff[i] = abs(array[i] - array[key]);
        }
        else
        {
            diff[i] = NUM_OF_CYLINDERS;
        }
    }

    int min = NUM_OF_CYLINDERS;
    for(i = 0; i < size; i++)
    {
        if(diff[i] < min && diff[i] > 0)
        {
            min = diff[i];
            *index = i;
        }
    }

    return (min == NUM_OF_CYLINDERS) ? -1 : min;
}

/**
    Selection sort algorithm

    @param arr: array to be sorted
    @param size: size of the array
*/
void sort(int arr[], int size)
{
    int i, j, index;

    for (i = 0; i < size - 1; i++)
    {
        index = i;
        for (j = i + 1; j < size; j++)
          if (arr[j] < arr[index])
            index = j;

        swap(&arr[index], &arr[i]);
    }
}

/**
    Swaps to integers
*/
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
