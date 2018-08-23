cmake_minimum_required(VERSION 3.12)
project(hello C)

set(CMAKE_C_STANDARD 99)
set(phproot /usr/include/php/20170718/)
message("php root: ${phproot}")
include_directories(${phproot}TSRM/)
include_directories(${phproot}main/)
include_directories(${phproot}Zend/)

add_library(hello SHARED hello.c hello.h)