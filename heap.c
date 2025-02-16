//
// Created by Asia i Tomek on 05.01.2024.
//

#include "heap.h"


struct Heap* createHeap() {
    struct Heap* heap = (struct Heap*)malloc(sizeof(struct Heap));

    if (heap == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    heap->arr = (unsigned long long*)malloc(HEAP_CAPACITY * sizeof(unsigned long long));

    if (heap->arr == NULL) {
        // Handle memory allocation failure
        free(heap);
        return NULL;
    }

    heap->capacity = HEAP_CAPACITY;
    heap->size = 0;

    return heap;
}

void push(struct Heap* heap, unsigned long long value) {
    if (heap->size == heap->capacity) {
        // Handle heap full scenario (resize or return an error)
        return;
    }

    heap->arr[heap->size] = value;
    heap->size++;
}

unsigned long long pop(struct Heap* heap) {
    if (heap->size == 0) {
        // Handle empty heap scenario (return a sentinel value or an error)
        return 0;  // Assuming 0 is a sentinel value; replace with appropriate logic
    }

    // Simply decrement the size to remove the last element
    heap->size--;

    return heap->arr[heap->size];
}

void destroyHeap(struct Heap* heap) {
    free(heap->arr);
    free(heap);
}