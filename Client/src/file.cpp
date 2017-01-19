#define _CRT_SECURE_NO_WARNINGS

#include "include/file.h"

//#include <stdio.h>
#include <fstream>
#include <iostream>

char *read_file(const char * filename) {
    /*FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }
    if (fseek(file, 0, SEEK_END) == -1) {
        return NULL;
    }
    long int size = ftell(file);
    std::cout << size << std::endl;
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
    return data;*/


    std::fstream fin;
    fin.open(filename, std::ios_base::in);
    if (!fin.is_open()) {
        return NULL;
    }
    fin.seekp(0, std::ios::end);
    int size = fin.tellg();
    std::cout << size << std::endl;
    fin.seekp(0, std::ios::beg);
    char *data = new char[size + 1];
    if (data == NULL) {
        return NULL;
    }
    fin.read(data, size);
    data[size] = '\0';
    fin.close();
    return data;
}
