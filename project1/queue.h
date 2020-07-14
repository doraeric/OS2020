#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct Node {
    struct Node * next;
    int data;
} Node;

typedef struct Queue {
    Node * head;
    Node * tail;
} Queue;

Queue * new_Queue();
void delete_Queue(Queue * q);
void Queue_push(Queue * q, int data);
int Queue_pop(Queue * q);
int Queue_empty(Queue * q);

#endif // _QUEUE_H
