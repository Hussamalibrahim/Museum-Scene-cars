#include "3DTexture.h"
#include "stb_image.h"
#include <stdio.h>

GLTexture::GLTexture()
{
    texID = 0;
    width = height = 0;
}

GLTexture::~GLTexture()
{
    if (texID)
        glDeleteTextures(1, &texID);
}

void GLTexture::Use()
{
    glBindTexture(GL_TEXTURE_2D, texID);
}

void GLTexture::BuildColorTexture(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char color[3] = { r, g, b };

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, color);

    width = height = 1;
}

void GLTexture::Load(const char* filename)
{
    int w, h, ch;
    unsigned char* data = stbi_load(filename, &w, &h, &ch, 3);

    if (!data)
    {
        printf("Failed to load texture: %s\n", filename);
        return;
    }

    width = w;
    height = h;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,
                      width, height,
                      GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}
void GLTexture::Load(char* filename)
{
    Load((const char*)filename);
}


