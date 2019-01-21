/**
    Header file for Queue implementation

    author: Fuad Aghazada
    date: 17/12/2018
*/

#ifndef QUEUE_
#define QUEUE_

// Node structure
struct node
{
    int value;
    struct node *next;
};

// Properties
extern struct node *q_head;
extern int list_size;

// Functions
void print_queue();

void enqueue();
int dequeue();
int peek();

void clear_queue();

#endif
