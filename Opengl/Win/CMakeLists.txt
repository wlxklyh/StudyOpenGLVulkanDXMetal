cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

#（1）设置工程的名字、设置include目录 和 lib目录、设置cmake module 拆分模块避免cmakelists.txt过大、设置输出目录
project(LearnOpenGL)
set(LOCAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/includes)
set(LOCAL_LIB_DIR ${CMAKE_SOURCE_DIR}/dependencies/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
# 设置输出目录
if (WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)
# 设置C++11
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
# 添加非标准的共享库搜索路径
link_directories(${CMAKE_SOURCE_DIR}/dependencies/lib)



#（2）搜索外部库
find_package(GLM REQUIRED)
message(STATUS "Found GLM in ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
if(WIN32)
    set(LIBS glfw3 opengl32 assimp freetype irrKlang)
endif(WIN32)

#（3）项目内的库
add_library(STB_IMAGE "src/ThirdParty/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)
add_library(GLAD "src/ThirdParty/glad.c")
set(LIBS ${LIBS} GLAD)
set(HCORE_DIR src/Core/)

file(GLOB_RECURSE HCORE_CPP_FILES ${HCORE_DIR}/*.cpp)
add_library(HCore ${HCORE_CPP_FILES})
set(LIBS ${LIBS} HCore)

#（4）这个来把路径设置到工程里面
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)


#（5）组织工程 每一个节都是一个独立工程
# ????
macro(makeLink src dest target)
    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()


set(CHAPTERS "")
file(GLOB dirs "src/Samples/*")
foreach(dir ${dirs})
    if(IS_DIRECTORY ${dir})
        get_filename_component(dir_name ${dir} NAME)
        list(APPEND CHAPTERS ${dir_name})
    endif()
endforeach()


foreach(CHAPTER ${CHAPTERS})
    set(${CHAPTER} "")
    file(GLOB Chapter_Node_Dirs "src/Samples/${CHAPTER}/*")
    foreach(Chapter_Node_Dir ${Chapter_Node_Dirs})
        if(IS_DIRECTORY ${Chapter_Node_Dir})
            get_filename_component(dir_name ${Chapter_Node_Dir} NAME)
            list(APPEND ${CHAPTER} ${dir_name})
        endif()
    endforeach()
endforeach()




# then create a project file per tutorial
foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})
        file(GLOB SOURCE
                "src/Samples/${CHAPTER}/${DEMO}/*.h"
                "src/Samples/${CHAPTER}/${DEMO}/*.cpp"
                "src/Samples/${CHAPTER}/${DEMO}/*.vs"
                "src/Samples/${CHAPTER}/${DEMO}/*.fs"
                "src/Samples/${CHAPTER}/${DEMO}/*.gs"
                "src/Samples/${CHAPTER}/${DEMO}/*.glsl"
                )
        set(NAME "${CHAPTER}__${DEMO}")
        add_executable(${NAME} ${SOURCE})


        target_link_libraries(${NAME} ${LIBS})
        if(WIN32)
            file(GLOB dll_src "${CMAKE_SOURCE_DIR}/dlls/*.dll" )
            file(COPY ${dll_src} DESTINATION "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
            file(COPY ${dll_src} DESTINATION "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/Debug")

            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
            set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/Debug")
        elseif(UNIX AND NOT APPLE)
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
        elseif(APPLE)
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
        endif(WIN32)

        # 拷贝shader到build目录
        file(GLOB SHADERS
                "src/Samples/${CHAPTER}/${DEMO}/*.vs"
                "src/Samples/${CHAPTER}/${DEMO}/*.fs"
                "src/Samples/${CHAPTER}/${DEMO}/*.gs"
                "src/Samples/${CHAPTER}/${DEMO}/*.glsl"
                )
        foreach(SHADER ${SHADERS})
            if(WIN32)
                # configure_file(${SHADER} "test")
                target_compile_definitions(${NAME} PRIVATE "SLN_SOURCE_CODE_DIR=\"${CMAKE_SOURCE_DIR}/src/Samples/${CHAPTER}/${DEMO}/\"")
                add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)

            elseif(UNIX AND NOT APPLE)
                file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${CHAPTER})
            elseif(APPLE)
                # create symbolic link for *.vs *.fs *.gs
                get_filename_component(SHADERNAME ${SHADER} NAME)
                makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/${SHADERNAME} ${NAME})
            endif(WIN32)
        endforeach(SHADER)

        # if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
        if(MSVC)
            configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
        endif(MSVC)
    endforeach(DEMO)
endforeach(CHAPTER)

include_directories(${CMAKE_SOURCE_DIR}/dependencies/includes ${CMAKE_SOURCE_DIR}/src/Core)