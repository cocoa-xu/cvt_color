defmodule CvtColor.MixProject do
  use Mix.Project

  def project do
    [
      app: :cvt_color,
      version: "0.1.0",
      elixir: "~> 1.12",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      description: description(),
      package: package(),
      deps: deps(),
      source_url: "https://github.com/cocoa-xu/cvt_color"
    ]
  end

  def application do
    []
  end

  defp description() do
    "Functions for converting image color"
  end

  defp deps do
    [
      {:elixir_make, "~> 0.6"},
      {:ex_doc, "~> 0.14", only: :dev, runtime: false}
    ]
  end

  defp package() do
    [
      name: "cvt_color",
      files: ~w(c_src lib .formatter.exs mix.exs README* LICENSE*),
      licenses: ["Apache-2.0"],
      links: %{"GitHub" => "https://github.com/cocoa-xu/cvt_color"}
    ]
  end
end
