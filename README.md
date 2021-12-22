# CvtColor

Functions for converting image color.

Currently supported pairs:

| src color | dst color |
|-----------|-----------|
| `:bgr888` | `:bgr565` |
| `:bgr888` | `:rgb565` |
| `:rgb888` | `:bgr565` |
| `:rgb888` | `:rgb565` |

## Usage

```elixir
bgr565_data = CvtColor.cvt(binary_data, :bgr888, :bgr565)
```

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `cvt_color` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:cvt_color, "~> 0.1.0"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/cvt_color](https://hexdocs.pm/cvt_color).

