#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <print>
#include "WFC.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool paused;
bool spacePressedLastFrame = false;

const char* imagePaths[5] = {
    "trackTiles/blank.png",
    "trackTiles/down.png",
    "trackTiles/left.png",
    "trackTiles/right.png",
    "trackTiles/up.png"
};

std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool spacePressedThisFrame = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (spacePressedThisFrame && !spacePressedLastFrame) {
        paused = !paused;  // Toggle only on "key down"
    }

    spacePressedLastFrame = spacePressedThisFrame; // Update for next frame
}

int initGLFW(GLFWwindow** window) {
    /* Initialize the GLFW library */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Disables functionality that is deprecated in older OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    *window = glfwCreateWindow(1000, 1000, "TEST WINDOW", NULL, NULL); //width, height, title, monitor, shared context
    if (!window)
    {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback); // TODO not sure where this goes.

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    return 0;
}

void setupShaders(GLuint& shaderProgram) {
    std::string vertexShaderStr = loadShaderSource("vertex.glsl");
    std::string fragmentShaderStr = loadShaderSource("fragment.glsl");
    const char* vertexShaderSource = vertexShaderStr.c_str();
    const char* fragmentShaderSource = fragmentShaderStr.c_str();

    // Create a vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for compilation errors for vertex shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create a fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for compilation errors for fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Linking: Create a shader program to link shaders together
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

int main(void)
{
    GLFWwindow* window;
    initGLFW(&window);

    // To check version of OpenGL that the context created. (current 4.6)
    // std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Set up shaders
    GLuint shaderProgram;
    setupShaders(shaderProgram);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* Load and create a texture */
    //GLuint texture;
    GLuint tileTextures[5]; // number of textures to generate, pointer to the texture ID   
    glGenTextures(5, tileTextures);
    for (int i = 0; i < 5; i++) {
        glBindTexture(GL_TEXTURE_2D, tileTextures[i]); // Bind the texture object to the target GL_TEXTURE_2D to do other stuff with
        
        // Set texture filtering/wrapping options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image data
        int width, height, nrChannels;
        unsigned char* data = stbi_load(imagePaths[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0); // use texture unit 0

    //////////////////////
    /// Set up vertex data and buffers and configure vertex attributes
    //////////////////////

    float vertices[] = { //along with UVs
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Create a vertex array object (VAO) to store the vertex attributes
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO); // num of buffers, var to store buffer id GLUint
    glGenBuffers(1, &VBO); // num of buffers, var to store buffer id GLUint
    glBindVertexArray(VAO);

    // Create a vertex buffer object (VBO) and element array buffer object (EBO) and copy the vertex data to it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // GL_ARRAY_BUFFER is the vertex buffer object. any buffer calls to GL_ARRAY_BUFFER will go to our VBO object
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // target buffer object, size in bytes of new data for buffer object, pointer to data or NULL, 
	
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // Tell GPU how to interpret the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // index of the vertex attribute, size of the vertex attribute, type of the vertex attribute, normalized?, stride (byte offset between consecutive vertex attributes), pointer to the first component of the first attribute in the buffer
    glEnableVertexAttribArray(0); // enable the vertex attribute. parameter is the index location of the vertex attribute
    // UVs are in location 1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	// Unbind the VBO and VAO (not necessary, but good practice)
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the VBO
	glBindVertexArray(0); // unbind the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //Don't unbind the EBO before VAO because it's stored in the VAO 

    // Set to wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Input */
        processInput(window);

        /* Poll for and process events */
        glfwPollEvents();

        //Paused
        if (paused == true) {
            continue;
        }

        /* Render here */
        glClearColor(0.301f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw Triangle */
		//glUseProgram(shaderProgram); // Use the shader program we created earlier
  //      glBindVertexArray(VAO); // Bind the VAO so that the vertex attributes are set up correctly
  //      //glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the triangle using the vertex data in the VBO. GL_TRIANGLES is the primitive type, 0 is the starting index in the VBO, 3 is the number of vertices to draw
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw the triangle using the vertex data in the EBO. GL_TRIANGLES is the primitive type, 6 is the number of indices to draw, GL_UNSIGNED_INT is the type of the indices, 0 is the offset in the EBO

        glm::mat4 projection = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f, -1.0f, 1.0f);
        int gridWidth = 5;
        int gridHeight = 5;
        int tileSize = 160;

        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                int tileIndex = rand() % 5; // pick a texture index randomly

                // Create transform matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(100, 100, 0));
                model = glm::translate(model, glm::vec3(x * tileSize * 1.05, y * tileSize *1.05, 0.0f));
                model = glm::scale(model, glm::vec3(tileSize, tileSize, 1.0f));
                glm::mat4 transform = projection * model;

                // Pass transform to shader
                GLuint transformLoc = glGetUniformLocation(shaderProgram, "uTransform");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

                // Bind texture and draw
                glBindTexture(GL_TEXTURE_2D, tileTextures[tileIndex]);
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }


        glUseProgram(shaderProgram);
        ////glBindTexture(GL_TEXTURE_2D, texture);
        //glBindTexture(GL_TEXTURE_2D, tileTextures[2]);
        //glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

//int main(void) {
//    //WFC();
//    main2();
//    // read level from text
//}