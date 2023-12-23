#include "ShaderFactory.h"

ShaderFactory& ShaderFactory::instance()
{
    static ShaderFactory shaderFactory;
    return shaderFactory;
}

Shader::Ptr
ShaderFactory::createShader(const std::string& vertexShaderFilename,
                            const std::string& fragmentShaderFilename)
{
    return instance().createShaderImpl(vertexShaderFilename,
                                       fragmentShaderFilename);
}

Shader::Ptr
ShaderFactory::createShaderImpl(const std::string& vertexShaderFilename,
                                const std::string& fragmentShaderFilename)
{
    std::string id = vertexShaderFilename + fragmentShaderFilename;
    if (m_shaders.contains(id))
    {
        return m_shaders.at(id);
    }
    else
    {
        m_shaders.emplace(id, std::make_shared<Shader>(vertexShaderFilename,
                                                       fragmentShaderFilename));
        return m_shaders.at(id);
    }
}
