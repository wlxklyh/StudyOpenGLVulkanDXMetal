#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader.h>
#include <learnopengl/filesystem.h>
#include "stb_image.h"
#include "CoreHeader.h"
// 三角形的顶点数据 是在NDC范围  远是1 近是-1
float g_vertices[] = {
//     ---- 位置 ----         ---- 位置 ----
        1.0f,  1.0f, 0.1f, 1.0f, 0.0f, 0.0f,//红色
        1.0f, -1.0f, 0.1f, 1.0f, 0.0f, 0.0f,//红色
        -1.0f,  1.0f, 0.1f, 1.0f, 0.0f, 0.0f,//红色

        1.0f,  1.0f, 0.8f, 0.0f, 1.0f, 0.0f,//绿色
        1.0f, -1.0f, 0.8f, 0.0f, 1.0f, 0.0f,//绿色
        -1.0f,  1.0f, 0.8f, 0.0f, 1.0f, 0.0f,//绿色
};
uint32 g_indices[] =
{
        0, 1, 2, // first triangle
        3, 4, 5, // first triangle
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
uint32 GenerateVAO(int32 verticesSize, float *vertices,int32 indicesSize=0,uint32 *indices=NULL);


int main()
{
    int width = 500;
    int height = 500;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(width,height,"LearnOpenGL",NULL,NULL);
    if(window == NULL)
    {
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        return -1;
    }

    uint32 VAO = GenerateVAO(sizeof(g_vertices), g_vertices,sizeof(g_indices), g_indices);

    std::string vsPath = SLN_SOURCE_CODE_DIR + std::string("vs.glsl");
    std::string fsPath = SLN_SOURCE_CODE_DIR + std::string("fs.glsl");
    Shader shader(vsPath.c_str(), fsPath.c_str());

    // 绑定 uniform atomic_uint counter 变量
    GLuint counterBuffer;
    glGenBuffers(1, &counterBuffer);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterBuffer);

    // 将 counterBuffer 与 counterLocation 绑定
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    // 设置 uniform atomic_uint counter 变量的值
    GLuint initialValue = 0;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &initialValue);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    bool OpenDepth = true;
    if(OpenDepth){
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    int frame = 0;
    while(!glfwWindowShouldClose(window))
    {
        frame+=1;
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        if(OpenDepth)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        else
            glClear(GL_COLOR_BUFFER_BIT);

        GLuint newValue = 0;
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
        glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &newValue);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);

        // 变量用于存储读取的计数器值
        GLuint counterValue = 0;
        // 绑定原子计数器缓冲区对象
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
        // 从GPU读取原子计数器的值
        glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &counterValue);
        // 解绑原子计数器缓冲区对象
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
        printf("counterValue:%d \n", counterValue);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


uint32 GenerateVAO(int32 verticesSize, float *vertices,int32 indicesSize,uint32 *indices) {
    uint32 VAO;
    uint32 VBO;
    uint32 EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}