cmake_minimum_required(VERSION 3.0.0)
project(RankSelect VERSION 0.1.0)

add_compile_options(-pedantic)
add_compile_options(-Wextra)
add_compile_options(-march=native)


add_link_options(-flto)

include(CTest)
enable_testing()

add_executable(RankSelect main.c bitvector.h bitvector.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
