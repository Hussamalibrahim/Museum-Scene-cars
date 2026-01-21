#ifndef SKYBOX_H
#define SKYBOX_H
#include <windows.h>
#include <gl/GL.h>


extern GLuint skyboxTex[6];

enum {
    SKY_RIGHT = 0,
    SKY_LEFT,
    SKY_TOP,
    SKY_BOTTOM,
    SKY_FRONT,
    SKY_BACK
};

void LoadSkybox(
    const char* right,
    const char* left,
    const char* top,
    const char* bottom,
    const char* front,
    const char* back);

void DrawSkybox(float size);

#endif
