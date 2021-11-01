#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "CoreHeader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/camera.h>



int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

float g_vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

// pos scale rotate light color
glm::vec3 g_light_direction[] = {
        glm::vec3(0.0f, 1.0f, 1.0f)
};


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos);

void mouse_callback(GLFWwindow *window, int button, int action, int mods);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

uint32 GenerateVAO(int32 verticesSize, float *vertices, int32 indicesSize = 0, uint32 *indices = NULL);

uint32 GenerateTexture(const char *path);

int main() {


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return -1;
    }

    uint32 VAO = GenerateVAO(sizeof(g_vertices), g_vertices);

    uint32 boxTexDiffuse = GenerateTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
    uint32 boxTexSpecular = GenerateTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());

    std::string lightvsPath = SLN_SOURCE_CODE_DIR + std::string("light.vs.glsl");
    std::string lightfsPath = SLN_SOURCE_CODE_DIR + std::string("light.fs.glsl");
    std::string colorvsPath = SLN_SOURCE_CODE_DIR + std::string("cube.vs.glsl");
    std::string colorfsPath = SLN_SOURCE_CODE_DIR + std::string("cube.fs.glsl");
    Shader shader[2] = {Shader(lightvsPath.c_str(), lightfsPath.c_str()),
                        Shader(colorvsPath.c_str(), colorfsPath.c_str())};



    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glBindVertexArray(VAO);
        shader[1].use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boxTexDiffuse);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, boxTexSpecular);

        shader[1].setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        shader[1].setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        shader[1].setVec3("light.position",  5*sin(lastFrame), 0, 5*abs(cos(lastFrame)));
        shader[1].setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader[1].setFloat("light.constant",  1.0f);
        shader[1].setFloat("light.linear",    0.09f);
        shader[1].setFloat("light.quadratic", 0.032f);
        shader[1].setInt("material.diffuceTex", 0);
        shader[1].setInt("material.specularTex", 1);
        shader[1].setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        shader[1].setFloat("material.shininess", 32.0f);

        srand(0);
        //随机的
        for(int i=0; i < 30; i++){
            shader[1].use();
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                    100.0f);
            shader[1].setMat4("projection", projection);
            shader[1].setMat4("view", view);
            shader[1].setVec3("viewPos", camera.Position);

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, glm::vec3(frand(-6,6),frand(-3,3),frand(-3,0)));
//            model = glm::rotate(model, frand(0,360), glm::vec3(1.0f, 0.0f, 0.0f));
//            model = glm::rotate(model, frand(0,360), glm::vec3(0.0f, 1.0f, 0.0f));
//            model = glm::rotate(model, frand(0,360), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(frand(1.0,1.5)));
            shader[1].setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertices) / sizeof(float) / 3);
        }

        //光
//        {
//            shader[0].use();
//            glm::mat4 view = camera.GetViewMatrix();
//            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
//                                                    100.0f);
//            shader[0].setMat4("projection", projection);
//            shader[0].setMat4("view", view);
//            glm::mat4 model = glm::mat4(1.0f);
//            model = glm::translate(model, g_light_position[0]);
//            model = glm::rotate(model, glm::radians(g_light_position[2].x), glm::vec3(1.0f, 0.0f, 0.0f));
//            model = glm::rotate(model, glm::radians(g_light_position[2].y), glm::vec3(0.0f, 1.0f, 0.0f));
//            model = glm::rotate(model, glm::radians(g_light_position[2].z), glm::vec3(0.0f, 0.0f, 1.0f));
//            model = glm::scale(model, g_light_position[1]);
//            shader[0].setMat4("model", model);
//            glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertices) / sizeof(float) / 3);
//        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

uint32 GenerateTexture(const char *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

uint32 GenerateVAO(int32 verticesSize, float *vertices, int32 indicesSize, uint32 *indices) {
    uint32 VAO;
    uint32 VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return VAO;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


bool mousePress = false;

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos) {
    if (mousePress) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

}

void mouse_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            std::cout << "+GLFW_PRESS\n";
            firstMouse = true;
            mousePress = true;
        }

        if (action == GLFW_RELEASE) {
            std::cout << "-GLFW_RELEASE\n";
            mousePress = false;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}