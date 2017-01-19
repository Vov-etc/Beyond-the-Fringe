#define _CRT_SECURE_NO_WARNINGS

#include "include/file.h"

#include <stdio.h>
#include <fstream>

char *read_file(const char * filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }
    if (fseek(file, 0, SEEK_END) == -1) {
        return NULL;
    }
    long int size = ftell(file);
    if (size == -1) {
        return NULL;
    }
    if (fseek(file, 0, SEEK_SET) == -1) {
        return NULL;
    }
    char *data = new char[size + 1];
    if (data == NULL) {
        return NULL;
    }
    if (fread(data, 1, size, file) != (size_t) size || ferror(file)) {
        free(data);
        return NULL;
    }
    fclose(file);
    data[size] = '\0';
    return data;
}
