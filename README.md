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

### Minimal chunk size (when OpenMP is enabled)
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
