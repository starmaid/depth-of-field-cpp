// dof.cpp : Defines the entry point for the application.
//


#include "dof.h"
#include <opencv2/core.hpp>
#include <librealsense2/rs.hpp>
#include <GL/glew.h>
//#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "example-utils.hpp"
#include "example.hpp"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

enum class direction
{
	to_depth,
	to_color
};


//const char* vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 aPos;\n"
//"void main()\n"
//"{\n"
//"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//"}\0";
//const char* fragmentShaderSource = "#version 330 core\n"
//"out vec4 FragColor;\n"
//"void main()\n"
//"{\n"
//"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//"}\n\0";


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(int argc, char* argv[])
{
	// init everything!!!
	//glutInit(&argc, argv);
	//glutCreateWindow("GLEW Test");
	
	if (!glfwInit()) {
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	}

	cout << "Built with CMAKE. OpenCV: " << cv::getVersionString()
		<< " Realsense: " << RS2_API_VERSION_STR
		<< " GLEW: " << glewGetString(GLEW_VERSION)
		<< endl;

	if (GLEW_VERSION_4_6) { cout << "OpenGL 4.6 Supported" << endl; }
	if (GLEW_VERSION_3_3) { cout << "OpenGL 3.3 Supported" << endl; }


	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	//unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glCompileShader(vertexShader);
	//// check for shader compile errors
	//int success;
	//char infoLog[512];
	//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	//if (!success)
	//{
	//	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	//}
	//// fragment shader
	//unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//glCompileShader(fragmentShader);
	//// check for shader compile errors
	//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	//if (!success)
	//{
	//	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	//}
	//// link shaders
	//unsigned int shaderProgram = glCreateProgram();
	//glAttachShader(shaderProgram, vertexShader);
	//glAttachShader(shaderProgram, fragmentShader);
	//glLinkProgram(shaderProgram);
	//// check for linking errors
	//glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	//if (!success) {
	//	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	//}
	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);

	Shader ourShader("shaders/a.vert", "shaders/a.frag");


	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);



	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw our first triangle
		//glUseProgram(shaderProgram);
		
		ourShader.use();
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;


	// start realsense!!!
	std::string serial;
	if (!device_with_streams({ RS2_STREAM_COLOR,RS2_STREAM_DEPTH }, serial))
		return EXIT_SUCCESS;


	// Create a Pipeline - this serves as a top-level API for streaming and processing frames
	rs2::pipeline pipe;
	rs2::config cfg;
	if (!serial.empty())
		cfg.enable_device(serial);
	cfg.enable_stream(RS2_STREAM_DEPTH);
	cfg.enable_stream(RS2_STREAM_COLOR);
	pipe.start(cfg);

	// Define two align objects. One will be used to align
	// to depth viewport and the other to color.
	// Creating align object is an expensive operation
	// that should not be performed in the main loop
	rs2::align align_to_depth(RS2_STREAM_DEPTH);
	rs2::align align_to_color(RS2_STREAM_COLOR);

	float       alpha = 0.5f;               // Transparancy coefficient
	direction   dir = direction::to_depth;  // Alignment direction

	

	while (true) {
        // Using the align object, we block the application until a frameset is available
        rs2::frameset frameset = pipe.wait_for_frames();

        if (dir == direction::to_depth)
        {
            // Align all frames to depth viewport
            frameset = align_to_depth.process(frameset);
        }
        else
        {
            // Align all frames to color viewport
            frameset = align_to_color.process(frameset);
        }

        // With the aligned frameset we proceed as usual
        auto depth = frameset.get_depth_frame();
        auto color = frameset.get_color_frame();

		color.get_data_size();
		color.get_data();




	}


	return 0;
}





