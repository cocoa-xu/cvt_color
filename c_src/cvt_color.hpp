#ifndef __CVT_COLOR_HPP__
#define __CVT_COLOR_HPP__

#pragma once
#include <omp.h>
#include <stdint.h>
#include <functional>

typedef enum {
    RGB888,     // 24bit
    RGB565,     // 16bit
    BGR888,     // 24bit
    BGR565      // 16bit
} color;

static int cvt_color(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
    const std::function<void*(size_t)>& alloc_func
);

static int cvt_color_888_to_565(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
    const std::function<void*(size_t)>& alloc_func
);

int cvt_color(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
    const std::function<void*(size_t)>& alloc_func)
{
    if (out_data == nullptr || out_size == nullptr) return 1;
    if (src == dst) {
        *out_data = (uint8_t *)alloc_func(size);
        if (*out_data == nullptr) return 1;
        memcpy(*out_data, data, size);
        *out_size = size;
        return 0;
    }

    if ((src == RGB888 || src == BGR888) && (dst == RGB565 || dst == BGR565)) {
        return cvt_color_888_to_565(data, size, src, dst, out_data, out_size, alloc_func);
    }

    return 1;
}

int cvt_color_888_to_565(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
    const std::function<void*(size_t)>& alloc_func)
{
    *out_size = size / 24 * 16;
    uint16_t * allocated_data = (uint16_t *)alloc_func(*out_size);
    if (allocated_data == nullptr) return 1;

    *out_data = (uint8_t *)allocated_data;
    size_t num_pixels = size / 3;
    size_t r_offset = 0, b_offset = 2;
    if (src == BGR888) {
        r_offset = 2;
        b_offset = 0;
    }
    bool target_bgr = (dst == BGR565);
    int n_jobs = omp_get_num_procs();
    int chunk_size = num_pixels / n_jobs;
    if (target_bgr) {
#pragma omp parallel for schedule(static, chunk_size)
        for (size_t i = 0; i < num_pixels; ++i) {
            size_t index = i * 3;
            uint16_t r = data[index + r_offset];
            uint16_t g = data[index + 1];
            uint16_t b = data[index + b_offset];
            uint16_t format565 = 0;
            format565 = ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | ((r & 0xF8) >> 3);
            allocated_data[i] = (format565>>8) | (format565<<8);
        }
    } else {
#pragma omp parallel for schedule(static, chunk_size)
        for (size_t i = 0; i < num_pixels; ++i) {
            size_t index = i * 3;
            uint16_t r = data[index + r_offset];
            uint16_t g = data[index + 1];
            uint16_t b = data[index + b_offset];
            uint16_t format565 = 0;
            format565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
            allocated_data[i] = (format565>>8) | (format565<<8);
        }
    }

    return 0;
}

#endif  // __CVT_COLOR_HPP__
