cmake_minimum_required(VERSION 3.22)
project(hash_file C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable( ${PROJECT_NAME}
        hash_file.c
        hash_file.h
        jt_record.c
        jt_record.h)
