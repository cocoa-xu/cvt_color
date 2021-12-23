#include "cvt_color.hpp"
#include <chrono>
#include <iostream>

uint8_t * generate_888(size_t width, size_t height, size_t * size) {
    uint8_t * data = (uint8_t *)malloc(sizeof(uint8_t) * width * height * 3);
    *size = sizeof(uint8_t) * width * height * 3;
    uint8_t val = 0;
    for (size_t row = 0; row < height; ++row) {
        for (size_t col = 0; col < width; ++col) {
            data[row * width + col * 3 + 0] = val++;
            data[row * width + col * 3 + 1] = val++;
            data[row * width + col * 3 + 2] = val++;
        }
    }
    return data;
}

int main() {
    const size_t num_repeat = 100;
    const size_t width = 3840*2;
    const size_t height = 2160*2;

    {
        std::cout << "convert " << width << "x" << height << " for " << num_repeat << " times\n";
        size_t size = 0;
        uint8_t * data = generate_888(width, height, &size);
        if (data == nullptr) return -1;

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < num_repeat; ++i) {
            uint8_t * out_data = nullptr;
            size_t out_size = 0;
            int ret = cvt_color(data, size, color::RGB888, color::RGB565, &out_data, &out_size, malloc);
            if (ret != 0) {
                return -1;
            } else {
                free((void *)out_data);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ns_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "convert from RGB888 => RGB565: ";
        std::cout << ns_int.count() / 1.f / num_repeat << "ms per convert" << "\n";

        free((void *)data);
    }
}
