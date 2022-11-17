#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct Vertex {
	float pos[2];

	Vertex()
	{
		pos[0] = 0.0f;
		pos[1] = 0.0f;
	};
	Vertex(float _x, float _y)
	{
		pos[0] = _x;
		pos[1] = _y;
	};
	float getX() {
		return pos[0];
	}
	float setX(float _x) {
		 pos[0] = _x;
	}
	
	float getY() {
		return pos[1];
	}
	float setY(float _y) {
		 pos[1] = _y;
	}


	__declspec(property(get = getX, put = setX)) float x;
	__declspec(property(get = getY, put = setY)) float y;
};

struct ShaderProgramSources {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSources ParseShader(const std::string& fBuffer) {
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::ifstream stream(fBuffer);
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("Vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("Fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}

	}

	return { ss[0].str(), ss[1].str() };
};

static unsigned int CompileShader(const std::string& _source, unsigned int _type) {
	unsigned int id = glCreateShader(_type);
	const char* src = _source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* errMsg = (char*)(alloca(sizeof(char) * length));
		glGetShaderInfoLog(id, length, &length, errMsg);
		std::cout << "Shader Compilation Failed. Type: " <<
			(_type == GL_VERTEX_SHADER ? "Vertex Shader" : "Pixel Shader") << std::endl;
		std::cout << errMsg << std::endl;
		glDeleteShader(id);

		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& _vertexShader, const std::string& _fragmentShader) {
	//create shaders
	unsigned int program = glCreateProgram();
	unsigned int vertexShader = CompileShader(_vertexShader, GL_VERTEX_SHADER);
	unsigned int pixelShader = CompileShader(_fragmentShader, GL_FRAGMENT_SHADER);

	//link created shaders
	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);
	glLinkProgram(program);
	glValidateProgram(program);

	//delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(pixelShader);

	return program;
};

int main(void) {
#pragma region variables
	//variables
	GLFWwindow* window;
	float positions[] =
	{
		-0.5f, -0.5f, //0
		 0.5f, -0.5f, //1
		 0.5f,  0.5f, //2
		-0.5f,  0.5f, //3
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	Vertex testVert = Vertex(0.5, -0.5);

	std::cout << "testVert.x was: " << testVert.x << std::endl;
	std::cout << "testVert.y was: " << testVert.y << std::endl;
#pragma endregion

#pragma region glfw Init
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
#pragma endregion

#pragma region Glew Init
	if (GLEW_OK != glewInit())
	{
		std::cout << "UH OH, something bad happened" << std::endl;
	}
	std::cout << glGetString(GL_VERSION) << std::endl;
#pragma endregion

#pragma region Buffer Creation
	//create buffers
	unsigned int buffers;
	glGenBuffers(1, &buffers);

	//after creation you have to select the buffer (select = binding)
	//BindBuffer(buffertype, buffer)
	glBindBuffer(GL_ARRAY_BUFFER, buffers);
	//giving the buffer some data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//INDEX BUFFER
	unsigned int indexBuffers;
	glGenBuffers(1, &indexBuffers);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);
#pragma endregion


#pragma region Shaders
	ShaderProgramSources sources = ParseShader("res/shaders/Basic.shader");
	std::cout << sources.VertexSource << std::endl;
	std::cout << sources.FragmentSource << std::endl;
	unsigned int shader = CreateShader(sources.VertexSource, sources.FragmentSource);
	glUseProgram(shader);
#pragma endregion


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//issue a draw call for the current buffer
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
#pragma region Shutdown
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
#pragma endregion   
}