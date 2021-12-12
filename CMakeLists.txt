cmake_minimum_required(VERSION 3.10)
project(eccodes_bench
  LANGUAGES CXX
  VERSION 0.1)
set(CMAKE_CXX_COMPILER mpic++)
find_package(eccodes REQUIRED)
find_package(MPI REQUIRED)
add_executable(get_keys_recorddist src/get_keys_recorddist.cxx)
target_link_libraries(get_keys_recorddist eccodes mpi)

add_executable(get_keys_filedist src/get_keys_filedist.cxx)
target_link_libraries(get_keys_filedist eccodes mpi)


add_executable(mpi_bench src/mpi_bench.cxx)
target_link_libraries(mpi_bench mpi)


