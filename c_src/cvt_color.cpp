#include "cvt_color.hpp"
#include <memory>
#include <vector>
#include <string>
#include "nif_utils.hpp"
#include <erl_nif.h>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#  pragma GCC diagnostic ignored "-Wunused-variable"
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif

static ERL_NIF_TERM cvt_color_nif(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    if (argc != 3) {
        return erlang::nif::error(env, "expecting 3 arguments: image_data, channel_order_source, channel_order_target");
    }

    std::string source_color, target_color;
    erlang::nif::get_atom(env, argv[1], source_color);
    erlang::nif::get_atom(env, argv[2], target_color);
    ErlNifBinary binary;
    if (enif_inspect_binary(env, argv[0], &binary)) {
        if (source_color == "rgb888" || source_color == "bgr888") {
            if (binary.size % 24 == 0) {
                ErlNifBinary result;
                color src = color::RGB888, dst = color::RGB565;
                if (source_color == "bgr888") src = color::BGR888;
                if (target_color == "bgr565") src = color::BGR565;
                uint8_t * out_data = nullptr;
                size_t out_size = 0;
                int ret = cvt_color(binary.data, binary.size, src, dst, &out_data, &out_size, [&result](size_t bytes) {
                    enif_alloc_binary(bytes, &result);
                    return result.data;
                });
                if (ret != 0) {
                    return erlang::nif::error(env, "cvt failed");
                } else {
                    return enif_make_binary(env, &result);
                }
            }
            return erlang::nif::error(env, "malformed BGR888/RGB888 binary data");
        } else {
            return erlang::nif::error(env, "not implemented yet");
        }
    } else {
        return erlang::nif::error(env, "expecting bitstring");
    }
}

static int on_load(ErlNifEnv* env, void**, ERL_NIF_TERM)
{
    return 0;
}

static int on_reload(ErlNifEnv*, void**, ERL_NIF_TERM)
{
    return 0;
}

static int on_upgrade(ErlNifEnv*, void**, void**, ERL_NIF_TERM)
{
    return 0;
}

#define F(NAME, ARITY) {#NAME, ARITY, NAME, 0}

static ErlNifFunc nif_functions[] = {
    {"cvt_color", 3, cvt_color_nif, 0}
};

ERL_NIF_INIT(Elixir.CvtColor.Nif, nif_functions, on_load, on_reload, on_upgrade, NULL);

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif
