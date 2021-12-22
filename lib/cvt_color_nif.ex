defmodule CvtColor.Nif do
  @moduledoc false

  @on_load :load_nif
  def load_nif do
    nif_file = '#{:code.priv_dir(:cvt_color)}/cvt_color'

    case :erlang.load_nif(nif_file, 0) do
      :ok -> :ok
      {:error, {:reload, _}} -> :ok
      {:error, reason} -> IO.puts("Failed to load nif: #{inspect(reason)}")
    end
  end

  def cvt_color(_image_data, _source_color, _target_color), do: :erlang.nif_error(:not_loaded)
end
