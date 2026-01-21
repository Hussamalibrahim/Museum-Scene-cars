#include <windows.h>
#include <gl/GLU.h>
#include "skybox.h"
#include "stb_image.h"
#include "texture.h"   // <--- IMPORTANT: add this to use LoadTexture()

GLuint skyboxTex[6];

void LoadSkybox(
    const char* right,
    const char* left,
    const char* top,
    const char* bottom,
    const char* front,
    const char* back)
{
    skyboxTex[SKY_RIGHT]  = LoadTexture(right);
    skyboxTex[SKY_LEFT]   = LoadTexture(left);
    skyboxTex[SKY_TOP]    = LoadTexture(top);
    skyboxTex[SKY_BOTTOM] = LoadTexture(bottom);
    skyboxTex[SKY_FRONT]  = LoadTexture(front);
    skyboxTex[SKY_BACK]   = LoadTexture(back);
}

void DrawSkybox(float size)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glScalef(size, size, size);

    // RIGHT
    glBindTexture(GL_TEXTURE_2D, skyboxTex[SKY_RIGHT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f( 1,-1,-1);
    glTexCoord2f(1,0); glVertex3f( 1,-1, 1);
    glTexCoord2f(1,1); glVertex3f( 1, 1, 1);
    glTexCoord2f(0,1); glVertex3f( 1, 1,-1);
    glEnd();

    // LEFT
    glBindTexture(GL_TEXTURE_2D, skyboxTex[SKY_LEFT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1, 1,-1);
    glTexCoord2f(0,1); glVertex3f(-1, 1, 1);
    glEnd();

    // TOP
    glBindTexture(GL_TEXTURE_2D, skyboxTex[SKY_TOP]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,1); glVertex3f(-1, 1,-1);
    glTexCoord2f(0,0); glVertex3f(-1, 1, 1);
    glTexCoord2f(1,0); glVertex3f( 1, 1, 1);
    glTexCoord2f(1,1); glVertex3f( 1, 1,-1);
    glEnd();

    // BOTTOM
    glBindTexture(GL_TEXTURE_2D, skyboxTex[SKY_BOTTOM]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(0,1); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,1); glVertex3f( 1,-1, 1);
    glTexCoord2f(1,0); glVertex3f( 1,-1,-1);
    glEnd();

    // FRONT
    glBindTexture(GL_TEXTURE_2D, skyboxTex[SKY_FRONT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f( 1,-1, 1);
    glTexCoord2f(1,1); glVertex3f( 1, 1, 1);
    glTexCoord2f(0,1); glVertex3f(-1, 1, 1);
    glEnd();

    // BACK
    glBindTexture(GL_TEXTURE_2D, skyboxTex[SKY_BACK]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f( 1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1, 1,-1);
    glTexCoord2f(0,1); glVertex3f( 1, 1,-1);
    glEnd();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}
