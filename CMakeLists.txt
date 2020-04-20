set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE  "-Wl -Map=output.map")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

add_executable(main test.cpp)

add_library(test1 test1/test1.cpp)
target_link_libraries(test1 dbghelp)
target_link_libraries(main test1)

