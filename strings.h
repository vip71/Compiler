//
// Created by Asia i Tomek on 31.12.2023.
//

#ifndef KOMPILATOR1_STRINGS_H
#define KOMPILATOR1_STRINGS_H

#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* concat(const char* str1, const char* str2);
const char* append_int_to_string(const char* str1, int i);
const char* append_long_to_string(const char* str1, unsigned long long i);
const char* right_padding(const char* text, int length);
int prepend_to_file(FILE *file, const char *newString);
int overwrite_file(FILE *file, const char *newString, long position);

#endif //KOMPILATOR1_STRINGS_H
