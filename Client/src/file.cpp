#define _CRT_SECURE_NO_WARNINGS

#include "include/file.h"
#include <stdio.h>
#include <fstream>
#include <string>

std::string read_file(const char * filename) {
    std::ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        char error_message[200];
        sprintf(error_message, "cannot open file %s", filename);
        throw std::runtime_error(error_message);
    }
    auto buffer = fin.rdbuf();
    int size = buffer->in_avail();
    std::string data(size + 1, 0);
    buffer->sgetn(&data[0], size);
    fin.close();
    return data;
}
