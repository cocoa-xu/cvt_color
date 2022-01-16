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
    if (argc != 4) {
        return erlang::nif::error(env, "expecting 4 arguments: image_data, source_color, target_color, chunk_size");
    }

    std::string source_color, target_color;
    erlang::nif::get_atom(env, argv[1], source_color);
    erlang::nif::get_atom(env, argv[2], target_color);
    ErlNifBinary binary;
    if (enif_inspect_binary(env, argv[0], &binary)) {
        if (source_color == "rgb888" || source_color == "bgr888") {
            if (binary.size % 3 == 0) {
                ErlNifBinary result;
                color src = color::RGB888, dst = color::RGB565;
                if (source_color == "bgr888") src = color::BGR888;
                if (target_color == "bgr565") dst = color::BGR565;

                if (target_color == "rgb666") dst = color::RGB666;
                if (target_color == "bgr666") dst = color::BGR666;
                if (target_color == "rgb666_compact") dst = color::RGB666Compact;
                if (target_color == "bgr666_compact") dst = color::BGR666Compact;

#ifdef USE_OPENMP
                ErlNifUInt64 chunk_size;
                if (!enif_get_uint64(env, argv[3], &chunk_size)) {
                    return erlang::nif::error(env, "expecting chunk_size to be unsigned 64-bit integer");
                }
#endif
                uint8_t * out_data = nullptr;
                size_t out_size = 0;
#ifdef USE_OPENMP
                int ret = cvt_color(binary.data, binary.size, src, dst, &out_data, &out_size, chunk_size, [&result](size_t bytes) {
#else
                int ret = cvt_color(binary.data, binary.size, src, dst, &out_data, &out_size, [&result](size_t bytes) {
#endif
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

static ErlNifFunc nif_functions[] = {
    {"cvt_color", 4, cvt_color_nif, ERL_NIF_DIRTY_JOB_IO_BOUND}
};

ERL_NIF_INIT(Elixir.CvtColor.Nif, nif_functions, on_load, on_reload, on_upgrade, NULL);

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif
