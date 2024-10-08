﻿# CMakeList.txt : CMake project for MatrixExplorer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  # set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MatrixExplorer")
add_subdirectory(HulaScript)

# Add source to this project's executable.
add_executable (MatrixExplorer "MatrixExplorer.cpp" "matrix.h" "matrix.cpp" "print.cpp" "rows.cpp"  "rational.h" "rational.cpp")
set_property(TARGET MatrixExplorer PROPERTY CXX_STANDARD 20)
set_property(TARGET MatrixExplorer PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ttmath)
target_link_libraries(${PROJECT_NAME} PRIVATE HulaScript)

# TODO: Add tests and install targets if needed.
