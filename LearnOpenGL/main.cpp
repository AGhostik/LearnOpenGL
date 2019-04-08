#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

const char* _readFile(const char* path) {
	ifstream::pos_type size;
	char * memblock;
	string text;

	// file read based on example in cplusplus.com tutorial
	ifstream file (path, ios::in|ios::binary|ios::ate);
	if (file.is_open())	{
		size = file.tellg();
		//fSize = (GLuint) size;
		memblock = new char [1 + size];
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();
		memblock[size] = '\0';
		text.assign(memblock);
	}
	else {
		return 0;
	}

	return memblock;
}

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

GLuint _shaders() {
	_log("Commencing shader compile");

	const char* vertexShaderSource = _readFile("shader1.vert");
	GLuint vertexShader = _createVertexShader(vertexShaderSource);
		
	const char *fragmentShaderSource = _readFile("shader2.frag");
	GLuint fragmentShader = _createFragmentShader(fragmentShaderSource);
	
	GLuint shaders[] = {
		vertexShader, fragmentShader
	};

	GLuint shaderProgram = _createShaderProgram(shaders, 2);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);
	_log("Shader compile complete");

	return shaderProgram;
}

void _draw(GLuint vao, GLuint shaderProgram) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram); 
	glBindVertexArray(vao); 

	//��� EBO
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
		
	GLfloat vertices[] = {
		0.5f,  0.5f, 0.0f,  // ������� ������ ����
		0.5f, -0.5f, 0.0f,  // ������ ������ ����
		-0.5f, -0.5f, 0.0f,  // ������ ����� ����
		-0.5f,  0.5f, 0.0f   // ������� ����� ����
	};
	GLuint indices[] = {  // �������, ��� �� �������� � 0!
		0, 1, 3,   // ������ �����������
		1, 2, 3    // ������ �����������
	};
	
	//Vertex Buffer Objects
	GLuint VBO;
	glGenBuffers(1, &VBO);

	//Index Buffer Objects
	GLuint IBO;
	glGenBuffers(1, &IBO);	

	//Vertex Array Object
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 1. ����������� VAO
	glBindVertexArray(VAO);
	// 2. �������� ��� ������ ������ � ����� ��� OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
	// 3. �������� ���� ������� � � ����� ��� OpenGL
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
	// 4. ������������� ��������� �� ��������� �������� 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); 
	// 5. ���������� VAO
	glBindVertexArray(0); 
		
	GLuint shaderProgram = _shaders();

	//����� wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//���������� �����
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_log("Commencing");

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		_draw(VAO, shaderProgram);
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}