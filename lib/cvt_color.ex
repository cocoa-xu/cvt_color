defmodule CvtColor do
  @moduledoc """
  Color convert function
  """

  @doc """
  Convert color

  - **image_data**: source image data. binary.
  - **src_color**: source image color.
    - :bgr888
    - :rgb888
  - **dst_color**: destination image color
    - :bgr565
    - :rgb565
  """
  def cvt(image_data, src_color, dst_color) do
    CvtColor.Nif.cvt_color(image_data, src_color, dst_color)
  end
end
