#include "GlfwGeneral.hpp"

int main() {
	//初始化窗口
	if (!InitializeWindow({1280,720}))
		return -1;

	//Loop
	while (!glfwWindowShouldClose(pWindow)) {

		/*渲染过程，待填充*/

		glfwPollEvents();
		TitleFps();
	}
	TerminateWindow();
	return 0;
}