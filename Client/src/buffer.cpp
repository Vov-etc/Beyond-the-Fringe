#include <vector>
#include "include/buffer.h"

const int BUFF_SIZE = 1024 * 1024;

Buffer::Buffer() {
    buffer = new char[BUFF_SIZE];
    curr_pointer = buffer;
}

Buffer::~Buffer() {
    delete[] buffer;
}

char* Buffer::get_data() {
    return buffer;
}

void Buffer::reset() {
    curr_pointer = buffer;
}

char& Buffer::operator[](int i) {
    return buffer[i];
}

const char& Buffer::operator[](int i) const {
    return buffer[i];
}

int Buffer::length() const {
    return curr_pointer - buffer;
}

