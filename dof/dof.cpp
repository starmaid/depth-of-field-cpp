// dof.cpp : Defines the entry point for the application.
//


#include "dof.h"
#include <opencv2/core.hpp>
#include <librealsense2/rs.hpp>
#include <GL/glew.h>
//#include <GL/glut.h>
#include <GLFW/glfw3.h>

//#include <gst/gst.h>
//#include <gst/rtsp-server/rtsp-server.h>

#include "example-utils.hpp"
#include "example.hpp"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

// settings
const unsigned int SCR_WIDTH = 848;
const unsigned int SCR_HEIGHT = 480;

float shader_focalplanedistance = 0.4f;
float shader_focusrange = 0.3f;

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
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		shader_focalplanedistance += 0.05f;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		shader_focalplanedistance -= 0.05f;
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		shader_focusrange += 0.05f;
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		shader_focusrange -= 0.05f;
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


	// ====================== start GL =============================

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

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
	cfg.enable_stream(RS2_STREAM_COLOR, 0, 848, 480, RS2_FORMAT_RGB8, 15);
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
	rs2::colorizer c = rs2::colorizer(3);   // Helper to colorize depth images. 2 - WhiteToBlack 3 - BlackToWhite

	rs2::decimation_filter dec_filter;
	dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
	rs2::hole_filling_filter hole_filter;
	hole_filter.set_option(RS2_OPTION_HOLES_FILL, 1);




	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-0.8f,  0.8f,  0.0f, 1.0f,
		-0.8f, -0.8f,  0.0f, 0.0f,
		 0.8f, -0.8f,  1.0f, 0.0f,
		-0.8f,  0.8f,  0.0f, 1.0f,
		 0.8f, -0.8f,  1.0f, 0.0f,
		 0.8f,  0.8f,  1.0f, 1.0f
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



	float quadVerticesFull[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
	// screen quad VAO
	unsigned int quadVAOFull, quadVBOFull;
	glGenVertexArrays(1, &quadVAOFull);
	glGenBuffers(1, &quadVBOFull);
	glBindVertexArray(quadVAOFull);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBOFull);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticesFull), &quadVerticesFull, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	float quadVerticesFullFlipped[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 1.0f,

		-1.0f,  1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 0.0f
	};
	// screen quad VAO
	unsigned int quadVAOFullFlipped, quadVBOFullFlipped;
	glGenVertexArrays(1, &quadVAOFullFlipped);
	glGenBuffers(1, &quadVBOFullFlipped);
	glBindVertexArray(quadVAOFullFlipped);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBOFullFlipped);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticesFullFlipped), &quadVerticesFullFlipped, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	// --------------------
	// 	//Shader ourShader("shaders/a.vert", "shaders/a.frag");
	Shader camShader("shaders/b.vert", "shaders/b.frag");
	camShader.use();
	camShader.setInt("screenTexture", 4);


	Shader cocShader("shaders/postp.vert", "shaders/coc.frag");
	cocShader.use();
	// set uniforms
	cocShader.setInt("colorTex", 0);
	cocShader.setInt("depthTex", 1);
	//cocShader.setFloat("_FocalPlaneDistance", 0.4f);
	//cocShader.setFloat("_FocusRange", 0.3f);
	cocShader.setFloat("_FocalPlaneDistance", shader_focalplanedistance);
	cocShader.setFloat("_FocusRange", shader_focusrange);
	



	Shader blurShader("shaders/postp.vert", "shaders/blur.frag");
	blurShader.use();
	blurShader.setInt("colorTex", 0);
	blurShader.setInt("cocTex", 3);



	// framebuffer configuration
	// -------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// SECOND framebuffer configuration
	// -------------------------
	unsigned int framebuffer2;
	glGenFramebuffers(1, &framebuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
	// create a color attachment texture
	unsigned int textureColorbuffer2;
	glGenTextures(1, &textureColorbuffer2);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer2, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo2;
	glGenRenderbuffers(1, &rbo2);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo2); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		

		// render
		// ------

		// first pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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



		// render
		// ------
		// 
		// set the first shader
		cocShader.use();
		cocShader.setFloat("_FocalPlaneDistance", shader_focalplanedistance);
		cocShader.setFloat("_FocusRange", shader_focusrange);

		printf("plane: %f, range: %f\n", shader_focalplanedistance, shader_focusrange);
		// bind to framebuffer and draw scene as we normally would to color texture 
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
		// make sure we clear the framebuffer's content
		glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind first texture 
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, color_image.get_gl_handle());

		// Bind the second texture 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depth_image.get_gl_handle());

		
		glBindVertexArray(quadVAOFull);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		
		blurShader.use();
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

		// bind to a second framebuffer to store the output
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.5f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		glBindVertexArray(quadVAOFull);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		camShader.use();
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		// Bind the current tris

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer2);


		// flip everything at the very end
		glBindVertexArray(quadVAOFullFlipped);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		
		// Unbind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("\nCleaning up...\n");

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteFramebuffers(1, &framebuffer2);

	glfwTerminate();


	return 0;
	 
}





