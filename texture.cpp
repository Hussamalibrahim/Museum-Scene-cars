#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdio.h>
#include <stdlib.h>
#include "stb_image.h"

#pragma warning(disable:4996)

GLuint LoadTexture(const char* filename)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (!data)
    {
        MessageBoxA(NULL, filename, "Could not load texture", MB_ICONERROR);
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Build mipmaps for OpenGL 1.1
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA,
                      width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return texID;
}
