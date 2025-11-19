/*
Author: Sidney Land

This program handles the main thread of this OpenGL project.
*/

// Must include GLEW (GL) before GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// For Debug I/O
#include <iostream>

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
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
    glfwSwapInterval(1);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Clears color buffer for next frame
        glClear(GL_COLOR_BUFFER_BIT);

        // Draws a white triangle with Legacy OpenGL
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        // Swap front and back buffers, refreshing display
        glfwSwapBuffers(window);

        // Poll for and process events, like closing the window
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();

    // Terminate Program
    return 0;
}