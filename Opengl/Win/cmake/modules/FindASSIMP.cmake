# 查询Assimp
# 从相对路径里面找然后赋值给变量ASSIMP_INCLUDE_DIR 用法：FIND_PATH(<VAR> name path1 path2 …)
find_path(ASSIMP_INCLUDE_DIR assimp/mesh.h ${LOCAL_INCLUDE_DIR})
# 从相对路径里面找然后赋值给变量ASSIMP_INCLUDE_DIR 用法：FIND_LIBRARY(<VAR> name path1 path2 …)
find_library(ASSIMP_LIBRARY assimp ${LOCAL_LIB_DIR})
