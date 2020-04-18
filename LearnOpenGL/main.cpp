#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

// EBO - element buffer objects

const char* read_file(const char* path) {
	// file read based on example in cplusplus.com tutorial
	ifstream file (path, ios::in|ios::binary|ios::ate);
	if (file.is_open())	{
		const auto size = file.tellg();
		//fSize = (GLuint) size;
		auto* const memblock = new char [1 + size];
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();
		memblock[size] = '\0';
		string text;
		text.assign(memblock);

		return memblock;
	}
	else {
		return nullptr;
	}
}

static void log(const string& message) {
	#if _DEBUG
	cout << message << endl;
	#endif
}

void key_callback(GLFWwindow* window, const int key, int scancode, const int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		log("Complete");
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

GLuint create_shader(const char* shader_source_code, const int shader_type) {
	GLint success = 0;

	const GLuint shader = glCreateShader(shader_type);	
	glShaderSource(shader, 1, &shader_source_code, nullptr);
	glCompileShader(shader);	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	string message;

	if(!success) {
		const int log_size = 512;
		GLchar info_log[log_size];

		glGetShaderInfoLog(shader, log_size, nullptr, info_log);

		message = "Shader compilation - failed\n";
		message += info_log;
		log(message);
	}
	else {
		message = "Shader compilation - success";
		log(message);
	}

	return shader;
}

GLuint create_shader_program(GLuint shaders[], const int array_size) {
	GLint success = 0;
	GLchar info_log[512];
	const GLuint shader_program = glCreateProgram();
	
	for (int i = 0; i < array_size; i++)	{
		glAttachShader(shader_program, shaders[i]);
	}

	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
		log("Shader program compilation - failed");
	}

	return shader_program;
}

GLuint shaders() {
	log("Commencing shader program compile");

	const char* vertex_shader_source = read_file("shader1.vert");
	const GLuint vertex_shader = create_shader(vertex_shader_source, GL_VERTEX_SHADER);
		
	const char* fragment_shader_source = read_file("shader2.frag");
	const GLuint fragment_shader = create_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
	
	GLuint shaders[] = {
		vertex_shader, fragment_shader
	};

	const GLuint shader_program = create_shader_program(shaders, 2);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(shader_program);
	log("Shader program compile complete");

	return shader_program;
}

void draw(const GLuint vao, const GLuint shader_program) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader_program); 
	glBindVertexArray(vao); 

	//without EBO
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

int main()
{
	try
	{	
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
			log("Failed to create GLFW window");
			
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);

		glfwSetKeyCallback(window, key_callback);  

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) 	{
			log("Failed to initialize GLEW");
			
			return -1;
		}

		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);  
		glViewport(0, 0, width, height);
			
		GLfloat vertices[] = {
			0.5f,  0.5f, 0.0f,  // top right
			0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left
		};
		GLuint indices[] = {  // start from 0
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		
		//Vertex Buffer Objects
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);

		//Index Buffer Objects
		GLuint ibo = 0;
		glGenBuffers(1, &ibo);	

		//Vertex Array Object
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// 1. bind VAO
		glBindVertexArray(vao);
		// 2. copy vertex array into OpenGL buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
		// 3. copy indexes into OpenGL buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
		// 4. set pointers at vertex attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));
		glEnableVertexAttribArray(0); 
		// 5. unbind VAO
		glBindVertexArray(0);

		const GLuint shader_program = shaders();

		//wireframe mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//normal mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		log("Commencing");

		while(!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			draw(vao, shader_program);
			glfwSwapBuffers(window);
		}

		glfwTerminate();

		return 0;
	}
	catch (...)
	{
		return 0;
	}
}
