# 查询GLFW3
# 从相对路径里面找然后赋值给变量  用法：FIND_PATH(<VAR> name path1 path2 …)
find_path(GLFW3_INCLUDE_DIR "GLFW/glfw3.h" PATHS ${LOCAL_INCLUDE_DIR})
# 从相对路径里面找然后赋值给变量  用法：FIND_LIBRARY(<VAR> name path1 path2 …)
find_library(GLFW3_LIBRARY NAMES glfw3 PATHS ${LOCAL_LIB_DIR})
