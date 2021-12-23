cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(cvt_color)

find_package(OpenMP)
file(GLOB cvt_color_lib_sources CONFIGURE_DEPENDS "${C_SRC}/cvt_color.cpp" "${C_SRC}/*.hpp")
file(GLOB cvt_color_benchmark_sources CONFIGURE_DEPENDS "${C_SRC}/benchmark.cpp" "${C_SRC}/cvt_color.hpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -I${ERTS_INCLUDE_DIR} -O3 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")

add_executable(cvt_color.so ${cvt_color_lib_sources})
set_property(TARGET cvt_color.so PROPERTY CXX_STANDARD 14)

set_target_properties(cvt_color.so PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)
set_property(TARGET cvt_color.so PROPERTY COMPILE_OPTIONS "-dynamiclib")

add_executable(cvt_color_benchmark ${cvt_color_benchmark_sources})
set_property(TARGET cvt_color_benchmark PROPERTY CXX_STANDARD 14)

if(OpenMP_CXX_FOUND)
    target_link_libraries(cvt_color.so PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(cvt_color_benchmark PUBLIC OpenMP::OpenMP_CXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DUSE_OPENMP=1")
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared")
else()
    set(CMAKE_SHARED_LINKER_FLAGS "-bundle -undefined suppress")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -undefined dynamic_lookup")
endif()
