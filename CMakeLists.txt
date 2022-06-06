cmake_minimum_required(VERSION 3.16)
project(Unix_File_System C)

set(CMAKE_C_STANDARD 99)

add_executable(Unix_File_System main.c ufs.c ufs.h)
