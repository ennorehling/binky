cmake_minimum_required(VERSION 3.13)
project (binky C)

add_subdirectory(crpat)
add_executable(binky main.c gamedata.c)
