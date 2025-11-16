/*
A01:calloc(v->size, sizeof(void *))
A02: v->free_slot
A03: realloc(v->data, v->size * sizeof(void *))
A04: v->data[--v->count]
A05: v->data[i]
A06: cb(v->data[i], data)
A07: p = vector_pop(v)
A08: dc(p)
A09: 至少 40 個英文單詞構成的回覆，要有明確的 C 程式碼和提及實作上的好處
*/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VECTOR_MIN_SIZE 16

typedef struct {
    void **data;
    size_t size;      /* Allocated size */
    size_t count;     /* Number of elements */
    size_t free_slot; /* Index of a known hole */
} vector_t;

typedef void (*vector_delete_callback_t)(void *);
typedef void *(*vector_foreach_callback_t)(void *, void *);

/* Initialize vector */
void vector_init(vector_t *v)
{
    v->data = NULL;
    v->size = 0;
    v->count = 0;
    v->free_slot = 0;
}

/* Push element to vector, returns index */
int32_t vector_push(vector_t *v, void *ptr)
{
    if (!v->size) {
        v->size = VECTOR_MIN_SIZE;
        v->data = calloc(v->size, sizeof(void *)); //A01
    }

    /* Reuse free slot if available */
    if (v->free_slot && v->free_slot < v->count) {
        size_t idx = v->free_slot;
        v->data[idx] = ptr;
        v->free_slot = 0;
        return idx;
    }

    /* Resize if needed */
    if (v->count == v->size) {
        v->size *= 2;
        v->data = realloc(v->data, v->size * sizeof(void *)); //A03
        memset(v->data + v->count, 0, (v->size - v->count) * sizeof(void *));
    }

    v->data[v->count] = ptr;
    return v->count++;
}

/* Pop last element */
void *vector_pop(vector_t *v)
{
    if (!v->count)
        return NULL;
    void *last = v->data[--v->count]; //A04
    v->data[v->count] = NULL;
    return last;
}

/* Get element at index */
void *vector_get_at(vector_t *v, size_t index)
{
    return (index < v->count) ? v->data[index] : NULL;
}

/* Set element at index (replaces existing) */
void *vector_set_at(vector_t *v, size_t index, void *ptr)
{
    if (index >= v->count)
        return NULL;
    v->data[index] = ptr;
    return ptr;
}

/* Get last element */
void *vector_get_end(vector_t *v)
{
    return v->count ? v->data[v->count - 1] : NULL;
}

/* Delete at index (creates hole) */
void vector_delete_at(vector_t *v, size_t index)
{
    if (index < v->count) {
        v->data[index] = NULL;
        v->free_slot = index;
    }
}

/* Iterate over elements */
void *vector_for_each(vector_t *v, vector_foreach_callback_t cb, void *data)
{
    if (!cb)
        return NULL;

    for (size_t i = 0; i < v->count; i++) {
        if (!v->data[i]) //A05
            continue;
        void *ret = cb(v->data[i], data); //A06
        if (ret)
            return ret;
    }
    return NULL;
}

/* Delete all elements */
void vector_delete_all(vector_t *v, vector_delete_callback_t dc)
{
    for (void *p; (p = vector_pop(v));) { //A07
        if (dc)
            dc(p); //A08
    }
}

/* Free vector memory */
void vector_free(vector_t *v)
{
    if (!v->data)
        return;

    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->count = 0;
    v->free_slot = 0;
}

/* Get number of elements */
size_t vector_used(vector_t *v)
{
    return v->count;
}