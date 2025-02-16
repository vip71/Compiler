//
// Created by Asia i Tomek on 03.01.2024.
//

#include "numbers.h"

int getNthBinaryDigit(unsigned long long num, int n) {
    if (n < 0) {
        return -1; // Invalid index
    }
    return (num >> n) & 1;
}

int findLeftmostSetBit(unsigned long long num) {
    if (num == 0) {
        return -1; // No '1' bit found
    }

    int position = 0;

    // Find the position of the leftmost '1' bit using bitwise operations
    while (num > 0) {
        num >>= 1;
        position++;
    }

    return position - 1; // Adjust for zero-based indexing
}

