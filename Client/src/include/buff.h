#ifndef BUFFER_H
#define BUFFER_H

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
        curr_pointer = buffer;
        return buffer;
    }

    template <typename T>
    void write(T* from) {
        if (reading) {
            curr_pointer = buffer;
        }
        memcpy(curr_pointer, from, sizeof(T));
        curr_pointer += sizeof(T) / sizeof(char);
        curr_pointer[0] = 0;
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

    template <typename T>
    void read_vector(std::vector<T> &from) {
        write(from.size());
        for (int i = 0; i < from.size(); i++) {
            write(from[i]);
        }
    }
};
#endif //BUFFER_H
