# CvtColor

Functions for converting image color.

| OS               | Build Status |
|------------------|--------------|
| Ubuntu 20.04     | [![CI](https://github.com/cocoa-xu/cvt_color/actions/workflows/linux-build.yml/badge.svg)](https://github.com/cocoa-xu/cvt_color/actions/workflows/linux-build.yml) |
| macOS 11         | [![CI](https://github.com/cocoa-xu/cvt_color/actions/workflows/macos-build.yml/badge.svg)](https://github.com/cocoa-xu/cvt_color/actions/workflows/macos-build.yml) |

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `cvt_color` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:cvt_color, "~> 0.1.3"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/cvt_color](https://hexdocs.pm/cvt_color).

## Usage

```elixir
bgr565_data = CvtColor.cvt(binary_data, :bgr888, :bgr565)
```

If you have OpenMP enabled, see more details about this in the Optional Config section below.
```elixir
chunk_size = 65536
bgr565_data = CvtColor.cvt(binary_data, :bgr888, :bgr565, chunk_size)

# to balance the task
# set chunk_size to 0 or do not pass anything
bgr565_data = CvtColor.cvt(binary_data, :bgr888, :bgr565, 0)
bgr565_data = CvtColor.cvt(binary_data, :bgr888, :bgr565)
```

Currently supported pairs:

| src color | dst color |
|-----------|-----------|
| `:bgr888` | `:bgr565` |
| `:bgr888` | `:rgb565` |
| `:rgb888` | `:bgr565` |
| `:rgb888` | `:rgb565` |
| `:bgr888` | `:bgr666` |
| `:bgr888` | `:rgb666` |
| `:rgb888` | `:bgr666` |
| `:rgb888` | `:rgb666` |
| `:bgr888` | `:bgr666_compact` |
| `:bgr888` | `:rgb666_compact` |
| `:rgb888` | `:bgr666_compact` |
| `:rgb888` | `:rgb666_compact` |

Each component in `bgr666` and `rgb666` takes 8bit space, but only bits in MSB(7-2) are valid. 

```
 MSB 7                                       LSB
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│  X  │  X  │  X  │  X  │  X  │  X  │  -  │  -  │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```

`X` indicates valid bit. `-` indicates ignored bit.

An example of `:rgb666_compact` is shown below. Each rectangle indicates 1 bit.

```
┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
│R5│R4│R3│R2│R1│R0│G5│G4│G3│G2│G1│G0│B5│B4│B3│B2│B1│B0│
└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘
```

## Optional Config
### OpenMP
This project can set to use OpenMP to accelerate the converting process. By default, this project will try to find and 
use OpenMP. However, you can completely disable OpenMP by setting the environment variable `CVT_COLOR_USE_OPENMP` to `OFF`.

### Chunk size (when OpenMP is enabled)
When assigning computing task to each thread, `cvt_color` will try to balance the number of consecutive pixels for each thread.
For example, say you have `4` (logical) cores (and didn't set an explicit value for `OMP_NUM_THREADS`, i.e., `OMP_NUM_THREADS` 
will be equal to the number of logical cores), and `4_000_000` pixels, then core 0 will process pixels from index `0` to `999_999`, 
pixels from `1_000_000` to `1_999_999` will be assigned to core 1, and so on. The chunk size is calculated in the way of the
following pseudocode:

```
if not specified chunk_size
    chunk_size = number_of_pixels / ${OMP_NUM_THREADS};
    if chunk_size is 0
        # the result of the integer division is 0
        # which means that number_of_pixels < ${OMP_NUM_THREADS}
        # then a single thread is good enough
        # unless ${OMP_NUM_THREADS} is really a large value
        # e.g., ${OMP_NUM_THREADS} > 1_000_000 or even larger
        chunk_size = number_of_pixels
    endif
endif
```

However, when you have a relatively fewer number of pixels, say `255`, then the time of starting ~`${OMP_NUM_THREADS}` new
threads and waiting for them to finish can take a significant portion of the converting process. Therefore, we should set 
a minimal chunk size, i.e., the forth argument of `CvtColor.cvt_color/4`.

The appropriate minimal chunk size can vary a lot depending on the processor (cache size, speed, etc), the threading library 
(how long it takes to init a new thread).

You can test and find a good value on your device using the benchmark program. To compile the benchmark program, set 
the environment variable `CVT_COLOR_BUILD_BENCHMARK` to `ON`.

Or you can pass different values to `CvtColor.cvt_color/4` and pick the one that satisfies your requirement.

The default chunk size is `1048576` (pixels) based on some benchmarks (see below). And presumably, as long as it is a power
of 2 and range from `65536` to `1048576`, it should be good. But if you are in doubt, you can always run the benchmark
on your device to find a good value.

## Benchmarks
Image size 7680x4320, pixel format RGB888. The time of generating the source image is not counted, and the source image
will only be generated once per benchmark (i.e, per entry in the table below). The converting task will be repeated for
`100` times, and the final results denote the average running time per converting.

Note that RGB888 to RGB666 is basically `memcpy`. `RGB666C.` in the table below means convert to `:rgb666_compact`. 

`chunk_size` is the number of pixels (instead of bytes) assigned to a thread.

Time unit is milliseconds.

| Platform           | #threads | chunk_size | RGB666C. | RGB666 | RGB565 |
|--------------------|---------:|-----------:|---------:|-------:|-------:|
| GitHub CI Linux    |        2 |      256   |    36.62 |  40.03 |  40.73 |
| GitHub CI Linux    |        2 |        1 K |    35.79 |  40.46 |  39.46 |
| GitHub CI Linux    |        2 |       16 K |    35.43 |  40.29 |  37.97 |
| GitHub CI Linux    |        2 |       64 K |    35.61 |  40.82 |  38.25 |
| GitHub CI Linux    |        2 |      256 K |    33.13 |  40.35 |  37.80 |
| GitHub CI Linux    |        2 |        1 M |    32.55 |  40.23 |  35.20 |
| GitHub CI Linux    |        2 |        2 M |    31.81 |  41.62 |  35.29 |
| GitHub CI Linux    |        2 |        4 M |    31.15 |  40.38 |  35.37 |
| GitHub CI Linux    |        2 |       auto |    62.45 |  41.09 |  35.12 |
| GitHub CI Linux    |        1 |         NA |    61.52 |  39.87 |  67.66 |
| GitHub CI macOS    |        3 |      256   |    36.58 |  21.32 |  27.68 |
| GitHub CI macOS    |        3 |        1 K |    34.17 |  21.55 |  23.23 |
| GitHub CI macOS    |        3 |       16 K |    33.63 |  21.58 |  22.11 |
| GitHub CI macOS    |        3 |       64 K |    34.27 |  21.43 |  22.91 |
| GitHub CI macOS    |        3 |      256 K |    35.41 |  21.73 |  21.46 |
| GitHub CI macOS    |        3 |        1 M |    37.89 |  21.61 |  23.10 |
| GitHub CI macOS    |        3 |        2 M |    47.56 |  21.38 |  23.03 |
| GitHub CI macOS    |        3 |        4 M |    48.49 |  21.78 |  24.08 |
| GitHub CI macOS    |        3 |       auto |    90.44 |  21.56 |  29.51 |
| GitHub CI macOS    |        1 |         NA |    90.81 |  21.73 |  54.03 |
| Raspberry Pi 4     |        4 |      256   |   114.45 | 148.92 | 120.07 |
| Raspberry Pi 4     |        4 |        1 K |    94.61 | 149.58 |  98.46 |
| Raspberry Pi 4     |        4 |       16 K |    88.28 | 149.65 |  76.26 |
| Raspberry Pi 4     |        4 |       64 K |    87.04 | 148.34 |  74.32 |
| Raspberry Pi 4     |        4 |      256 K |    87.65 | 148.93 |  75.46 |
| Raspberry Pi 4     |        4 |        1 M |    87.11 | 148.80 |  74.65 |
| Raspberry Pi 4     |        4 |        2 M |    86.53 | 149.31 |  74.91 |
| Raspberry Pi 4     |        4 |        4 M |   120.25 | 149.34 |  74.13 |
| Raspberry Pi 4     |        4 |       auto |   211.52 | 149.06 |  75.09 |
| Raspberry Pi 4     |        1 |         NA |   210.87 | 148.98 | 140.95 |
| M1 Max             |       10 |      256   |    11.49 |   7.81 |  12.44 |
| M1 Max             |       10 |        1 K |     9.90 |   7.88 |   8.44 |
| M1 Max             |       10 |       16 K |     9.84 |   7.79 |   8.07 |
| M1 Max             |       10 |       64 K |     9.80 |   7.77 |   7.69 |
| M1 Max             |       10 |      256 K |     9.32 |   7.84 |   7.62 |
| M1 Max             |       10 |        1 M |     8.83 |   7.73 |   7.34 |
| M1 Max             |       10 |        2 M |    14.10 |   7.75 |   7.51 |
| M1 Max             |       10 |        4 M |    26.55 |   7.78 |   6.94 |
| M1 Max             |       10 |       auto |    26.38 |   7.84 |   6.99 |
| M1 Max             |        1 |         NA |    49.64 |   7.79 |  35.22 |
| AMD 3900X          |       24 |      256   |    25.46 |  24.26 |  47.52 |
| AMD 3900X          |       24 |        1 K |    10.03 |  23.52 |  12.13 |
| AMD 3900X          |       24 |       16 K |    10.03 |  23.17 |   8.25 |
| AMD 3900X          |       24 |       64 K |     9.00 |  23.17 |   8.25 |
| AMD 3900X          |       24 |      256 K |     9.70 |  24.08 |   8.12 |
| AMD 3900X          |       24 |        1 M |    11.08 |  23.34 |   8.88 |
| AMD 3900X          |       24 |        2 M |    16.94 |  23.61 |   8.42 |
| AMD 3900X          |       24 |        4 M |    28.25 |  23.60 |  10.90 |
| AMD 3900X          |       24 |       auto |    17.03 |  23.96 |   8.45 |
| AMD 3900X          |        1 |         NA |    44.03 |  23.15 |  48.57 |
