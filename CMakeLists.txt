cmake_minimum_required(VERSION 3.7)
project(yoda)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp calc_value.hpp core.hpp win_supp.h lolcat_version.h process_line.hpp string_stack.cpp string_stack.hpp terminal_colors.cpp terminal_colors.hpp user_variables.cpp user_variables.hpp utils.hpp list.hpp lambda.hpp termio.hpp termio.cpp user_types.hpp user_types.cpp top_modifiers.hpp calc_value.cpp process_line.cpp utils.cpp core.cpp lolcat_version.cpp list.cpp win_supp.cpp top_modifiers.cpp)
add_executable(yoda ${SOURCE_FILES})
