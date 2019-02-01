#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#if _DEBUG
#include <iostream>
#endif
#if !_DEBUG
#include <Windows.h>
#endif

static void _log(const char* message) {
	#if _DEBUG
	std::cout << message << std::endl;
	#endif
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		_log("Complete");
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void _draw() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main() {
	glfwInit();
	//min OpenGL version - 3.3 - major.minor
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	#if !_DEBUG
	FreeConsole();
	#endif

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);
	if (window == nullptr) {	
		_log("Failed to create GLFW window");
		
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);  

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) 	{
		_log("Failed to initialize GLEW");
		
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);  
	glViewport(0, 0, width, height);

	_log("Commencing");

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		_draw();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}