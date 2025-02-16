//
// Created by Asia i Tomek on 31.12.2023.
//
#include "strings.h"
char* concat(const char* str1, const char* str2) {
    char* result = (char*)malloc(strlen(str1) + strlen(str2) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(2);
    }
    strcpy(result, str1);
    strcat(result, str2);

    return result;
}

const char* append_int_to_string(const char *originalString, int numberToAppend) {
    char numberAsString[20];
    sprintf(numberAsString, "%d", numberToAppend);
    size_t newSize = strlen(originalString) + strlen(numberAsString) + 1;
    char *result = (char*)malloc(newSize);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result, originalString);
    strcat(result, numberAsString);

    return result;
}

const char* append_long_to_string(const char *originalString, unsigned long long numberToAppend) {
    char numberAsString[100];
    sprintf(numberAsString, "%llu", numberToAppend);
    size_t newSize = strlen(originalString) + strlen(numberAsString) + 1;
    char *result = (char*)malloc(newSize);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result, originalString);
    strcat(result, numberAsString);

    return result;
}

const char* right_padding(const char* text, int length) {
    // Calculate the length of the input text
    int text_length = strlen(text);

    // Check if padding is needed
    if (text_length >= length) {
        // No padding needed, return a copy of the input text
        char* result = (char*)malloc((text_length + 1) * sizeof(char));
        strcpy(result, text);
        return result;
    }

    // Calculate the number of spaces needed for padding
    //int spaces_needed = length - text_length;

    // Create a new string with the required length
    char* result = (char*)malloc((length + 1) * sizeof(char));

    // Copy the input text to the beginning of the result string
    strcpy(result, text);

    // Pad the remaining space with spaces
    for (int i = text_length; i < length; ++i) {
        result[i] = ' ';
    }

    // Null-terminate the result string
    result[length] = '\0';

    return result;
}

int prepend_to_file(FILE *file,const char *prependString) {
    fseek(file, 0, SEEK_SET);
    size_t newContentLength = strlen(prependString);
    fwrite(prependString, 1, newContentLength, file);
    fseek(file, 0, SEEK_END);
    return 0;
}

int overwrite_file(FILE *file,const char *newString, long position) {
    fseek(file, position, SEEK_SET);
    size_t newContentLength = strlen(newString);
    fwrite(newString, 1, newContentLength, file);
    fseek(file, 0, SEEK_END);
    return 0;
}