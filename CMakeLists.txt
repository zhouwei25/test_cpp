add_executable(main test.cpp)

add_library(test1 SHARED test1/test1.cpp)
target_link_libraries(main test1)