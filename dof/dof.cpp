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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

enum class direction
{
	to_depth,
	to_color
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
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


	int WIDTH = 848;
	int HEIGHT = 480;


	// ====================== start GL =============================

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);

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






	// =================== start realsense!!! ==================================
	std::string serial;
	if (!device_with_streams({ RS2_STREAM_COLOR,RS2_STREAM_DEPTH }, serial))
		return EXIT_SUCCESS;

	// Create a Pipeline - this serves as a top-level API for streaming and processing frames
	rs2::pipeline pipe;
	rs2::config cfg;
	if (!serial.empty())
		cfg.enable_device(serial);
	cfg.enable_stream(RS2_STREAM_DEPTH, 848, 480, RS2_FORMAT_Z16, 15);
	//cfg.enable_stream(RS2_STREAM_COLOR);
	cfg.enable_stream(RS2_STREAM_COLOR, 848, 480, RS2_FORMAT_RGB8, 15);
	pipe.start(cfg);

	// Define two align objects. One will be used to align
	// to depth viewport and the other to color.
	// Creating align object is an expensive operation
	// that should not be performed in the main loop
	rs2::align align_to_depth(RS2_STREAM_DEPTH);
	rs2::align align_to_color(RS2_STREAM_COLOR);

	float       alpha = 0.5f;               // Transparancy coefficient
	direction   dir = direction::to_color;  // Alignment direction

	texture depth_image, color_image;     // Helpers for rendering images
	rs2::colorizer c = rs2::colorizer(3);   // Helper to colorize depth images. mode 3 means black-to-white

	rs2::decimation_filter dec_filter;
	dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
	rs2::hole_filling_filter hole_filter;
	hole_filter.set_option(RS2_OPTION_HOLES_FILL, 1);




	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Shader ourShader("shaders/a.vert", "shaders/a.frag");
	Shader camShader("shaders/b.vert", "shaders/b.frag");
	Shader modShader("shaders/c.vert", "shaders/c.frag");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-0.8f,  0.8f,  0.0f, 0.0f,
		-0.8f, -0.8f,  0.0f, 1.0f,
		 0.8f, -0.8f,  1.0f, 1.0f,
		-0.8f,  0.8f,  0.0f, 0.0f,
		 0.8f, -0.8f,  1.0f, 1.0f,
		 0.8f,  0.8f,  1.0f, 0.0f
	};
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	// --------------------
	//camShader.use();
	//camShader.setInt("screenTexture", 0);

	modShader.use();
	// set uniforms
	modShader.setInt("colorTex", 0);
	modShader.setInt("depthTex", 1);


	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------

		// first pass
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

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
		depth = dec_filter.process(depth);
		depth = hole_filter.process(depth);

		auto color = frameset.get_color_frame();
		auto colorized_depth = c.colorize(depth); //we need to colorize depth before we can render it???

		
		color_image.upload(color);
		depth_image.upload(colorized_depth);

		// Bind first texture 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, color_image.get_gl_handle());

		// Bind the second texture 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depth_image.get_gl_handle());
		

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Unbind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("\nCleaning up...\n");

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	glfwTerminate();


	return 0;
	 
}





