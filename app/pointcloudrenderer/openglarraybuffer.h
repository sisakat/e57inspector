#ifndef OPENGLARRAYBUFFER_H
#define OPENGLARRAYBUFFER_H

#include <QOpenGLFunctions>

class OpenGLArrayBuffer : protected QOpenGLFunctions
{
public:
    typedef std::shared_ptr<OpenGLArrayBuffer> Ptr;
    typedef std::weak_ptr<OpenGLArrayBuffer> WPtr;

    OpenGLArrayBuffer(
        const void* data,
        const GLenum componentType,
        const GLint componentCount,
        const GLsizei elementCount,
        const GLenum usage,
        const GLenum type = GL_ARRAY_BUFFER
        );

    virtual ~OpenGLArrayBuffer(void);

    GLenum componentType(void) const { return _componentType; }
    GLsizei componentByteSize(void) const { return _componentByteSize; }
    GLint componentCount(void) const { return _componentCount; }
    GLsizei elementCount(void) const { return _elementCount; }
    GLsizei byteSize(void) const { return _byteSize; }
    GLuint buffer(void) const { return _buffer; }

private:
    GLenum _componentType;
    GLsizei _componentByteSize;
    GLint _componentCount;
    GLsizei _elementCount;
    GLsizei _byteSize;
    GLuint _buffer;
};

#endif // OPENGLARRAYBUFFER_H
