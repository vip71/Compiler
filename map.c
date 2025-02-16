//
// Created by Asia i Tomek on 30.12.2023.
//

#include "map.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function to create a new map
struct Map* createMap() {
    struct Map* newMap = (struct Map*)malloc(sizeof(struct Map));
    if (newMap != NULL) {
        newMap->size = 0;
    }
    return newMap;
}

// Function to destroy a map and free its memory
void destroyMap(struct Map* map) {
    free(map);
}

// Function to add a key-value pair to the map
void addToMap(struct Map* map, const char* key, int value) {
    if (map->size < MAX_ENTRIES) {
        strcpy(map->entries[map->size].key, key);
        map->entries[map->size].value = value;
        map->size++;
    } else {
        printf("Map is full, cannot add more entries.\n");
    }
}

void addToMapWithType(struct Map* map, const char* key, int value, char type) {
    if (map->size < MAX_ENTRIES) {
        strcpy(map->entries[map->size].key, key);
        map->entries[map->size].value = value;
        map->entries[map->size].type = type;
        map->size++;
    } else {
        printf("Map is full, cannot add more entries.\n");
    }
}

// Function to get the value associated with a key in the map
int getFromMap(const struct Map* map, const char* key) {
    for (int i = 0; i < map->size; ++i) {
        if (strcmp(map->entries[i].key, key) == 0) {
            return map->entries[i].value;
        }
    }
    // Return a default value (you can modify this behavior based on your requirements)
    return -1;
}

char getTypeFromMap(const struct Map* map, const char* key) {
    for (int i = 0; i < map->size; ++i) {
        if (strcmp(map->entries[i].key, key) == 0) {
            return map->entries[i].type;
        }
    }
    // Return a default value (you can modify this behavior based on your requirements)
    return '?';
}

int getKeyIndex(const struct Map* map, const char* key) {
    for (int i = 0; i < map->size; ++i) {
        if (strcmp(map->entries[i].key, key) == 0) {
            return i;
        }
    }
    // Return a default value (you can modify this behavior based on your requirements)
    return -1;
}

int hasKey(const struct Map* map, const char* key) {
    for (int i = 0; i < map->size; ++i) {
        if (strcmp(map->entries[i].key, key) == 0) {
            return 1;
        }
    }
    return 0;
}

int hasValue(const struct Map* map, int value) {
    for (int i = 0; i < map->size; ++i) {
        if (map->entries[i].value == value) {
            return 1;
        }
    }
    return 0;
}

int hasPair(const struct Map* map, const char* key, int value) {
    for (int i = 0; i < map->size; ++i) {
        if (strcmp(map->entries[i].key, key) == 0 && map->entries[i].value == value) {
            return 1;
        }
    }
    return 0;
}

// Function to print the map
void printMap(const struct Map* map) {
    for (int i = 0; i < map->size; ++i) {
        printf("Key: %s, Value: %d, Type: %c\n", map->entries[i].key, map->entries[i].value, map->entries[i].type);
    }
    if (map->size == 0) {
        printf("empty map :C");
    }
}
        