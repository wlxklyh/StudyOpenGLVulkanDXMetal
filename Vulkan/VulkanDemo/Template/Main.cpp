#include "GlfwGeneral.hpp"

int main() {
	if (!InitializeWindow({1280,720}))
		return -1;//来个你讨厌的返回值
	while (!glfwWindowShouldClose(pWindow)) {

		/*渲染过程，待填充*/

		glfwPollEvents();
		TitleFps();
	}
	TerminateWindow();
	return 0;
}