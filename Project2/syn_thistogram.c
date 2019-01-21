/*
Operating Systems - Project 2 : Threads with Synchronization (Part B)

author: Fuad Aghazada
date: 03/11/2018
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
#define DEBUG 0

// Function Prototypes
void handleThreads();
void *handleProducer(void *arg);
void *handleConsumer();
void linked_list_print();
void linked_list_insert(double value, int place);
double linked_list_remove(int place);
void linked_list_clear();

//*** Global ****
char out_file_name [MAX_SIZE_FILENAME];
char in_file_names [MAX_NUM_OF_FILES][MAX_SIZE_FILENAME];
double min_value, max_value;
int bin_count, num_of_files, batch_size;

// Head and size of GLOBAL linked list
struct node *head;
int list_size;

pthread_mutex_t mutex;
pthread_cond_t wait_consume;     // consumer waits
// **************

/*
    Linked list implementation part
*/
struct node
{
    double value;
    struct node *next;
};

// Main for executing
int main(int argc, char **argv)
{
    // Checking arguments
    if(argc < 7)
    {
        printf("Insufficient number of parameters. \nRequired format: \nthistogram minvalue maxvalue bincount N file1 … fileN outfile\n");
        return -1;
    }

    // Input values - got/parse/process
    min_value = atof(argv[1]);
    max_value = atof(argv[2]);
    bin_count = atoi(argv[3]);
    num_of_files = atoi(argv[4]);
    batch_size = atoi(argv[num_of_files + 6]);

    // Checking batch
    if(batch_size < 1 || batch_size > 100)
    {
        printf("Batch size is not in the proper range. Must be between 1 and 100\n");
        return -1;
    }

    // Putting filenames coming from input into the array
    for(int i = 0; i < num_of_files; i++)
    {
        strcpy(in_file_names[i], argv[5 + i]);
    }
    strcpy(out_file_name, argv[num_of_files + 5]);

    // ------- Linked list, mutex, condition variable initializing ----------
    head = NULL;
    list_size = 0;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&wait_consume, NULL);
    // ------- ---------


    // ------- Calling primary function + (time measuring)----------------
    struct timeval start_time, end_time;
    double elapsed_time;

    gettimeofday(&start_time, NULL);
    handleThreads();
    gettimeofday(&end_time, NULL);

    // Calculating elapsed time in microseconds
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000;
    elapsed_time += (end_time.tv_usec - start_time.tv_usec);

    // Clean up
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&wait_consume);

    linked_list_clear();

    // Time result
    printf("\n\nElapsed time for multi-thread application: %f ms\n", elapsed_time);

    return 0;
}

// Handling the thread business
void handleThreads()
{
    pthread_t consumer_thread;
    pthread_t producer_threads[num_of_files];

    // Creating the producer threads
    int res;
    for(int i = 0; i < num_of_files; i++)
    {
        res = pthread_create(&producer_threads[i], NULL, handleProducer, (void *)(intptr_t)i);
        if(res != 0)
        {
            perror("ERROR: Cannot create thread\n");
            exit(1);
        }
    }

    // Creating the consumer  thread
    res = pthread_create(&consumer_thread, NULL, handleConsumer, NULL);
    if(res != 0)
    {
        perror("ERROR: Cannot create thread\n");
        exit(1);
    }

    // Waiting the consumer thread to terminate
    res = pthread_join(consumer_thread, NULL);
    if(res != 0)
    {
        perror("ERROR: Cannot join thread\n");
        exit(1);
    }

    // Done
    printf("\nThreads are finished. Check the '%s'\n", out_file_name);
}

// Producer code
void *handleProducer(void *arg)
{
    // Index of the file
    int index = (intptr_t) arg;

    FILE *f;
    char *line = NULL;
    size_t len = 0;
    char path[MAX_SIZE_FILENAME] = "input/";
    strcat(path, in_file_names[index]);

    // File open for read
    f = fopen(path, "r");
    if(f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    // Measuring size of the file (# of elements in the file)
    int size = 0;
    while(getline(&line, &len, f) != -1)
        size++;
    fclose(f);

    // resetting reading params
    line = NULL;
    len = 0;
    f = fopen(path, "r");
    if(f == NULL)
    {
        printf("ERROR: Cannot open the file\n");
        exit(1);
    }

    // Iterating through the file
    int iterate = size;
    while (getline(&line, &len, f) != -1)
    {
        double num = atof(line);

        pthread_mutex_lock(&mutex);
        // Critical section: begins

        // Inseting into the linked list
        linked_list_insert(num, 1);
        iterate--;
        if(DEBUG)
            printf("Producer inserted: %f\n", num);

        if((list_size == batch_size) || (iterate == 0))
        {
            pthread_cond_signal(&wait_consume);
        }

        // Critical section: ends
        pthread_mutex_unlock(&mutex);
    }

    fclose(f);

    /* INSERTING A VALUE FOR DETECTING END OF FILE */

    pthread_mutex_lock(&mutex);
    // Critical section: begins

    // Inseting into the linked list
    linked_list_insert(min_value - 7, 1);
    if(DEBUG)
        printf("Producer inserted end of file \n");

    if(list_size == 1)
    {
        pthread_cond_signal(&wait_consume);
    }

    // Critical section: ends
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

// Consumer code
void *handleConsumer()
{
    // Histogram data array
    int hist_data [bin_count];
    double bin_width = (max_value - min_value) / bin_count;
    for(int i = 0; i < bin_count; i++)
        hist_data[i] = 0;

    int counter = 0;

    while(1)
    {
        pthread_mutex_lock(&mutex);
        // Critical section: begins

        while(list_size == 0)
        {
            pthread_cond_wait(&wait_consume, &mutex);
        }

        // Building the histogram
        double num = linked_list_remove(0);
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

        // In case remove is unsuccessful
        if(num == -1.0)
        {
            printf("ERROR: Cannot remove element from linked list\n");
            exit(1);
        }

        // Checking if the file iteration has been ended
        if(num < min_value)
        {
            counter++;
        }
        else
        {
            if(DEBUG)
                printf("Consumer got: %f\n", num);
        }

        // Critical section: begins
        pthread_mutex_unlock(&mutex);

        // To finish the infinite loop
        if(counter == num_of_files)
            break;
    }

    // Result txt in results folder
    char out_path[MAX_SIZE_FILENAME] = "results/";
    strcat(out_path, out_file_name);

    FILE *f = fopen(out_path, "w");
    for(int j = 0; j < bin_count; j++)
    {
        fprintf(f, "%d: %d\n", (j + 1), hist_data[j]);
    }
    fclose(f);

    pthread_exit(0);
}

/*
    LINKED LIST METHODS
*/

// Print content of linked list (For debugging purposes)
void linked_list_print()
{
    printf("Content of Linked list: ");
    struct node *cur = head;
    while(cur != NULL)
    {
        printf("%f\t", cur->value);
        cur = cur->next;
    }
    printf("\nSize: %d\n", list_size);
}

/*
    Inserting element at the beginning or end of the list

    place: 0 - beginning
    else - end
*/
void linked_list_insert(double value, int place)
{
    struct node *node = (struct node *)malloc(sizeof(struct node));

    // Error condition
    if(node == NULL)
    {
        perror("ERROR: Cannot create node\n");
        exit(1);
    }

    node->value = value;

    if(place == 0)
    {
        node->next = head;
        head = node;
    }
    else
    {
        node->next = NULL;
        if(head != NULL)
        {
            struct node *cur = head;
            while(cur->next != NULL)
            {
                cur = cur->next;
            }
            cur->next = node;
        }
        else
        {
            head = node;
        }
    }
    list_size++;
}

/*
    Removing element from the beginning or end of the list

    place: 0 - beginning
    else - end
*/
double linked_list_remove(int place)
{
    struct node *temp = head;

    // One element in the list
    if(head == NULL)
    {
        perror("ERROR: Linked list is empty\n");
        exit(1);
    }
    else if(list_size == 1)
    {
        double val = head->value;

        head = NULL;
        free(temp);
        list_size--;

        return val;
    }
    else
    {
        double val = -1.0;
        if(place == 0)
        {
            val = head->value;
            head = temp->next;
            free(temp);
        }
        else
        {
            struct node *prev;
            while(temp->next != NULL)
            {
                prev = temp;
                temp = temp->next;
            }
            prev->next = NULL;
            val = temp->value;
            free(temp);
        }
        list_size--;
        return val;
    }
}

// For clearing the list (cleaning the memory)
void linked_list_clear()
{
    struct node *cur = head;
    struct node *next;

    while(cur != NULL)
    {
        next = cur->next;
        free(cur);
        cur = next;
    }
    head = NULL;
    list_size = 0;
}
