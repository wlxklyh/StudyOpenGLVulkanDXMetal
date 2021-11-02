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

set(CHAPTERS
        Chapter1.getting_started
        Chapter2.lighting
        Chapter3.model_loading
        Chapter4.advanced_opengl
        Chapter5.advanced_lighting
        Chapter6.pbr
        Chapter7.in_practice
        )

set(Chapter1.getting_started
        Chapter1_1.1.hello_window
        Chapter1_1.2.hello_window_clear
        Chapter1_2.1.hello_triangle
        Chapter1_2.2.hello_triangle_indexed
        Chapter1_2.3.hello_triangle_exercise1
        Chapter1_2.4.hello_triangle_exercise2
        Chapter1_2.5.hello_triangle_exercise3
        Chapter1_3.1.shaders_uniform
        Chapter1_3.2.shaders_interpolation
        Chapter1_3.3.shaders_class
        Chapter1_3.4.shaders_exercise1
        Chapter1_3.5.shaders_exercise2
        Chapter1_3.6.shaders_exercise3
        Chapter1_4.1.textures
        Chapter1_4.2.textures_combined
        Chapter1_4.3.textures_exercise1
        Chapter1_4.4.textures_exercise2
        Chapter1_4.5.textures_exercise3
        Chapter1_4.6.textures_exercise4
        Chapter1_5.1.transformations
        Chapter1_5.2.transformations_exercise2
        Chapter1_6.1.coordinate_systems
        Chapter1_6.2.coordinate_systems_depth
        Chapter1_6.3.coordinate_systems_multiple
        Chapter1_7.1.camera_circle
        Chapter1_7.2.camera_keyboard_dt
        Chapter1_7.3.camera_mouse_zoom
        Chapter1_7.4.camera_class
        )

set(Chapter2.lighting
        Chapter2_1.colors
        Chapter2_2.1.basic_lighting_diffuse
        Chapter2_2.2.basic_lighting_specular
        Chapter2_3.1.materials
        Chapter2_3.2.materials_exercise1
        Chapter2_4.1.lighting_maps_diffuse_map
        Chapter2_4.2.lighting_maps_specular_map
        Chapter2_4.4.lighting_maps_exercise4
        Chapter2_5.1.light_casters_directional
        Chapter2_5.2.light_casters_point
        Chapter2_5.3.light_casters_spot
        Chapter2_5.4.light_casters_spot_soft
        Chapter2_6.multiple_lights
        )

set(Chapter3.model_loading
        Chapter3_1.model_loading
        )

set(Chapter4.advanced_opengl
        Chapter4_1.1.depth_testing
        Chapter4_1.2.depth_testing_view
        Chapter4_2.stencil_testing
        Chapter4_3.1.blending_discard
        Chapter4_3.2.blending_sort
        Chapter4_5.1.framebuffers
        Chapter4_5.2.framebuffers_exercise1
        Chapter4_6.1.cubemaps_skybox
        Chapter4_6.2.cubemaps_environment_mapping
        Chapter4_8.advanced_glsl_ubo
        Chapter4_9.1.geometry_shader_houses
        Chapter4_9.2.geometry_shader_exploding
        Chapter4_9.3.geometry_shader_normals
        Chapter4_10.1.instancing_quads
        Chapter4_10.2.asteroids
        Chapter4_10.3.asteroids_instanced
        Chapter4_11.1.anti_aliasing_msaa
        Chapter4_11.2.anti_aliasing_offscreen
        )

set(Chapter5.advanced_lighting
        Chapter5_1.advanced_lighting
        Chapter5_2.gamma_correction
        Chapter5_3.1.1.shadow_mapping_depth
        Chapter5_3.1.2.shadow_mapping_base
        Chapter5_3.1.3.shadow_mapping
        Chapter5_3.2.1.point_shadows
        Chapter5_3.2.2.point_shadows_soft
        Chapter5_4.normal_mapping
        Chapter5_5.1.parallax_mapping
        Chapter5_5.2.steep_parallax_mapping
        Chapter5_5.3.parallax_occlusion_mapping
        Chapter5_6.hdr
        Chapter5_7.bloom
        Chapter5_8.1.deferred_shading
        Chapter5_8.2.deferred_shading_volumes
        Chapter5_9.ssao
        )

set(Chapter6.pbr
        Chapter6_1.1.lighting
        Chapter6_1.2.lighting_textured
        Chapter6_2.1.1.ibl_irradiance_conversion
        Chapter6_2.1.2.ibl_irradiance
        Chapter6_2.2.1.ibl_specular
        Chapter6_2.2.2.ibl_specular_textured
        )

set(Chapter7.in_practice
        Chapter7_1.debugging
        Chapter7_2.text_rendering
        #Chapter7_3.2d_game
        )




# 计划这样组织
# src
# --core
# ----Util
# ----Math
# --sample
# ----Chapter1.getting_started
# ----Chapter2
# ----Chapter3
# ----Chapter4
# ----Chapter5
# ----Chapter6
# ----Chapter7
# ----Chapter8
# ----Advanced
# dependencies
# --include
# --lib

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