#（1）把文件夹下的cpp文件名字添加到SRC_List 然后组织文件夹结构
#（1.1）把目录下的文件收集到变量里面
file(GLOB FRAMEWORK_SOURCE Framework/*.cpp Framework/*.hpp Framework/*.inl Framework/*.h)
file(GLOB RMATH_SOURCE RMath/*.cpp RMath/*.hpp RMath/*.inl RMath/*.h)
file(GLOB MAIN_SOURCE Main/*.cpp Main/*.hpp Main/*.inl Main/*.h)
file(GLOB unittest_source UnitTest/*.cpp UnitTest/*.hpp UnitTest/*.inl UnitTest/*.h)
file(GLOB MATERIAL_SOURCE Material/*.cpp Material/*.hpp Material/*.inl Material/*.h)
file(GLOB PLATFORM_SOURCE Platform/*.cpp Platform/*.hpp Platform/*.inl Platform/*.h)


#（1.2）把每个文件夹目录里面的文件列表弄到分组里面
source_group("Framework" FILES ${FRAMEWORK_SOURCE})
source_group("RMath" FILES ${RMATH_SOURCE})
source_group("Main" FILES ${MAIN_SOURCE})
source_group("UnitTest" FILES ${UNITTEST_SOURCE})
source_group("Material" FILES ${MATERIAL_SOURCE})
source_group("Platform" FILES ${PLATFORM_SOURCE})


ADD_COMPILE_DEFINITIONS(_USE_MATH_DEFINES)

#（2）添加一个可执行的文件
ADD_EXECUTABLE(${PROJECT_NAME}
        ${FRAMEWORK_SOURCE}
        ${RMATH_SOURCE}
        ${MATERIAL_SOURCE}
        ${PLATFORM_SOURCE}
        ${MAIN_SOURCE})

#（3）设置可执行的文件 放在哪个目录
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#（4）添加include 目录 这样cpp里面可以直接include这些目录的文件
INCLUDE_DIRECTORIES(Material)
INCLUDE_DIRECTORIES(Framework)
INCLUDE_DIRECTORIES(Main)
INCLUDE_DIRECTORIES(RMath)
INCLUDE_DIRECTORIES(Platform)


#（5）如果是VS则设置启动项目
if (MSVC)
    # Set the startup project .
    if (NOT ${CMAKE_VERSION} VERSION_LESS "3.6.0")
        set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    endif ()
endif (MSVC)


#（6）单元测试
INCLUDE(FetchContent)
FETCHCONTENT_DECLARE(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FETCHCONTENT_MAKEAVAILABLE(googletest)


INCLUDE_DIRECTORIES(Material)
INCLUDE_DIRECTORIES(RMath)
INCLUDE_DIRECTORIES(Framework)
INCLUDE_DIRECTORIES(UnitTest)
INCLUDE_DIRECTORIES(Platform)
# Now simply link against gtest or gtest_main as needed. Eg
ADD_EXECUTABLE(UnitTest
        ${UNITTEST_SOURCE}
        ${FRAMEWORK_SOURCE}
        ${RMath_SOURCE}
        ${PLATFORM_SOURCE})


TARGET_LINK_LIBRARIES(UnitTest gtest_main)
ADD_TEST(NAME example_test COMMAND UnitTest)
