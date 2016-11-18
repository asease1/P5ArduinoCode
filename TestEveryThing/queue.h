#include <stdlib.h>
/**
  * This sample is about how to implement a queue in c
  *
  * Type of item is int
  * Add item to tail
  * Get item from head
  * Can get the size
  * Can display all content
  */
  
/*
 * The Node struct,
 * contains item and the pointer that point to next node.
 */
typedef struct Node {
    void* item;
    struct Node* next;
} Node;

/**
 * The Queue struct, contains the pointers that
 * point to first node and last node, the size of the Queue,
 * and the function pointers.
 */
typedef struct Queue {
    Node* head;
    Node* tail;
    int sizeOfElement;
    void (*push) (struct Queue*, void*); // add item to tail
    // get item from head and remove it from queue
    void* (*pop) (struct Queue*);
    // get item from head but keep it in queue
    void* (*peek) (struct Queue*);
    // display all element in queue
    void (*display) (struct Queue*);
    // size of this queue
    int size;
} Queue;

/**
 * Push an item into queue, if this is the first item,
 * both queue->head and queue->tail will point to it,
 * otherwise the oldtail->next and tail will point to it.
 */
void push (Queue* queue, void* item) {
    // Create a new node
    Node* n = (Node*) malloc (sizeof(Node));
    n->item = malloc(queue->sizeOfElement);
    n->next = NULL;

    memcpy(n->item, item, queue->sizeOfElement);

    if (queue->head == NULL) { // no head
        queue->head = n;
    } else{
        queue->tail->next = n;
    }
    queue->tail = n;
    queue->size++;
}

/**
 * Return and remove the first item.
 */
void* pop (Queue* queue) {
    // get the first item
    Node* head = queue->head;
    void* item = head->item;
    // move head pointer to next node, decrease size
    queue->head = head->next;
    queue->size--;
    // free the memory of original head
    free(head);
    Serial.println((int)item);
    return item;
}

/**
 * Return but not remove the first item.
 */
void* peek (Queue* queue) {
    Node* head = queue->head;
    return head->item;
}

/**
 * Create and initiate a Queue
 */
Queue CreateQueue (int sizeOfElement) {
    Queue queue;
    queue.size = 0;
    queue.head = NULL;
    queue.tail = NULL;
    queue.push = &push;
    queue.pop = &pop;
    queue.peek = &peek;
    queue.sizeOfElement = sizeOfElement;
    return queue;
}
