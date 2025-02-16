//
// Created by Asia i Tomek on 30.12.2023.
//

#ifndef KOMPILATOR1_MAP_H
#define KOMPILATOR1_MAP_H

#define MAX_ENTRIES 1000
#define KEY_LENGTH 100

// Structure to represent key-value pairs
struct KeyValuePair {
    char key[KEY_LENGTH];
    int value;
    char type;
};

// Structure to represent a map
struct Map {
    struct KeyValuePair entries[MAX_ENTRIES];
    int size;
};

// Function prototypes
struct Map* createMap();
void destroyMap(struct Map* map);
void addToMap(struct Map *map, const char *key, int value);
void addToMapWithType(struct Map *map, const char *key, int value,char type);
int getFromMap(const struct Map *map, const char *key);
char getTypeFromMap(const struct Map *map, const char *key);
int getKeyIndex(const struct Map *map, const char *key);
int hasValue(const struct Map* map, int value);
int hasKey(const struct Map* map, const char *key);
int hasPair(const struct Map* map, const char *key, int value);
void printMap(const struct Map *map);

#endif  // KOMPILATOR1_MAP_H