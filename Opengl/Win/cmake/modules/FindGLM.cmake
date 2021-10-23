# 查询GLFW3
# 从相对路径里面找然后赋值给变量  用法：FIND_PATH(<VAR> name path1 path2 …)
find_path(GLM_INCLUDE_DIR "glm/glm.hpp" PATHS ${LOCAL_INCLUDE_DIR})

