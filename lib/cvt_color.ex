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

  - **chunk_size**: Optional. Number of pixels to handle in a thread. Only valid when OpenMP is enabled.
    Default value: 1048576.

    Set to 0 to automatically set the chunk size in a naive way.

    chunk_size = total number of pixels / max number of OMP threads
  """
  @spec cvt(bitstring, Atom.t(), Atom.t(), List.t()) :: binary | {:error, String.t()}
  def cvt(image_data, src_color, dst_color, chunk_size \\ 1048576)
  def cvt(image_data, src_color, dst_color, chunk_size) when chunk_size >= 0 do
    CvtColor.Nif.cvt_color(image_data, src_color, dst_color, chunk_size)
  end
end
