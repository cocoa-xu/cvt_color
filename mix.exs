defmodule CvtColor.MixProject do
  use Mix.Project

  @repo_url "https://github.com/cocoa-xu/cvt_color"
  def project do
    [
      app: :cvt_color,
      version: "0.1.3",
      elixir: "~> 1.12",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      description: description(),
      package: package(),
      deps: deps(),
      source_url: @repo_url
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
      {:elixir_make, "~> 0.6", runtime: false},
      {:ex_doc, "~> 0.14", only: :dev, runtime: false}
    ]
  end

  defp package() do
    [
      name: "cvt_color",
      files: ~w(c_src lib .formatter.exs mix.exs README* LICENSE* Makefile CMakeLists.txt),
      licenses: ["Apache-2.0"],
      links: %{"GitHub" => @repo_url}
    ]
  end
end
