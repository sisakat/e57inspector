#include "shader.h"

#include <QFile>
#include <fstream>
#include <sstream>

std::string readFileToString(const std::string& filename)
{
    std::ifstream file(filename, std::ios::in);
    if (file.is_open() && file.good())
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::stringstream err;
    err << "Could not open file '" << filename << "'.";
    throw std::runtime_error(err.str());
}

std::string readResourceToString(const std::string& filename)
{
    QFile file(filename.c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
        std::stringstream err;
        err << "Could not open file '" << filename << "'.";
        throw std::runtime_error(err.str());
    }
    return file.readAll().toStdString();
}

Shader::Shader(const std::string& vertexShaderFilename,
               const std::string& fragmentShaderFilename)
{
    initializeOpenGLFunctions();

    const char* source;
    int length;
    GLint status;

    auto vertexShaderCode = readResourceToString(vertexShaderFilename);
    auto fragmentShaderCode = readResourceToString(fragmentShaderFilename);

    source = vertexShaderCode.c_str();
    length = vertexShaderCode.size();
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &source, &length);
    glCompileShader(m_vertexShader);
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        throw std::runtime_error("Could not compile vertex shader.");
    }

    source = fragmentShaderCode.c_str();
    length = fragmentShaderCode.size();
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &source, &length);
    glCompileShader(m_fragmentShader);
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        throw std::runtime_error("Could not compile fragment shader.");
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);

    glLinkProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        throw std::runtime_error("Could not link shaders.");
    }
}

Shader::~Shader()
{
    glDetachShader(m_shaderProgram, m_vertexShader);
    glDetachShader(m_shaderProgram, m_fragmentShader);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    glDeleteProgram(m_shaderProgram);
}

void Shader::use() { glUseProgram(m_shaderProgram); }

void Shader::release() { glUseProgram(0); }

GLint Shader::location(const std::string& name)
{
    auto location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0)
    {
        throw std::runtime_error(
            "Could not get location of uniform variable: " + name);
    }
    return location;
}

void Shader::setUniformFloat(const std::string& name, float value)
{
    glUniform1f(location(name), value);
}

void Shader::setUniformInt(const std::string& name, int value)
{
    glUniform1i(location(name), value);
}

void Shader::setUniformVec3(const std::string& name, const float* data)
{
    glUniform3fv(location(name), 1, data);
}

void Shader::setUniformVec4(const std::string& name, const float* data)
{
    glUniform4fv(location(name), 1, data);
}

void Shader::setUniformMat4(const std::string& name, const float* data)
{
    glUniformMatrix4fv(location(name), 1, GL_FALSE, data);
}
