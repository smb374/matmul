cmake_minimum_required(VERSION 3.31)
project(matmul)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-march=native -Wall ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-mavx2 -mprefer-vector-width=256 ${CMAKE_CXX_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenMP)
find_package(MPI REQUIRED)

include_directories(${MPI_CXX_INCLUDE_DIRS})
link_libraries(${MPI_CXX_LIBRARIES} OpenMP::OpenMP_CXX)

set(HELPERS matrix_io.cpp matrix_io.h malloc2D.cpp malloc2D.h basic_op.cpp basic_op.h)

add_executable(playground main.cpp ${HELPERS})
add_executable(matvec_mpi matvec_mpi.cpp ${HELPERS})
add_executable(matvec_thread matvec_thread.cpp ${HELPERS})
add_executable(matvec_thread_grid matvec_thread_grid.cpp ${HELPERS})
add_executable(matvec_mpi_grid matvec_mpi_grid.cpp ${HELPERS})
add_executable(type_index_example type_index_example.cpp)

add_custom_target(all_bins)
add_dependencies(all_bins matvec_mpi matvec_thread matvec_thread_grid matvec_mpi_grid)
