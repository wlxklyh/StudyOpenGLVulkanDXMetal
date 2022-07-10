#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "CoreHeader.h"

const char* vertexShaderStr = "#version 330 core\n"
                              "layout (location = 0) in vec3 aPos;\n"
                              "\n"
                              "void main()\n"
                              "{\n"
                              "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                              "}";

const char* fragShaderStr = "#version 330 core\n"
                            "out vec4 FragColor;\n"
                            "\n"
                            "void main()\n"
                            "{\n"
                            "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                            "} ";

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
    uint32 vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderStr,NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
        std::cout<<"ERROR VertexShader:"<<infoLog<<std::endl;
    }

    uint32 fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader,1,&fragShaderStr,NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragShader,512,NULL,infoLog);
        std::cout<<"ERROR FragShader:"<<infoLog<<std::endl;
    }

    uint32 shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout<<"ERROR FragShader:"<<infoLog<<std::endl;
    }
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    uint32 VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    uint32 VBO;
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    //第一个参数：对应layout(location = 0)
    //第二个参数：指定顶点属性的大小跟第三个参数GL_FLOAT 指明一个顶点是 3xfloat
    //第三个参数：
    //第四个参数：是否normalize
    //第五个参数：步长 一个顶点对应多少数据 下一个顶点在多少数据后面
    //第六个参数：偏移 offset
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
    //启用顶点属性
    glEnableVertexAttribArray(0);

    uint32 EBO;
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);


//==本节核心代码 End==


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
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)*3));
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