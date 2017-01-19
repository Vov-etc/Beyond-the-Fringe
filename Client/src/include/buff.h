#pragma once
#include <iostream>
#include <vector>

const static int BUFF_SIZE = 1024 * 1024;


class buff
{
    char* buffer;
    char* curr_pointer;
    bool reading;
public:
    buff() {
        curr_pointer = buffer = new char[BUFF_SIZE];
        reading = false;
    }

    ~buff() {
        delete[] buffer;
    }

    char& operator[](int i) {
        return buffer[i];
    }

    char* recv() {
        reading = false;
        return buffer;
    }

    template <typename T>
    void write(T* from) {
        if (reading) {
            curr_pointer = buffer;
        }
        memcpy(curr_pointer, from, sizeof(T));
        curr_pointer += sizeof(T) / sizeof(char);
    }

    template <typename T>
    void read(T* to) {
        if (!reading) {
            curr_pointer = buffer;
        }
        memcpy(to, curr_pointer, sizeof(T));
        curr_pointer += sizeof(T) / sizeof(char);
    }

    template <typename T>
    void write_vector(std::vector<T> &to) {
        int vec_size = -1;
        read(&vec_size);
        to.resize(vec_size);
        for (auto &elem : to) {
            read(elem);
        }
    }
};