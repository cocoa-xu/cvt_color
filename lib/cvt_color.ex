defmodule CvtColor do
  @moduledoc """
  Color convert function
  """

  @doc """
  Convert color

  - **image_data**: source image data. binary.
  - **src_color**: source image color.
    - `:bgr888`
    - `:rgb888`
  - **dst_color**: destination image color
    - `:bgr565`
    - `:rgb565`
    - `:bgr666`
    - `:rgb666`
    - `:bgr666_compact`
    - `:rgb666_compact`

  Currently supported pairs:
    - `:bgr888`, `:bgr565`
    - `:bgr888`, `:rgb565`
    - `:rgb888`, `:bgr565`
    - `:rgb888`, `:rgb565`
    - `:bgr888`, `:bgr666`
    - `:bgr888`, `:rgb666`
    - `:rgb888`, `:bgr666`
    - `:rgb888`, `:rgb666`
    - `:bgr888`, `:bgr666_compact`
    - `:bgr888`, `:rgb666_compact`
    - `:rgb888`, `:bgr666_compact`
    - `:rgb888`, `:rgb666_compact`

  - **min_chunk_size**: Optional. Only valid when OpenMP is enabled.
    Default value: 65535
  """
  @spec cvt(bitstring, Atom.t(), Atom.t(), Integer.t()) :: binary | {:error, String.t()}
  def cvt(image_data, src_color, dst_color, min_chunk_size \\ 65535)
  def cvt(image_data, src_color, dst_color, min_chunk_size) when min_chunk_size > 0 do
    CvtColor.Nif.cvt_color(image_data, src_color, dst_color, min_chunk_size)
  end
end
