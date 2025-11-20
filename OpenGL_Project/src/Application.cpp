/*
Author: Sidney Land

This program handles the main thread of this OpenGL project.
*/

// Must include GLEW (GL) before GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// For Debug I/O
#include <iostream>

// Verticies will be positioned in POS_DIMENSION-D space (for example, 2-D space)
const int POS_DIMENSION = 2;

// Number of verticies in the triangle
const int NUM_VERTICIES = 3;

// Triangle vertex positions (2D) to send to GPU
const float VERTEX_POS[POS_DIMENSION * NUM_VERTICIES] =
{
    -0.5f, -0.5f,
    0.0f, 0.5f,
    0.5f, -0.5f
};

// Index to access the position attributes of a vertex for use in shaders and OpenGL functions
const int POS_INDEX = 0;

// Width and Height of the display window in screen coordinates
const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

// Number of frames to generate before display buffers are swapped (0 -> no vsync, >0 -> vsync)
const int SWAP_INTERVAL = 1;

int main(void)
{
    // Declare a GLFW window
    GLFWwindow* window;

    // Initialize the GLFW window/context manager
    if (glfwInit() != GLFW_TRUE)
    {
        printf("GLFW Init Error");
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW extension manager (must be done after a context is made current)
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW Init Error");
        return -1;
    }

    // Print the current OpenGL version being used
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    
    // Only update display buffer (by swapping the double buffers) after 1 frame has been fully generated (enables vsync)
    glfwSwapInterval(SWAP_INTERVAL);

    // Create 1 glBuffer, where buffer is literally the unsigned int identifier used to refer to the buffer created in GPU VRAM
    GLuint buffer;
    glGenBuffers(1, &buffer);

    // Inform GPU that buffer will hold an array of vertex attributes, and 
    // bind buffer to the GL_ARRAY_BUFFER target (important for many functions afterward)
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // Write the array VERTEX_POS in the desired vertex attribute format, and
    // optimize usage for many reads after few modifications (STATIC) and being accessed to draw graphics (DRAW)
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POS), VERTEX_POS, GL_STATIC_DRAW);

    // Describe in greater detail to GPU the position attribites of the verticies:
    // that positions can be accessed at vertex index 0, that each vertex has POS_DIMENSION position attributes, each attribute is a float, 
    // that these values shouldn't be normalized, the size of a vertex in bytes, and that positions are the first attributes (no offset)
    glVertexAttribPointer(POS_INDEX, POS_DIMENSION, GL_FLOAT, GL_FALSE, sizeof(float) * POS_DIMENSION, 0);

    // Enable vertex position attributes (which are refered to by POS_INDEX)
    glEnableVertexAttribArray(POS_INDEX);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Clears color buffer for next frame
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle using verticies 0 to NUM_VERTICIES (0, 1, and 2) in the bound buffer
        // This will properly draw a white triangle if a default shader is provided by your GPU
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICIES);

        // Swap front and back buffers, refreshing display
        glfwSwapBuffers(window);

        // Poll for and process events, like closing the window
        glfwPollEvents();
    }

    // Delete and free the identifiers of the generated glBuffers (currently just buffer)
    glDeleteBuffers(1, &buffer);

    // Terminate GLFW
    glfwTerminate();

    // Terminate Program
    return 0;
}