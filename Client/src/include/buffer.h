#ifndef BUFFER_H
#define BUFFER_H
#include <vector>

extern const int BUFF_SIZE;

class Buffer {
    char* buffer;
    char* curr_pointer;

public:
    Buffer();

    ~Buffer();

    void reset();

    char* get_data();

    char& operator[](int i);

    const char& operator[](int i) const;

    int length() const;

    template <typename T>
    void read(const T& from);

    template <typename T>
    void write(T& to);

    template <typename T>
    void write_vector(std::vector<T>& to);

    template <typename T>
    void read_vector(const std::vector<T>& from);
};

template <typename T>
void Buffer::read(const T& from) {
    memcpy(curr_pointer, &from, sizeof(T));
    curr_pointer += sizeof(T);
}

template <typename T>
void Buffer::write(T& to) {
    memcpy(&to, curr_pointer, sizeof(T));
    curr_pointer += sizeof(T);
}

template <typename T>
void Buffer::write_vector(std::vector<T>& to) {
    int vec_size = -1;
    write(vec_size);
    to.resize(vec_size);
    for (auto& elem : to) {
        write(elem);
    }
}

template <typename T>
void Buffer::read_vector(const std::vector<T>& from) {
    read(from.size());
    for (const auto& elem : from) {
        read(elem);
    }
}
#endif //BUFFER_H
