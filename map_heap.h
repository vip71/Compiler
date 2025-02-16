//
// Created by Asia i Tomek on 01.01.2024.
//

#ifndef KOMPILATOR1_MAP_HEAP_H
#define KOMPILATOR1_MAP_HEAP_H
#include "map.h"
#include "strings.h"
#include <stdio.h>
#define HEAP_SIZE 10000
    extern void yyerror(const char*);

    // Structure to represent a heap of maps
    struct MapHeap {
        struct Map* maps[HEAP_SIZE];
        int size;
    };

    // Function prototypes for heap operations
    void addToHeap(struct MapHeap* heap, struct Map* map);
    struct Map* lookAtHeap(const struct MapHeap* heap);
    void addToTopMap(struct MapHeap* heap, const char* key, int value);
    void addToTopMapWithType(struct MapHeap* heap, const char* key, int value,char type);
    void addToTopMapUniqueWithType(struct MapHeap* heap, const char* key, int value,char type);
    struct Map* removeFromHeap(struct MapHeap* heap);

    struct Map* getHeapElementByNumber(const struct MapHeap* heap, int element_num);
#endif //KOMPILATOR1_MAP_HEAP_H
