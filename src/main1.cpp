/* this main uses two attributes in glBufferData vertices for 0th index and color for 1st index*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shader.h"


#define GL_CHECK() do { \
    GLenum e; \
    while ((e = glGetError()) != GL_NO_ERROR) \
        fprintf(stderr, "%s:%d from (%s:%d)\n", gl_err_str(e), e, __FILE__, __LINE__); \
} while (0)

static const char *gl_err_str(GLenum e) {
    switch (e) {
        case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
        default:                               return "unknown GL error";
    }
}

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

    // load our shader
    Shader myShader("./shader/shader.vs", "./shader/shader.fs");

    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };    

    unsigned int VAO;
    unsigned int VBO;

    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    glGenVertexArrays(1, &VAO);  
    glGenBuffers(1, &VBO);

    // 2. copy our vertices array in a buffer for OpenGL to use
    // start recording
    glBindVertexArray(VAO);

    // bind to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // The position data of the triangle does not change, is used a lot, and stays the same for every render call
    // So we should use GL_STATIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. then set our vertex attributes pointers, tell gpu how to parse it
    // set the vertex attributes pointers, this is how we are interpreting the vertex data (sequancial to gpu understandable)
    //     v1   |      v2     |      v3 
    //  X  Y  Z |  X   Y   Z  |   X   Y   Z
    //  0  4  8   12  16  20     24  28  32  36
    //<stride 12>
    // offset *0
    // also we set location / index to 0 (first paramter) usually 0th element is vertex data and 1st element is color
    // thats why we set location = 0 in vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);  

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    // Enable location 1
    glEnableVertexAttribArray(1);  

    // stop recording
    glBindVertexArray(0);

    // use the created program as the frag and vertex shader
    // update only once as shaders are not changing
    myShader.use();
    float offset = 0.5f;
    bool goingUp = true;
    float prevTime = 0.0f, currTime = 0.0f;

    while(!glfwWindowShouldClose(window)) {
        // input checking
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (offset <= -0.5f) goingUp = true;
        else if (offset >= 0.5f) goingUp = false;
        currTime = glfwGetTime(); 
        if (currTime - prevTime >= 1.0f) {
            printf("currTime %f \n", currTime);
            offset = goingUp ? (offset + 0.05) : (offset - 0.05);
            prevTime = currTime;
        }
        myShader.use();
        myShader.setFloat("xFloat", offset);
        
        // one call restores everything
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        GL_CHECK();
        // errorCheck();
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
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    return 0;
}
