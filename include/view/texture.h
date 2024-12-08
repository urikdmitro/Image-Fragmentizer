#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "opengl_context.h"
#include "model/image.h"

class Texture
{
private:
    static GLuint GenerateOpenGLTexture(
        const void* data,
        GLsizei x,
        GLsizei y,
        int channels_number
    );
    static GLuint DuplicateOpenGLTexture(GLuint id, GLsizei x, GLsizei y);

    GLuint id;
    GLsizei x;
    GLsizei y;

public:
    Texture();
    Texture(const Image &image);

    ~Texture();

    Texture(Texture &&texture);
    Texture(const Texture &texture);

    Texture& operator=(Texture &&texture);
    Texture& operator=(const Texture &texture);

    void LoadFromImage(const Image &image);

    GLuint GetOpenGLId() const;
    GLsizei GetWidth() const;
    GLsizei GetHeight() const;
};

#endif // TEXTURE_H_