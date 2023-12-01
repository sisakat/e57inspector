#include "openglarraybuffer.h"

OpenGLArrayBuffer::OpenGLArrayBuffer(
    const void* data,
    const GLenum componentType,
    const GLint componentCount,
    const GLsizei elementCount,
    const GLenum usage,
    const GLenum type)
        : _componentType(componentType),
          _componentCount(componentCount),
          _elementCount(elementCount)
{
    initializeOpenGLFunctions();

    switch (_componentType) {
    case GL_BYTE:
        _componentByteSize = sizeof(GLbyte); break;
    case GL_UNSIGNED_BYTE:
        _componentByteSize = sizeof(GLubyte); break;
    case GL_SHORT:
        _componentByteSize = sizeof(GLshort); break;
    case GL_UNSIGNED_SHORT:
        _componentByteSize = sizeof(GLushort); break;
    case GL_INT:
        _componentByteSize = sizeof(GLint); break;
    case GL_UNSIGNED_INT:
        _componentByteSize = sizeof(GLuint); break;
    case GL_FLOAT:
        _componentByteSize = sizeof(GLfloat); break;
    case GL_DOUBLE:
        _componentByteSize = sizeof(GLdouble); break;
    default:
        throw std::runtime_error("Invalid enumerator for componentType."); break;
    }

    _byteSize = _elementCount * _componentCount * _componentByteSize;

    _buffer = 0;
    glGenBuffers(1, &_buffer);
    glBindBuffer(type, _buffer);
    glBufferData(type, _byteSize, data, usage);
}

OpenGLArrayBuffer::~OpenGLArrayBuffer()
{
    if (_buffer != 0)
    {
        glDeleteBuffers(1, &_buffer);
        _buffer = 0;
    }
}
