#ifndef CONTROLS_H
#define CONTROLS_H

#include <set>
#include <vector>


class Controls {
    bool mouse_state;
    int mouse_x, mouse_y;
    std::set <int> keys_id;

public:

    Controls():
    mouse_state (false),
    mouse_x(0),
    mouse_y(0)
    {};

    ~Controls() {};

    void push(int key) {
        keys_id.insert(key);
    }

    void erase(int key) {
        keys_id.erase(key);
    }

    void clear() {
        mouse_state = false;
        mouse_x = 0;
        mouse_y = 0;
        keys_id.clear();
    }

    void set_mouse_pos(int x, int y) {
        mouse_x = x;
        mouse_y = y;
    }

    void set_mouse_state(bool state) {
        mouse_state = state;
    }

    std::vector <int> to_vec() {
        std::vector <int> vec;
        vec.push_back(mouse_state);
        vec.push_back(mouse_x);
        vec.push_back(mouse_y);
        for (auto el : keys_id) {
            vec.push_back(el);
        }
        return vec;
    }
};

#endif //CONTROLS_H
