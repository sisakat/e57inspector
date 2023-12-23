#ifndef E57INSPECTOR_SHADERFACTORY_H
#define E57INSPECTOR_SHADERFACTORY_H

#include "shader.h"

#include <string>
#include <unordered_map>

class ShaderFactory
{
public:
    static ShaderFactory& instance();

    static Shader::Ptr createShader(const std::string& vertexShaderFilename,
                                    const std::string& fragmentShaderFilename);

private:
    ShaderFactory() = default;
    ~ShaderFactory() = default;
    std::unordered_map<std::string, Shader::Ptr> m_shaders;

    Shader::Ptr createShaderImpl(const std::string& vertexShaderFilename,
                                 const std::string& fragmentShaderFilename);
};

#endif // E57INSPECTOR_SHADERFACTORY_H
