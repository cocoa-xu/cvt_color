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

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `cvt_color` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:cvt_color, "~> 0.1.2"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/cvt_color](https://hexdocs.pm/cvt_color).

