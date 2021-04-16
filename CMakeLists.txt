cmake_minimum_required(VERSION 2.8...3.13)
project (binky C)

add_subdirectory(crpat)

add_executable(binky main.c gamedata.c crfile.c)
target_link_libraries(binky crpat)
