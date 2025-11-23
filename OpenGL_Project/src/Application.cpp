/*
Author: Sidney Land

This program handles the main thread of this OpenGL project.
*/

// Include/Using

// Must include GLEW (Extension Wrangler) before GLFW (Window/Context Manager)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// For I/O
#include <iostream>
#include <fstream>

// For strings
#include <string>

using namespace std;

// Structures

// Set of strings that hold the source code for a program object's shaders
// The number of shader sources listed determines NUM_SHADERS
struct ShaderProgramSource
{
    string vertexSource;
    string fragmentSource;
};

// Constants

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

// Filepath where shaders can be found (in a single file)
const string SHADER_FILEPATH = "res/shaders/Basic.shader";

// Number of shaders in total within the shader file (for use in ParseShader())
const int NUM_SHADERS = 2;

// Function Prototypes 

// Returns an identifier to a compiled and linked shader program using the source code progSource
// returns 0 if program is invalid, and will print a validation error message
static GLuint LinkProgram(const ShaderProgramSource &progSource);

// Creates and compiles individual shaders of shaderType using thier source code as a string
// returns 0 if shader fails to compile, and will print a compilation error message
static GLuint CompileShader(const GLuint shaderType, const string& source);

// Reads shaders (vertex and fragment) from a given filepath, and returns the source code required for a program
// parseable shader file format: #vertex [vertex shader code] #fragment [fragment shader code]
static ShaderProgramSource ParseShader(const string& filepath);

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

    // Print the current OpenGL version being used, only in Debug builds
    #ifdef _DEBUG
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    #endif
    
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

    // Creates a program using SHADER_FILEPATH's shaders
    GLuint progShader = LinkProgram(ParseShader(SHADER_FILEPATH));

    // If program is valid, use program and render
    if (progShader != 0)
    {
        glUseProgram(progShader);

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
    }

    // Delete and free the identifier of the generated glBuffer
    glDeleteBuffers(1, &buffer);

    // Delete and free the identifier for entire shader program
    // This also detaches all attached shaders (which are then deleted since they are flagged as such) 
    glDeleteProgram(progShader);

    // Terminate GLFW
    glfwTerminate();

    // Terminate Program
    return 0;
}

static GLuint LinkProgram(const ShaderProgramSource &progSource)
{
    // Create program, vertex shader, and fragment shader objects (with shaders being compiled as well)
    // Return from errors from shader compilation and return invalid identifier if so (error messages already printed)
    GLuint program = glCreateProgram();
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, progSource.vertexSource);
    if (vertexShader == 0)
    {
        return 0;
    }
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, progSource.fragmentSource);
    if (fragmentShader == 0)
    {
        return 0;
    }
    
    // Attach individual shaders to the program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link the attached shader executables into a complete program, then validate that it can be ran
    glLinkProgram(program);
    glValidateProgram(program);

    // Read GL_VALIDATE_STATUS to ensure program is valid
    int validTest;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &validTest);
    if (validTest == GL_FALSE)
    {
        // Allocate however many characers are needed to hold the error message
        // _malloca is used to dynamically allocate the least bytes possible for the message, preferably to the stack (if not, the heap instead)
        int errMsgLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errMsgLength);
        char* errMessage = (char*)_malloca(sizeof(char) * errMsgLength);

        // Put the program's info log into errMessage
        glGetProgramInfoLog(program, errMsgLength, NULL, errMessage);

        // Print error message
        printf("Failed to Validate Program\n%s", errMessage);

        // Delete and free program from GPU, then its shaders (which are automatically unattached)
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Free error message buffer, then return the reserved invalid identifier (program failed validation)
        _freea(errMessage);
        return 0;
    }

    // Flag shader identifers for deletion (but are still attached, so they still are loaded on GPU)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Detach shaders from program object, deleting them from GPU since they are flagged
    // This would free up VRAM during runtime on Release, but is disabled for Debug so we can read source code error messages 
    #ifdef NDEBUG
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    #endif

    // Return the linked and valid program 
    return program;
}

static GLuint CompileShader(const GLuint shaderType, const string &source)
{
    // Create a shader object on the GPU
    GLuint shader = glCreateShader(shaderType);

    // Convert source from C++ string to null-terminated C string
    const char* cStrSource = source.c_str();

    // Give shader source (in form of cStrSource) for its code
    glShaderSource(shader, 1, &cStrSource, NULL);

    // Compile the shader to use by the GPU
    glCompileShader(shader);

    // Read GL_COMPILE_STATUS to ensure shader successfully compiled
    int compileTest;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileTest);
    if (compileTest == GL_FALSE)
    {
        // Allocate however many characers are needed to hold the error message
        // _malloca is used to dynamically allocate the least bytes possible for the message, preferably to the stack (if not, the heap instead)
        int errMsgLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errMsgLength);
        char* errMessage = (char*)_malloca(sizeof(char) * errMsgLength);

        // Put the shader's info log into errMessage
        glGetShaderInfoLog(shader, errMsgLength, NULL, errMessage);

        // Print error message, and inform which shader type it was
        printf("Failed to Compile %s Shader\n%s", (shaderType == GL_VERTEX_SHADER) ? "Vertex" : "Fragment", errMessage);

        // Delete and free shader from GPU (immediately, since it isn't attached yet)
        glDeleteShader(shader);

        // Free error message buffer, then return the reserved invalid identifier (shader failed compilation)
        _freea(errMessage);
        return 0;
    }

    // Return a successfully compiled shader object
    return shader;
}

static ShaderProgramSource ParseShader(const string &filepath)
{
    // Stream to read shader file input
    ifstream fileStream(filepath);

    // Determines what shader we are currently reading
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    // Current line in file
    string currLine;

    // Array of strings, each element holding a shader's source code
    string shaderSourceString[NUM_SHADERS];

    // Parse shader's vertex and fragment portions
    ShaderType type = ShaderType::NONE;
    while (getline(fileStream, currLine))
    {
        // Use #vertex and #fragment lines (in that order) to denote where these shaders are stored
        // (short circuits using current shader type to prevent redundant find() operations)
        if (type == ShaderType::NONE && currLine.find("#vertex") != string::npos)
        {
            type = ShaderType::VERTEX;
        }
        else if (type == ShaderType::VERTEX && currLine.find("#fragment") != string::npos)
        {
            type = ShaderType::FRAGMENT;
        }
        // If shader type hasn't changed, add currLine to appropriate shaderSourceString, based on shader type
        else
        {
            shaderSourceString[(int)type].append(currLine + '\n');
        }
    }

    return { shaderSourceString[0], shaderSourceString[1] };
}

