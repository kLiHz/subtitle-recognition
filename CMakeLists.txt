cmake_minimum_required(VERSION 3.25)
project(subrec)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(fmt CONFIG REQUIRED)
find_package(Tesseract CONFIG REQUIRED)

add_library(mysrt OBJECT srt.h srt.cpp)
target_link_libraries(mysrt fmt::fmt)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ${OpenCV_LIBS} Boost::program_options fmt::fmt Tesseract::libtesseract mysrt)
