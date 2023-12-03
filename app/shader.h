#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions>
#include <string>

class Shader : protected QOpenGLFunctions
{
public:
    using Ptr = std::shared_ptr<Shader>;

    Shader(const std::string& vertexShaderFilename,
           const std::string& fragmentShaderFilename);
    Shader(Shader&&) = delete;
    Shader(const Shader&) = delete;
    ~Shader();

    void use();

    GLint location(const std::string& name);

    void setUniformFloat(const std::string& name, float value);
    void setUniformInt(const std::string& name, int value);
    void setUniformVec3(const std::string& name, const float* data);
    void setUniformVec4(const std::string& name, const float* data);
    void setUniformMat4(const std::string& name, const float* data);

private:
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_shaderProgram;
};

#endif // SHADER_H
