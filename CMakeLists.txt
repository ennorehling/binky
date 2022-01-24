cmake_minimum_required(VERSION 2.8...3.13)
project (binky C)

add_subdirectory(crpat)

add_executable(binky main.c gamedata.c crfile.c crmerge.c sds/sds.c)
target_link_libraries(binky crpat)

add_executable(unittest unittest.c CuTest.c gamedata.test.c gamedata.c)
target_link_libraries(unittest crpat)
