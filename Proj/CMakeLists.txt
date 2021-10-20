cmake_minimum_required(VERSION 3.7)
# 指定C++11
set(CMAKE_CXX_STANDARD 11)
MESSAGE(STATUS "Main cmake code")
set(NAME RookieRenderer)
project(RookieRenderer)

ADD_SUBDIRECTORY(src)