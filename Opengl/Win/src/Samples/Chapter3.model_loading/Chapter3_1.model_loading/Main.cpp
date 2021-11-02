#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "CoreHeader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/camera.h>
#include "Model.h"
using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos);

void mouse_callback(GLFWwindow *window, int button, int action, int mods);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);


void DrawModel(Shader &modelShader, Model &ourModel, int modelIndex);

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
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    std::string model_loadingvsPath = SLN_SOURCE_CODE_DIR + std::string("model_loading.vs.glsl");
    std::string model_loadingfsPath = SLN_SOURCE_CODE_DIR + std::string("model_loading.fs.glsl");

    Shader modelShader = Shader(model_loadingvsPath.c_str(), model_loadingfsPath.c_str());
    Model backpackModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    Model cyborgModel(FileSystem::getPath("resources/objects/cyborg/cyborg.obj"));
    Model nanosuitModel(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj"));



    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawModel(modelShader, backpackModel, 0);
        DrawModel(modelShader, cyborgModel, 1);
        DrawModel(modelShader, nanosuitModel, 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void DrawModel(Shader &modelShader, Model &ourModel, int modelIndex) {
    modelShader.use();

    modelShader.setBool("dirLights[0].use", true);
    modelShader.setVec3("dirLights[0].direction", 0.5f, 0.5f, 0.5f);
    modelShader.setVec3("dirLights[0].ambient",  0.2f, 0.2f, 0.2f);
    modelShader.setVec3("dirLights[0].diffuse",0.5f, 0.5f, 0.5f);
    modelShader.setVec3("dirLights[0].specular", 1.0f, 1.0f, 1.0f);


    modelShader.setBool("pointLights[0].use", true);
    modelShader.setVec3("pointLights[0].position", sin(lastFrame) *100, 0.0 , 10.0f);
    modelShader.setVec3("pointLights[0].ambient",  0.2f, 0.2f, 0.2f);
    modelShader.setVec3("pointLights[0].diffuse",0.5f, 0.5f, 0.5f);
    modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    modelShader.setFloat("pointLights[0].constant",  1.0f);
    modelShader.setFloat("pointLights[0].linear",    0.09f);
    modelShader.setFloat("pointLights[0].quadratic", 0.032f);

    modelShader.setBool("spotLights[0].use", true);
    modelShader.setVec3("spotLights[0].direction", cos(lastFrame) * 100, sin(lastFrame) * 100, 10.0f);
    modelShader.setVec3("spotLights[0].position", 0.0f, 0.0f, 20.0f);
    modelShader.setVec3("spotLights[0].ambient",  0.1f, 0.1f, 0.1f);
    modelShader.setVec3("spotLights[0].diffuse",0.5f, 0.5f, 0.5f);
    modelShader.setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
    modelShader.setFloat("spotLights[0].constant",  1.0f);
    modelShader.setFloat("spotLights[0].linear",    0.09f);
    modelShader.setFloat("spotLights[0].quadratic", 0.032f);
    modelShader.setFloat("spotLights[0].cutOff", cos(glm::radians(12.0)));
    modelShader.setFloat("spotLights[0].outerCutOff", cos(glm::radians(17.5f)));

    modelShader.setFloat("material.shininess", 32.0f);


    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    modelShader.setMat4("projection", projection);
    modelShader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-9 + modelIndex * 9, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    modelShader.setMat4("model", model);
    ourModel.Draw(modelShader);
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