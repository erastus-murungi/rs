cmake_minimum_required(VERSION 3.0.0)
project(RankSelect VERSION 0.1.0)

add_compile_options(-pedantic)
add_compile_options(-Wextra)
add_compile_options(-march=native)


add_link_options(-flto)


include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(GLOB sources "${PROJECT_SOURCE_DIR}/*.c")

add_executable(RankSelect main.c bitvector.h bitvector.c string_utils.c)

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB sources "${PROJECT_SOURCE_DIR}/*.c")
list(REMOVE_ITEM sources "${PROJECT_SOURCE_DIR}/main.c")

file(GLOB tests "${PROJECT_SOURCE_DIR}/tests/*.cpp")
list(REMOVE_ITEM tests "${PROJECT_SOURCE_DIR}/tests/main.cpp")

foreach(file ${tests})
  set(name)
  get_filename_component(name ${file} NAME_WE)
  add_executable("${name}_tests"
    ${sources}
    ${file}
    "${PROJECT_SOURCE_DIR}/tests/main.cpp")
  target_link_libraries("${name}_tests" gtest_main)
  add_test(NAME ${name} COMMAND "${name}_tests")
endforeach()
