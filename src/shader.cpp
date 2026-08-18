#include "shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exception
    vShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contets into stream
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        //Close the file handle
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_READ::"<<std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertexShader, fragmentShader;
    int  success;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << infoLog << std::endl;
    }

    // Save it to local var
    this->shaderProgram = glCreateProgram();
    glAttachShader(this->shaderProgram, vertexShader);
    glAttachShader(this->shaderProgram, fragmentShader);
    glLinkProgram(this->shaderProgram);
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(this->shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  
}

Shader::~Shader() {
    glDeleteShader(this->shaderProgram);
}

void Shader::use(){
    glUseProgram(this->shaderProgram);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(this->shaderProgram, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(this->shaderProgram, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const{
    glUniform1f(glGetUniformLocation(this->shaderProgram, name.c_str()), value);
}