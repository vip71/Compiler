//
// Created by Asia i Tomek on 05.01.2024.
//

#ifndef KOMPILATOR1_HEAP_H
#define KOMPILATOR1_HEAP_H

#include <stdlib.h>

struct Heap {
    unsigned long long *arr;
    size_t capacity;
    size_t size;
};

struct Heap* createHeap();
void push(struct Heap* heap, unsigned long long value);
unsigned long long pop(struct Heap* heap);
void destroyHeap(struct Heap* heap);

#define HEAP_CAPACITY 100000

#endif //KOMPILATOR1_HEAP_H
