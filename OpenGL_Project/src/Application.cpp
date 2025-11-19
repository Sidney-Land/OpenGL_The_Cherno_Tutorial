// Must include GLEW (GL) before GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// For Debug I/O
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    // Initialize the GLFW window/context manager
    if (glfwInit() != GLFW_TRUE)
    {
        printf("GLFW Init Error");
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Initialize GLEW extension manager (must be done after a context is made current)
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW Init Error");
        return -1;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    
    // Only update display buffer (by swapping the double buffers) after 1 frame has been fully generated (enables vsync)
    glfwSwapInterval(1);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Draws a white triangle with Legacy OpenGL
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}