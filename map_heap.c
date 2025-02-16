//
// Created by Asia i Tomek on 01.01.2024.
//
#include "map_heap.h"
#include <stdlib.h>

void addToHeap(struct MapHeap* heap, struct Map* map) {
    if (heap->size < HEAP_SIZE) {
        heap->maps[heap->size++] = map;
    }
}

struct Map* getHeapElementByNumber(const struct MapHeap* heap, int element_num){
    return heap->maps[element_num];
}

struct Map* lookAtHeap(const struct MapHeap* heap) {
    if (heap->size > 0) {
        return heap->maps[heap->size - 1];
    }
    return NULL;
}

void addToTopMap(struct MapHeap* heap, const char* key, int value) {
    struct Map* topMap = lookAtHeap(heap);
    if (topMap != NULL) {
        addToMap(topMap, key, value);
    } else {
        printf("Heap is empty. Unable to add to the top map element %s.\n",key);
    }
}

void addToTopMapWithType(struct MapHeap* heap, const char* key, int value,char type) {
    struct Map* topMap = lookAtHeap(heap);
    if (topMap != NULL) {
        addToMapWithType(topMap, key, value,type);
    } else {
        //printf("Heap is empty. Unable to add to the top map element %s.\n",key);
    }
}

void addToTopMapUniqueWithType(struct MapHeap* heap, const char* key, int value,char type) {
    struct Map* topMap = lookAtHeap(heap);

    if (topMap != NULL) {
        if(!hasKey(topMap,key) && !hasKey(topMap,concat("*",key))){
            addToMapWithType(topMap, key, value, type);
        } else{
            yyerror("variable name duplication");
        }
    } else {
        printf("Heap is empty. Unable to add to the top map element %s.\n",key);
    }
}

struct Map* removeFromHeap(struct MapHeap* heap) {
    if (heap->size > 0) {
        struct Map* topMap = heap->maps[--heap->size];
        return topMap;
    }
    return NULL;
}
