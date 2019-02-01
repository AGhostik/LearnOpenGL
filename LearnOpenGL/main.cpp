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

GLuint _createVertexShader(const char* shaderSource) {
	GLint success;
	GLchar infoLog[512];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);	
	glShaderSource(vertexShader, 1, &shaderSource, NULL);
	glCompileShader(vertexShader);	
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if(!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		_log("Shader _vertex_ compilation - failed");
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;		
	}
	else {
		_log("Shader _vertex_ compilation - success");
	}

	return vertexShader;
}

GLuint _createFragmentShader(const char* shaderSource) {
	GLint success;
	GLchar infoLog[512];

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	
	glShaderSource(fragmentShader, 1, &shaderSource, NULL);
	glCompileShader(fragmentShader);	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if(!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		_log("Shader _fragment_ compilation - failed");
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;		
	}
	else {
		_log("Shader _fragment_ compilation - success");
	}

	return fragmentShader;
}

GLuint _createShaderProgram(GLuint shaders[], int arraySize) {
	GLint success;
	GLchar infoLog[512];
	GLuint shaderProgram = glCreateProgram();
	
	for (int i = 0; i < arraySize; i++)	{
		glAttachShader(shaderProgram, shaders[i]);
	}

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		_log("Shader program compilation - failed");
	}

	return shaderProgram;
}

void _draw(GLuint vao, GLuint shaderProgram) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram); 
	glBindVertexArray(vao); 

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
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
	
	_log("Commencing shader compile");	
	const char *vertexShaderSource = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"void main() {\n"
		"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"}";
	GLuint vertexShader = _createVertexShader(vertexShaderSource);
		
	const char *fragmentShaderSource = 
		"#version 330 core\n"
		"out vec4 color;\n"
		"void main() {\n"
		"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}";
	GLuint fragmentShader = _createFragmentShader(fragmentShaderSource);
	
	GLuint shaders[] = {
		vertexShader, fragmentShader
	};

	GLuint shaderProgram = _createShaderProgram(shaders, 2);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);
	_log("Shader compile complete");

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	// 1. Привязываем VAO
	glBindVertexArray(VAO);
	// 2. Копируем наш массив вершин в буфер для OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
	// 3. Устанавливаем указатели на вершинные атрибуты 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); 
	//4. Отвязываем VAO
	glBindVertexArray(0); 

	_log("Commencing");

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		_draw(VAO, shaderProgram);
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}