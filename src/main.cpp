#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define GL_CHECK() do { \
    GLenum e; \
    while ((e = glGetError()) != GL_NO_ERROR) \
        fprintf(stderr, "GL error 0x%x at %s:%d\n", e, __FILE__, __LINE__); \
} while (0)

// So if we want to send data from one shader to the other we'd have to declare an output in the sending 
// shader and a similar input in the receiving shader. When the types and the names are equal on both 
// sides OpenGL will link those variables together
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    // "out vec4 vertexColor; \n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    // "   vertexColor = vec4(0.5f, 0.5f, 0.0f, 1.0f);"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    // "in vec4 vertexColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
        "FragColor = ourColor;\n"
    "}";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // std::cout << "resize called to: (w) " << width << "And: (h) " << height << std::endl;
    glViewport(0, 0, width, height);
}

int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //  tell GLFW we want to explicitly use the core-profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Should we support higher version of GLFW with opengl ?
    // Mac caps the opneGL version to 4.2
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Handle Retina High DPI Displays
    int frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load glad" << std::endl;
        return -1;
    }

    // We have to tell OpenGL the size of the rendering window so OpenGL knows how we 
    // want to display the data and coordinates with respect to the window. 
    // Set the initial viewport using the Retina pixel count
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    unsigned int vertexShader, fragmentShader, shaderProgram;
    int  success;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };  

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    glGenVertexArrays(1, &VAO);  
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 2. copy our vertices array in a buffer for OpenGL to use
    // start recording
    glBindVertexArray(VAO);

    // bind to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // The position data of the triangle does not change, is used a lot, and stays the same for every render call
    // So we should use GL_STATIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Now use elemental array to save indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 3. then set our vertex attributes pointers, tell gpu how to parse it
    // set the vertex attributes pointers, this is how we are interpreting the vertex data (sequancial to gpu understandable)
    //     v1   |      v2     |      v3 
    //  X  Y  Z |  X   Y   Z  |   X   Y   Z
    //  0  4  8   12  16  20     24  28  32  36
    //<stride 12>
    // offset *0
    // also we set location / index to 0 (first paramter) usually 0th element is vertex data and 1st element is color
    // thats why we set location = 0 in vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    // stop recording
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window)) {
        // input checking
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // trying out uniform
        float timeValue = glfwGetTime();
        printf("timeValue %f , sine of time %f value of equation %f \n", timeValue, sin(timeValue), ((sin(timeValue) / 2.0f) + 0.5f));
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // get location of uniform variable
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // use the created program as the frag and vertex shader
        // need to update the shader program before we chagne the color, because it sets the uniform on the currently active shader program.
        glUseProgram(shaderProgram);
        // use location to fill the data, v0, v1, v2, v3
        // Because OpenGL is in its core a C library it does not have native support for function overloading, 
        // so wherever a function can be called with different types OpenGL defines new functions for each type required
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // one call restores everything
        glBindVertexArray(VAO);
        // open gl knows we binded indices with EBO, but how does it know how many elements we need to use
        // trigagle uses 3 indexes, line uses 2 and total number of elements (here 0th element is vertex so total binded vertex is 6)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);

        GL_CHECK();
        /**
         * Double buffer When an application draws in a single buffer the resulting image may display flickering issues. 
         * This is because the resulting output image is not drawn in an instant, but drawn pixel by pixel and usually 
         * from left to right and top to bottom. Because this image is not displayed at an instant to the user while 
         * still being rendered to, the result may contain artifacts. To circumvent these issues, windowing applications 
         * apply a double buffer for rendering. The front buffer contains the final output image that is shown at the 
         * screen, while all the rendering commands draw to the back buffer. As soon as all the rendering commands are 
         * finished we swap the back buffer to the front buffer so the image can be displayed without still being 
         * rendered to, removing all the aforementioned artifacts.
         */

        // swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) 
        // that is used to render to during this render iteration and show it as output to the screen.
        glfwSwapBuffers(window);
        // checks if any events are triggered (like keyboard input or mouse movement events), 
        // updates the window state, and calls the corresponding functions (which we can register via callback methods).
        glfwPollEvents();
    }


    std::cout << "hello world" << std::endl;
    glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}
