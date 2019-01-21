/*
 * Test Application for testing VM info
 * @author: Fuad Aghazada & Can Ozgurel
 * @date: 01.12.2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>


#define MAX_HEAP_ALLOC 1000

// TO keep pointers to allocated areas of heap for deallocating later
int k = 0;  // currently no pointer
int *pointers[MAX_HEAP_ALLOC];

/* Prototypes */
long int alloc_mem_stack(int n);
void alloc_mem_heap(int index, int size);
void dealloc_mem_heap(int index);

void print_choices();

// Main for executing
int main(int argc, char **argv)
{
    // Choice from menu
    char choice[3];
    int c;

    // Programm Loop
    do {
        print_choices();
        scanf("%s", choice);
        c = atoi(choice);

        if(c == 1)
        {
            // ---------- Stack allocation -----------
            printf("Please Enter how much you want to iterate: \n");

            char num_char[100];
            scanf("%s", num_char);
            int n = atoi(num_char);

            alloc_mem_stack(n);

            printf("Returned!\n");
        }
        else if(c == 2)
        {
            // ---------- Heap allocation -----------
            printf("Please Enter how much you want to allocate (current index: %d): \n", k);

            char num_char[100];
            scanf("%s", num_char);
            int size = atoi(num_char);

            // According to threshold
            for(int i = 0; i < 5; i++)
                alloc_mem_heap(k++, size / 5);

            printf("Allocation is successful! \n");
        }
        else if(c == 3)
        {
            // ---------- Heap deallocation -----------
            if(k == 0)
            {
                printf("No Heap allocation has been done! Please Allocate first!\n");
            }
            else
            {
                for(int i = 0; i < 5; i++)
                    dealloc_mem_heap(k--);
                printf("Deallocation is successful! \n");
            }
        }
        else
        {
            if(c != 4)
                printf("Input is invalid!\n");
            else
                printf("Program is terminated!\n");
        }
    }
    while(c != 4);

    return 0;
}

/**
    This function will allocate memory in Stack (static)
    using recursion: factorial function
    (Since allocation is static we do not need to dealloc manually)
*/
long int alloc_mem_stack(int n)
{
    if(n >= 1)
    {
        return (n * alloc_mem_stack(n - 1));
    }
    printf("!!!!!!!!!!!!!!!!!!\n");
    printf("This is the last call.\nYou can view the stack before exiting the function.\nFor exit enter any key and Press Enter...\n");
    printf("!!!!!!!!!!!!!!!!!!\n");
    char key[5];
    scanf("%s", key);
    return 1;
}

/**
    This function will allocate memory in Heap (dynamic)
    using malloc() function of C:
    (Since allocation is dynamic we NEED to deallocate it)

    index: index in pointers array
    size: size for allocating the area
*/
void alloc_mem_heap(int index, int size)
{
    pointers[index] = malloc(size);
}

/**
    This function will deallocate memory in heap
    using free() function of C:
*/
void dealloc_mem_heap(int index)
{
    free(pointers[index]);
}

/**
    This function is just for printing menu on the console
*/
void print_choices()
{
    printf("---------------------\n");
    printf("app.c process ID: %d\n", getpid());
    printf("---------------------\n");
    printf("1. Test stack allocation.\n");
    printf("2. Test heap allocation.\n");
    printf("3. Test heap deallocation.\n");
    printf("4. Exit.\n");
    printf("---------------------\n");
}
