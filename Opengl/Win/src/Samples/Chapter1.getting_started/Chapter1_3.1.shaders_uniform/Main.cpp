#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "CoreHeader.h"

// 着色器数据
const char* vertexShaderStrT1 = "#version 330 core\n"
                                "layout (location = 0) in vec3 aPos;\n"
                                "\n"
                                "void main()\n"
                                "{\n"
                                "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                "}";

const char* fragShaderStrT1 = "#version 330 core\n"
                              "out vec4 FragColor;\n"
                              "\n"
                              "uniform vec4 ourColor;\n"
                              "\n"
                              "void main()\n"
                              "{\n"
                              "    FragColor = ourColor;\n"
                              "} ";


// 三角形的顶点数据 是在NDC范围
float verticesT1[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
uint32 GenerateVAO(uint32 &VAO, uint32 &VBO, int32 verticesSize, float *vertices);
uint32 GeneratePrograme(const char *vertexShaderStr, const char *fragShaderStr);

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

    uint32 shaderProgramT1 = GeneratePrograme(vertexShaderStrT1, fragShaderStrT1);

    uint32 VAOT1;
    uint32 VBOT1;
    GenerateVAO(VAOT1, VBOT1, sizeof(verticesT1), verticesT1);



    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        //渲染指令
        //...
        //...
        //...
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramT1);
        float timeValue = glfwGetTime();
        float redValue = (sin(timeValue) / 2.0) + 0.5;
        //重点
        int vertexColorLocation = glGetUniformLocation(shaderProgramT1, "ourColor");
        //后面四个参数是这个vec4的四个分量
        glUniform4f(vertexColorLocation,redValue,0.0f,0.0f,1.0f);

        glBindVertexArray(VAOT1);
        glDrawArrays(GL_TRIANGLES,0,3);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

uint32 GeneratePrograme(const char *vertexShaderStr, const char *fragShaderStr) {
    uint32 vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderStr, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
        std::cout<<"ERROR VertexShader:"<<infoLog<<std::endl;
    }

    uint32 fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderStr, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragShader,512,NULL,infoLog);
        std::cout<<"ERROR FragShader:"<<infoLog<<std::endl;
    }

    uint32 shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout<<"ERROR FragShader:"<<infoLog<<std::endl;
    }
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    return shaderProgram;
}

uint32 GenerateVAO(uint32 &VAO, uint32 &VBO, int32 verticesSize, float *vertices) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
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