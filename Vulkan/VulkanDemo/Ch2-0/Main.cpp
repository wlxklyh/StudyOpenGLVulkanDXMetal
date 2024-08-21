#include "GlfwGeneral.hpp"

int main()
{
    if(!InitializeWindow({1280,720}))
    {
        return -1;
    }

    while(!glfwWindowShouldClose(pWindow))
    {
        glfwPollEvents();
        TileFps();
    }
    TerminateWindow();
}