#include "texture.h"

GLuint Texture::GetOpenGLId()
{
    return id;
}

GLsizei Texture::GetWidth()
{
    return x;
}

GLsizei Texture::GetHeight()
{
    return y;
}

void Texture::LoadFromImage(const Image &image)
{
    if(id != 0)
    {
        glDeleteTextures(1, &id);
    }

    id = GenerateOpenGLTexture(
        image.raw_data,
        static_cast<GLsizei>(image.width),
        static_cast<GLsizei>(image.height),
        image.channels
    );

    x = static_cast<GLsizei>(image.width);
    y = static_cast<GLsizei>(image.height);
}

Texture::Texture()
    : id(0)
    , x(0)
    , y(0)
{ }

Texture::Texture(const Image &image)
    : id(GenerateOpenGLTexture(
            image.raw_data,
            static_cast<GLsizei>(image.width),
            static_cast<GLsizei>(image.height),
            image.channels
        )
    )
    , x(static_cast<GLsizei>(image.width))
    , y(static_cast<GLsizei>(image.height))
{ }


Texture::Texture(Texture &&texture)
    : id(texture.id)
    , x(texture.x)
    , y(texture.y)
{
    texture.id = 0;
    texture.x = 0;
    texture.y = 0;
}

Texture::Texture(const Texture &texture)
    : id(DuplicateOpenGLTexture(texture.id, texture.x, texture.y))
    , x(texture.x)
    , y(texture.y)
{ }

Texture &Texture::operator=(Texture &&texture)
{
    if(this != &texture)
    {
        if (id != 0)
        {
            glDeleteTextures(1, &id);
        }

        this->id = texture.id;
        this->x = texture.x;
        this->y = texture.y;

        texture.id = 0;
        texture.x = 0;
        texture.y = 0;
    }

    return *this;
}

Texture &Texture::operator=(const Texture &texture)
{
    if(this != &texture)
    {
        if (id != 0)
        {
            glDeleteTextures(1, &id);
        }

        x = texture.x;
        y = texture.y;
        id = DuplicateOpenGLTexture(texture.id, x, y);
    }
    return *this;
}

Texture::~Texture()
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);
    }
}

GLuint Texture::GenerateOpenGLTexture(
    const void* data,
    GLsizei x,
    GLsizei y,
    int channels_number
)
    {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int channel_format = channels_number == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        channel_format,
        x,
        y,
        0,
        channel_format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    return id;
}

GLuint Texture::DuplicateOpenGLTexture(GLuint id, GLsizei x, GLsizei y)
{
    GLuint duplicated_texture_id;
    glGenTextures(1, &duplicated_texture_id);
    glBindTexture(GL_TEXTURE_2D, duplicated_texture_id);

    glCopyImageSubData(
        id,
        GL_TEXTURE_2D,
        0, 0, 0, 0,
        duplicated_texture_id,
        GL_TEXTURE_2D,
        0, 0, 0, 0,
        x,
        y,
        1
    );

    return duplicated_texture_id;
}
