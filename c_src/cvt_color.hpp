#ifndef __CVT_COLOR_HPP__
#define __CVT_COLOR_HPP__

#pragma once
#include <stdint.h>
#include <functional>
#include <cstring>

#ifdef USE_OPENMP
#include <omp.h>
#endif

typedef enum {
    RGB888,     // 24bit
    RGB565,     // 16bit
    BGR888,     // 24bit
    BGR565,     // 16bit
    BGR666,     // 18bit valid, but it takes 8bit per component
    RGB666,     // 18bit valid, but it takes 8bit per component
    // Compact, e.g. for 1 pixel in RGB666Compact
    // ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
    // │R5│R4│R3│R2│R1│R0│G5│G4│G3│G2│G1│G0│B5│B4│B3│B2│B1│B0│
    // └──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘
    BGR666Compact,     // 18bit per pixel, there maybe unused bits if `num_pixel * 18 % 8 != 0`
    RGB666Compact,     // 18bit per pixel, there maybe unused bits if `num_pixel * 18 % 8 != 0`
} color;

static int cvt_color(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
);

static int cvt_color_888_to_565(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
);

static int cvt_color_888_to_666(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
);

static int cvt_color_888_to_666compact(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
);

int cvt_color(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
)
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
#ifdef USE_OPENMP
        return cvt_color_888_to_565(data, size, src, dst, out_data, out_size, chunk_size, alloc_func);
#else
        return cvt_color_888_to_565(data, size, src, dst, out_data, out_size, alloc_func);
#endif
    }

    if ((src == RGB888 || src == BGR888) && (dst == RGB666 || dst == BGR666)) {
#ifdef USE_OPENMP
        return cvt_color_888_to_666(data, size, src, dst, out_data, out_size, chunk_size, alloc_func);
#else
        return cvt_color_888_to_666(data, size, src, dst, out_data, out_size, alloc_func);
#endif
    }

    if ((src == RGB888 || src == BGR888) && (dst == RGB666Compact || dst == BGR666Compact)) {
#ifdef USE_OPENMP
        return cvt_color_888_to_666compact(data, size, src, dst, out_data, out_size, chunk_size, alloc_func);
#else
        return cvt_color_888_to_666compact(data, size, src, dst, out_data, out_size, alloc_func);
#endif
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
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
)
{
    *out_size = size / 3 * 2;
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
#ifdef USE_OPENMP
    int n_jobs = omp_get_max_threads();
    if (chunk_size == 0) {
        chunk_size = num_pixels / n_jobs;
        if (chunk_size == 0) chunk_size = num_pixels;
    }
#endif

    if (!target_bgr) {
        size_t t = r_offset;
        r_offset = b_offset;
        b_offset = t;
    }

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

    return 0;
}

int cvt_color_888_to_666(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
)
{
    *out_size = size;
    uint8_t * allocated_data = (uint8_t *)alloc_func(*out_size);
    if (allocated_data == nullptr) return 1;

    *out_data = (uint8_t *)allocated_data;
    if (src == BGR888 && dst == BGR666) {
        memcpy(*out_data, data, size);
        return 0;
    }
    if (src == RGB888 && dst == RGB666) {
        memcpy(*out_data, data, size);
        return 0;
    }

    size_t num_pixels = size / 3;
    size_t r_offset = 0, b_offset = 2;
    if (src == BGR888) {
        r_offset = 2;
        b_offset = 0;
    }
    bool target_bgr = (dst == BGR666);
#ifdef USE_OPENMP
    int n_jobs = omp_get_max_threads();
    if (chunk_size == 0) {
        chunk_size = num_pixels / n_jobs;
        if (chunk_size == 0) chunk_size = num_pixels;
    }
#endif

    if (!target_bgr) {
        size_t t = r_offset;
        r_offset = b_offset;
        b_offset = t;
    }

#pragma omp parallel for schedule(static, chunk_size)
    for (size_t i = 0; i < num_pixels; ++i) {
        size_t index = i * 3;
        allocated_data[index] = data[index + b_offset];
        allocated_data[index + 1] = data[index + 1];
        allocated_data[index + 2] = data[index + r_offset];
    }

    return 0;
}

int cvt_color_888_to_666compact(
    const uint8_t * data,
    size_t size,
    color src,
    color dst,
    uint8_t ** out_data,
    size_t * out_size,
#ifdef USE_OPENMP
    size_t chunk_size,
#endif
    const std::function<void*(size_t)>& alloc_func
)
{
    size_t bits = size / 3 * 18;
    size_t bytes = (bits >> 3) + ((bits & 0x07) != 0);

    uint8_t * allocated_data = (uint8_t *)alloc_func(bytes);
    if (allocated_data == nullptr) return 1;
    *out_size = bytes;

    size_t num_pixels = size / 3;
    size_t r = 0, g = 1, b = 2;
    if (src == BGR888) {
        r = 2;
        b = 0;
    }
    bool target_bgr = (dst == BGR666Compact);

    // this struct can hold 4 compact BGR666/RGB666 pixels
    struct compact666 {
        uint8_t data[9]; // 72-bit, 9 bytes
    } __attribute__((__packed__, aligned(1)));

    size_t num_packed = num_pixels / 4;
    struct compact666 * packed_access = (struct compact666 *)allocated_data;

#ifdef USE_OPENMP
    int n_jobs = omp_get_max_threads();
    if (chunk_size == 0) {
        chunk_size = num_pixels / n_jobs;
        if (chunk_size == 0) chunk_size = num_pixels;
    }
#endif

    if (!target_bgr) {
        size_t t = r;
        r = b;
        b = t;
    }

#define _CVT_PIXEL(P) (3 * P)
#pragma omp parallel for schedule(static, chunk_size)
    for (size_t i = 0; i < num_packed; ++i) {
        struct compact666 &packed = packed_access[i];
        size_t index = i * 4 * 3; // each pack has 4 pixel, i.e., 12 components
        packed.data[0] = ( data[index + _CVT_PIXEL(0) + b] & 0xFC)       | ((data[index + _CVT_PIXEL(0) + g] & 0xC0) >> 6);   // 6bit | 2bit (B1  | G1!)
        packed.data[1] = ((data[index + _CVT_PIXEL(0) + g] & 0x3C) << 2) | ((data[index + _CVT_PIXEL(0) + r] & 0xF0) >> 4);   // 4bit | 4bit (G1! | R1!)
        packed.data[2] = ((data[index + _CVT_PIXEL(0) + r] & 0x0C) << 4) | ((data[index + _CVT_PIXEL(1) + b] & 0xFC) >> 2);   // 2bit | 6bit (R1! | B2 )
        packed.data[3] = ( data[index + _CVT_PIXEL(1) + g] & 0xFC)       | ((data[index + _CVT_PIXEL(1) + r] & 0xC0) >> 6);   // 6bit | 2bit (G2  | R2!)
        packed.data[4] = ((data[index + _CVT_PIXEL(1) + r] & 0x3C) << 2) | ((data[index + _CVT_PIXEL(2) + b] & 0xF0) >> 4);   // 4bit | 4bit (R2! | B3!)
        packed.data[5] = ((data[index + _CVT_PIXEL(2) + b] & 0x0C) << 4) | ((data[index + _CVT_PIXEL(2) + g] & 0xFC) >> 2);   // 2bit | 6bit (B3! | G3 )
        packed.data[6] = ( data[index + _CVT_PIXEL(2) + r] & 0xFC)       | ((data[index + _CVT_PIXEL(3) + b] & 0xC0) >> 6);   // 6bit | 2bit (R3  | B4!)
        packed.data[7] = ((data[index + _CVT_PIXEL(3) + b] & 0x3C) << 2) | ((data[index + _CVT_PIXEL(3) + g] & 0xF0) >> 4);   // 4bit | 4bit (B4! | G4!)
        packed.data[8] = ((data[index + _CVT_PIXEL(3) + 1] & 0x0C) << 4) | ((data[index + _CVT_PIXEL(3) + r] & 0xFC) >> 2);   // 2bit | 6bit (G4! | R4 )
    }

    size_t num_not_packed = num_pixels & 0b11;
    if (num_not_packed > 0) {
        struct compact666 * packed = (struct compact666 *)(((uint64_t)(uint64_t*)packed_access) + num_packed * sizeof(struct compact666));
        size_t index = num_packed * sizeof(struct compact666);

        packed->data[0] = ( data[index + _CVT_PIXEL(0) + b] & 0xFC)       | ((data[index + _CVT_PIXEL(0) + g] & 0xC0) >> 6);         // 6bit | 2bit (B1  | G1!)
        packed->data[1] = ((data[index + _CVT_PIXEL(0) + g] & 0x3C) << 2) | ((data[index + _CVT_PIXEL(0) + r] & 0xF0) >> 4);         // 4bit | 4bit (G1! | R1!)
        packed->data[2] = ((data[index + _CVT_PIXEL(0) + r] & 0x0C) << 4);                                                           // 2bit | 000000 (R1! | 000000 )

        if (num_not_packed > 1) {
            packed->data[2] |= ((data[index + _CVT_PIXEL(1) + b] & 0xFC) >> 2);                                                      // 2bit | 6bit (R1! | B2 )
            packed->data[3] = ( data[index + _CVT_PIXEL(1) + g] & 0xFC)       | ((data[index + _CVT_PIXEL(1) + r] & 0xC0) >> 6);     // 6bit | 2bit (G2  | R2!)
            packed->data[4] = ((data[index + _CVT_PIXEL(1) + r] & 0x3C) << 2);                                                       // 4bit | 0000 (R2! | 0000)

            if (num_not_packed > 2) {
                packed->data[4] |= ((data[index + _CVT_PIXEL(2) + b] & 0xF0) >> 4);                                                // 4bit | 0000 (R2! | B3!)
                packed->data[5] = ((data[index + _CVT_PIXEL(2) + b] & 0x0C) << 4) | ((data[index + _CVT_PIXEL(2) + g] & 0xFC) >> 2); // 2bit | 6bit (B3! | G3 )
                packed->data[6] = ( data[index + _CVT_PIXEL(2) + r] & 0xFC);                                                         // 6bit | 00 (R3  | 00)
            }
        }
    }
#undef _CVT_PIXEL

    *out_data = allocated_data;

    return 0;
}

#endif  // __CVT_COLOR_HPP__
