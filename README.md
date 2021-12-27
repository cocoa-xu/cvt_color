# CvtColor

Functions for converting image color.

| OS               | Build Status |
|------------------|--------------|
| Ubuntu 20.04     | [![CI](https://github.com/cocoa-xu/cvt_color/actions/workflows/linux-build.yml/badge.svg)](https://github.com/cocoa-xu/cvt_color/actions/workflows/linux-build.yml) |
| macOS 11         | [![CI](https://github.com/cocoa-xu/cvt_color/actions/workflows/macos-build.yml/badge.svg)](https://github.com/cocoa-xu/cvt_color/actions/workflows/macos-build.yml) |

## Usage

```elixir
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

Each component in `bgr666` and `rgb666` takes 8bit space, but only bits in MSB(7-2) are valid. 

```
 MSB 7                                       LSB
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│  X  │  X  │  X  │  X  │  X  │  X  │  -  │  -  │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```

`X` indicates valid bit. `-` indicates ignored bit. 

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `cvt_color` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:cvt_color, "~> 0.1.1"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/cvt_color](https://hexdocs.pm/cvt_color).

