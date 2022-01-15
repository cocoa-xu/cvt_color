cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(cvt_color)

option(USE_OPENMP "Allow to use OpenMP if available" ON)
option(BUILD_BENCHMARK "Build benchmark" OFF)
if (USE_OPENMP)
    set(USE_OPENMP false)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        set(USE_OPENMP true)
    endif()
else()
    set(USE_OPENMP false)
endif()

if(USE_OPENMP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DUSE_OPENMP=1")
endif()

if(NOT DEFINED C_SRC)
    set(C_SRC "c_src")
endif()

file(GLOB cvt_color_lib_sources CONFIGURE_DEPENDS "${C_SRC}/cvt_color.cpp" "${C_SRC}/*.hpp")
file(GLOB cvt_color_benchmark_sources CONFIGURE_DEPENDS "${C_SRC}/benchmark.cpp" "${C_SRC}/cvt_color.hpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -I${ERTS_INCLUDE_DIR} -O3 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")

add_library(cvt_color SHARED ${cvt_color_lib_sources})
set_property(TARGET cvt_color PROPERTY CXX_STANDARD 14)
set_target_properties(cvt_color PROPERTIES PREFIX "")
set_target_properties(cvt_color PROPERTIES SUFFIX ".so")
set_target_properties(cvt_color PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

if(BUILD_BENCHMARK)
    add_executable(cvt_color_benchmark ${cvt_color_benchmark_sources})
    set_property(TARGET cvt_color_benchmark PROPERTY CXX_STANDARD 14)
endif()

if(USE_OPENMP)
    target_link_libraries(cvt_color PUBLIC OpenMP::OpenMP_CXX)
    if(BUILD_BENCHMARK)
        target_link_libraries(cvt_color_benchmark PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()

if(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "-flat_namespace -undefined suppress -undefined dynamic_lookup")
endif()
