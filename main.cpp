#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the GLFW library */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1000, 1000, "TEST WINDOW", NULL, NULL); //width, height, title, monitor, shared context
    if (!window)
    {
        glfwTerminate();
		std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // TODO not sure where this goes.

	/* Initialize GLAD */ 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // To check version of OpenGL that the context created. (current 4.6)
    // std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CREATE TRIANGLE

    //error variables
    int  success;
    char infoLog[512];

    //////////////////////
    /// Set up shaders
	//////////////////////

    // Create a vertex shader   
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
        // Check for compilation errors for vertex shader
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //querying the shader about a given parameter (compile status here)
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create a fragment shader
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
        // Check for compilation errors for fragment Shader
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //querying the shader about a given parameter (compile status here)
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Linking: Create a shader program to link shaders together (output of one becomes input of the next)
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
        // Check for program linking errors:
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

	// Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //////////////////////
    /// Set up vertex data and buffers and configure vertex attributes
    //////////////////////

    float vertices[] = {
    -0.5f, -0.5f, 0.0f, // left
     0.5f, -0.5f, 0.0f, // right
     0.0f,  0.5f, 0.0f  // top
    };

    // Create a vertex array object (VAO) to store the vertex attributes
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create a vertex buffer object (VBO) and copy the vertex data to it
    GLuint VBO;
    glGenBuffers(1, &VBO); // num of buffers, var to store buffer id GLUint
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // GL_ARRAY_BUFFER is the vertex buffer object. any buffer calls to GL_ARRAY_BUFFER will go to our VBO object
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // target buffer object, size in bytes of new data for buffer object, pointer to data or NULL, 

    // Tell GPU how to interpret the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // index of the vertex attribute, size of the vertex attribute, type of the vertex attribute, normalized?, stride (byte offset between consecutive vertex attributes), pointer to the first component of the first attribute in the buffer
    glEnableVertexAttribArray(0); // enable the vertex attribute. parameter is the index location of the vertex attribute

	// Unbind the VBO and VAO (not necessary, but good practice)
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the VBO
	glBindVertexArray(0); // unbind the VAO

    // Set to wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Input */
        processInput(window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw Triangle */
		glUseProgram(shaderProgram); // Use the shader program we created earlier
        glBindVertexArray(VAO); // Bind the VAO so that the vertex attributes are set up correctly
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the triangle using the vertex data in the VBO. GL_TRIANGLES is the primitive type, 0 is the starting index in the VBO, 3 is the number of vertices to draw



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}