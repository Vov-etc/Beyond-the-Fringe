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
    fin.sync();
	std::string data;
	getline(fin, data);
    auto buffer = fin.rdbuf();
    int size = buffer->in_avail();
    if (size == 0) {
        char error_message[200];
        sprintf(error_message, "File %s is empty", filename);
        throw std::runtime_error(error_message);
    }
	int line_size = data.size();
	data.resize(size + line_size + 1);
    buffer->sgetn(&data[line_size], size);
    fin.close();
    return data;
}
