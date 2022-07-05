#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader.h>
#include "CoreHeader.h"

struct DrawElementsCommand
{
    GLuint vertexCount;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// 三角形的顶点数据 是在NDC范围
float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角 0
        0.5f, -0.5f, 0.0f,  // 右下角 1
        -0.5f, -0.5f, 0.0f, // 左下角 2
        -0.5f, 0.5f, 0.0f   // 左上角 3
};

unsigned int indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800,600,"LearnOpenGL",NULL,NULL);
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

//==本节核心代码 Begin==
    //(1)计算 instance 数据
    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }
    //(2)生成 instance buffer
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //(3)生成 单个instance的VAO VBO
    float quadVertices[] = {
            // positions     // colors
            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
            0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
            -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
            0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
            0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    //!!!! 顶点的第三个位置给instance buffer !!!!!
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //第2个参数 0：默认值，1个顶点更新;  1:1个实例更新; 2:2个实例更新;
    glVertexAttribDivisor(2, 1);

//==本节核心代码 End==



    std::string vsGreenPath = SLN_SOURCE_CODE_DIR + std::string("green.vs.glsl");
    std::string fsGreenPath = SLN_SOURCE_CODE_DIR + std::string("green.fs.glsl");
    std::string vsRedPath = SLN_SOURCE_CODE_DIR + std::string("red.vs.glsl");
    std::string fsRedPath = SLN_SOURCE_CODE_DIR + std::string("red.fs.glsl");
    Shader greenShader(vsGreenPath.c_str(), fsGreenPath.c_str());
    Shader redShader(vsGreenPath.c_str(), fsGreenPath.c_str());



    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        //渲染指令
        //...
        //...
        //...
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

//==本节核心代码 Begin==
        shader.use();
        //draw
        //（1）这个是NultiDrawIndirect
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glUseProgram(gProgramRed);
        //
        // glMultiDrawElementsIndirect(GL_TRIANGLES, //type
        //     GL_UNSIGNED_INT, //indices represented as unsigned ints
        //     (GLvoid*)0, //start with the first draw command
        //     100, //draw 100 objects
        //     0); //no stride, the draw commands are tightly packed

        //（2）这个是Indirect Draw
        for(int j = 0; j < 100; j++)
        {
            if(j % 2 == 0)
            {
                glDisable(GL_BLEND);
                greenShader.use();
            } else
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                redShader.use();
            }
            glDrawElementsIndirect(GL_TRIANGLES,
                                   GL_UNSIGNED_INT,
                                   (GLvoid*)(0 + j*sizeof(SDrawElementsCommand))
            );
        }
//==本节核心代码 End==


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
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