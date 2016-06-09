#include <stdlib.h>

#include "nqueue.h"

struct node {
    struct node *next;
    struct node *prev;
    void *data;
    int type;
};
#define SENTINEL_NODE   1
#define DATA_NODE       0

static inline struct node *node_new(void *d, int t)
{
    struct node *n = calloc(1, sizeof*n);
    n->data = d;
    n->type = t;
    return n;
}

#define node_set_next(no, ne) (no)->next = (ne)
#define node_set_prev(no, ne) (no)->prev = (ne)
#define node_set_data(no, ne) (no)->data = (ne)
#define node_get_next(no) (no)->next
#define node_get_prev(no) (no)->prev
#define node_get_data(no) (no)->data

#define node_free(no) free((no))

    
struct queue {
    int size;
    struct node *head; // sentinel
    struct node *tail; // sentinel
};

struct queue* nqueue_create(void)
{
    struct queue *queue = malloc(sizeof(*queue));
    queue->head = node_new(NULL, SENTINEL_NODE);
    queue->tail = node_new(NULL, SENTINEL_NODE);
    node_set_prev(queue->head, NULL);
    node_set_next(queue->head, queue->tail);
    node_set_prev(queue->tail, queue->head);
    node_set_next(queue->tail, NULL);
    queue->size = 0;
    return queue;
}

size_t nqueue_size(struct queue *queue)
{
    return queue->size;
}

void nqueue_destroy(struct queue *queue)
{
    while (nqueue_size(queue) > 0)
	nqueue_pop(queue);
    node_free(queue->head);
    node_free(queue->tail);
    free(queue);
}

void *nqueue_head(struct queue *queue)
{
    return node_get_data(queue->head);
}

void nqueue_push(struct queue *queue, void *data)
{
    struct node *new = node_new(data, DATA_NODE);
    struct node *last = node_get_prev(queue->tail);
    node_set_prev(new, last);
    node_set_next(last, new);
    node_set_prev(queue->tail, new);
    node_set_next(new, queue->tail);
    queue->size ++;
}

void *nqueue_pop(struct queue *queue)
{
    struct node *first = node_get_next(queue->head);
    struct node *second = node_get_next(first);
    node_set_next(queue->head, second);
    node_set_prev(second, queue->head);
    void *data = node_get_data(first);
    node_free(first);
    queue->size --;
    return data;
}
