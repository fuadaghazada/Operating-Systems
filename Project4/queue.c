/**
    Source file for Queue implementation

    author: Fuad Aghazada
    date: 17/12/2018
*/

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

// Helper function prototypes
void insert_(int value, int place);
int remove_(int place);

// Initializing properties
struct node *q_head = NULL;
int list_size = 0;

/**
    Print content of linked list (For debugging purposes)
*/
void print_queue()
{
    printf("Content of Queue: ");
    struct node *cur = q_head;
    while(cur != NULL)
    {
        printf("%d\t", cur->value);
        cur = cur->next;
    }
    printf("\nSize: %d\n", list_size);
}

/**
    Inserts element at the end of the list (queue)
*/
void enqueue(int value)
{
    insert_(value, 1);
}

/**
    Removes the first element in the list (queue)
*/
int dequeue()
{
    return remove_(0);
}

/**
    Returns First in the list (queue)
*/
int peek()
{
    return (list_size > 0) ? q_head->value : -1;
}

/**
    For clearing the list (cleaning the memory)
*/
void clear_queue()
{
    struct node *cur = q_head;
    struct node *next;

    while(cur != NULL)
    {
        next = cur->next;
        free(cur);
        cur = next;
    }
    q_head = NULL;
    list_size = 0;
}

// ***** Helper methods *******
/*
    Inserting element at the beginning or end of the list

    place: 0 - beginning
    else - end
*/
void insert_(int value, int place)
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
        node->next = q_head;
        q_head = node;
    }
    else
    {
        node->next = NULL;
        if(q_head != NULL)
        {
            struct node *cur = q_head;
            while(cur->next != NULL)
            {
                cur = cur->next;
            }
            cur->next = node;
        }
        else
        {
            q_head = node;
        }
    }
    list_size++;
}

/*
    Removing element from the beginning or end of the list

    place: 0 - beginning
    else - end
*/
int remove_(int place)
{
    struct node *temp = q_head;

    // One element in the list
    if(q_head == NULL)
    {
        perror("ERROR: Queue is empty\n");
        exit(1);
    }
    else if(list_size == 1)
    {
        int val = q_head->value;

        q_head = NULL;
        free(temp);
        list_size--;

        return val;
    }
    else
    {
        int val = -1;
        if(place == 0)
        {
            val = q_head->value;
            q_head = temp->next;
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
