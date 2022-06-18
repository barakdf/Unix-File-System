cmake_minimum_required(VERSION 3.16)
project(Unix_File_System C)

set(CMAKE_C_STANDARD 99)

add_executable(Unix_File_System ufs.c ufs.h myfile.c myfile.h ufs_test.c)
