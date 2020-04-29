#include <stdlib.h>
#include "queue.h"

void Queue_push(Queue * q, int data) {
    if (q->head != NULL) {
        Node * node = (Node *)malloc(sizeof(Node));
        node->next = NULL;
        node->data = data;
        q->tail->next = node;
        q->tail = node;
    }
    else {
        Node * node = (Node *)malloc(sizeof(Node));
        node->next = NULL;
        node->data = data;
        q->head = node;
        q->tail = node;
    }
}

int Queue_pop(Queue * q) {
    if (q->head == NULL) {
        return -1;
    }
    int ret = q->head->data;
    if (q->head == q->tail) {
        free(q->head);
        q->head = NULL;
        q->tail = NULL;
    } else {
        Node * tmp = q->head;
        q->head = q->head->next;
        free(tmp);
    }
    return ret;
}

int Queue_empty(Queue * q) {
    return q->head == NULL;
}

Queue * new_Queue() {
    Queue * q = (Queue *)malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void delete_Queue(Queue * q) {
    Node * node = q->head;
    Node * next;
    while(node) {
        next = node->next;
        free(node);
        node = next;
    }
    free(q);
}
