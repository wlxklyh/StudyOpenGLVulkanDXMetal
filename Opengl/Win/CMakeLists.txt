cmake_minimum_required(VERSION 3.7)
# 指定C++11
set(CMAKE_CXX_STANDARD 11)
MESSAGE(STATUS "Main cmake code")
set(NAME LearnOpenglDemo)
project(RookieRenderer)

ADD_SUBDIRECTORY(src)


# 计划这样组织
# src
# --core
# ----Util
# ----Math
# --sample
# dependencies
# --windows
# ----glew-2.0.0
# ----glfw-3.2.1