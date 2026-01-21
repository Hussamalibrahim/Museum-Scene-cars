#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

class GLTexture
{
public:
    GLuint texID;
    int width;
    int height;

    GLTexture();
    ~GLTexture();

    void Use();
    void BuildColorTexture(unsigned char r, unsigned char g, unsigned char b);

    void Load(const char* filename);   // real loader
    void Load(char* filename);         // wrapper, NO BODY HERE
};

#endif
