defmodule CvtColorTest do
  use ExUnit.Case
  doctest CvtColor

  test "bgr888 => bgr565/rgb565" do
    image_data = << 0b11111_000, 0b110011_00, 0b01110_000 >>
    cvt_image_data = CvtColor.cvt(image_data, :bgr888, :bgr565)
    assert << 0b11111_110, 0b011_01110 >> = cvt_image_data

    cvt_image_data = CvtColor.cvt(image_data, :bgr888, :rgb565)
    assert << 0b01110_110, 0b011_11111 >> = cvt_image_data
  end

  test "rgb888 => bgr565/rgb565" do
    image_data = << 0b11111_000, 0b110011_00, 0b01110_000 >>
    cvt_image_data = CvtColor.cvt(image_data, :rgb888, :bgr565)
    assert << 0b01110_110, 0b011_11111 >> = cvt_image_data

    cvt_image_data = CvtColor.cvt(image_data, :rgb888, :rgb565)
    assert << 0b11111_110, 0b011_01110 >> = cvt_image_data
  end

  test "bgr888 => bgr666, bgr888 => rgb666" do
    image_data = << 0,1,2,3,4,5 >>
    cvt_image_data = CvtColor.cvt(image_data, :bgr888, :bgr666)
    assert ^image_data = cvt_image_data

    cvt_image_data = CvtColor.cvt(image_data, :bgr888, :rgb666)
    assert << 2,1,0,5,4,3 >> = cvt_image_data
  end

  test "rgb888 => rgb666, rgb888 => bgr666" do
    image_data = << 0,1,2,3,4,5 >>
    cvt_image_data = CvtColor.cvt(image_data, :rgb888, :rgb666)
    assert ^image_data = cvt_image_data

    cvt_image_data = CvtColor.cvt(image_data, :rgb888, :bgr666)
    assert << 2,1,0,5,4,3 >> = cvt_image_data
  end

  test "bgr888 => bgr666_compact, bgr888 => rgb666_compact" do
    image_data = << 0b101010_00, 0b010101_00, 0b111111_00 >>
    cvt_image_data = CvtColor.cvt(image_data, :bgr888, :bgr666_compact)
    assert << 0b101010_01, 0b0101_1111, 0b11_000000 >> = cvt_image_data

    cvt_image_data = CvtColor.cvt(image_data, :bgr888, :rgb666_compact)
    assert << 0b111111_01, 0b0101_1010, 0b10_000000 >> = cvt_image_data
  end

  test "rgb888 => bgr666_compact, rgb888 => rgb666_compact" do
    image_data = << 0b101010_00, 0b010101_00, 0b111111_00 >>
    cvt_image_data = CvtColor.cvt(image_data, :rgb888, :bgr666_compact)
    assert << 0b111111_01, 0b0101_1010, 0b10_000000 >> = cvt_image_data

    cvt_image_data = CvtColor.cvt(image_data, :rgb888, :rgb666_compact)
    assert << 0b101010_01, 0b0101_1111, 0b11_000000 >> = cvt_image_data
  end
end
