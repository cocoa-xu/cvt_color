#include "cvt_color.hpp"
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <limits.h>
#include <vector>
#include <string>
#ifdef USE_OPENMP
#include <omp.h>
#endif

uint8_t * generate_888(size_t width, size_t height, size_t * size) {
    size_t bytes = sizeof(uint8_t) * width * height * 3;
    uint8_t * data = (uint8_t *)malloc(bytes);
    // printf("number of bytes allocated: %" PRIu64 ", %p\n", bytes, data);
    // fflush(stdout);

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

void get_number(const char * str, size_t& number) {
    char *eptr;
    long long result = strtoll(str, &eptr, 10);
    if (result == 0)
    {
        if (errno == EINVAL) {
            std::cout << "Conversion error occurred: " << errno << '\n';
            exit(0);
        }
    }

    if (result == LLONG_MIN || result == LLONG_MAX) {
        if (errno == ERANGE) {
            std::cout << "The value provided was out of range\n";
            exit(0);
        }
    }

    if (result <= 0) {
        std::cout << "The value should be a positive integer value\n";
        std::cout << "usage: cvt_color_benchmark width height [num_repeat] [chunk_size]\n";
        exit(0);
    }

    number = result;
}

int main(int argc, const char ** argv) {
    size_t width = 3840*2;
    size_t height = 2160*2;
    size_t num_repeat = 100;

    if (argc >= 2) get_number(argv[1], width);
    if (argc >= 3) get_number(argv[2], height);
    if (argc >= 4) get_number(argv[3], num_repeat);

#ifdef USE_OPENMP
    size_t chunk_size = 65535;
    if (argc == 5) {
        get_number(argv[4], chunk_size);
    }
#endif

    {
#ifdef USE_OPENMP
        std::cout << "USE_OPENMP=ON\n";
        std::cout << "omp_get_max_threads: " << omp_get_max_threads() << '\n';
        std::cout << "chunk_size: " << chunk_size << '\n';
#else
        std::cout << "USE_OPENMP=OFF\n";
#endif
        std::cout << "convert " << width << "x" << height << " for " << num_repeat << " times\n";
        size_t size = 0;
        uint8_t * data = generate_888(width, height, &size);
        if (data == nullptr) return -1;

        std::vector<color> dst_colors = {color::RGB666Compact, color::RGB666, color::RGB565};
        std::vector<std::string> dst_name = {"RGB666Compact", "RGB666", "RGB565"};
        for (int i = 0; i < (int)dst_colors.size(); i++) {
            auto dst_color = dst_colors[i];
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < num_repeat; ++i) {
                uint8_t * out_data = nullptr;
                size_t out_size = 0;
#ifdef USE_OPENMP
                int ret = cvt_color(data, size, color::RGB888, dst_color, &out_data, &out_size, chunk_size, malloc);
#else
                int ret = cvt_color(data, size, color::RGB888, dst_color, &out_data, &out_size, malloc);
#endif
                if (ret != 0) {
                    return -1;
                } else {
                    free((void *)out_data);
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto ns_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "convert from RGB888 => " << dst_name[i] << ": ";
            std::cout << ns_int.count() / 1.f / num_repeat << "ms per convert" << "\n";
        }

        free((void *)data);
    }
}
