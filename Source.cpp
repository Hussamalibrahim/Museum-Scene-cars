#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include "texture.h"
#include "3DTexture.h"
#include "camera.h"
#include "heightMap.h"
#include "Model_3DS.h"
#include "TgaLoader.h"
#include "skybox.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


HDC hDC = NULL;
HGLRC hRC = NULL;
HWND hWnd = NULL;
HINSTANCE hInstance;

bool keys[256];
bool active = true;

// Camera 
Camera cam;
float moveSpeed = 0.3f;
float lastMouseX = 0;
float lastMouseY = 0;
bool initMouse = true;

//  textures
int texture[64];

//  models
Model_3DS Jeep;
Model_3DS lotus;
Model_3DS Subaru;
Model_3DS tree;
Model_3DS SignStore;
Model_3DS suc;

// dooe movment
float movingInsideDoor[3] = { -20.0f, -20.0f, -20.0f };
float doorOpenSpeed[3] = { 0.10f, 0.08f, 0.06f };
float doorOpenFactor = 0.0f;

// car movment
bool insideCar = false;           
float carX = 0, carY = -2, carZ = 20;   

bool insideTruck = false;
bool isPlayerNearTruck();


float truckRot = -45;               
float truckSpeed = 0.3f;            

float truckBaseX = 60, truckBaseY = -2, truckBaseZ = -60;

void ReSizeGLScene(int width, int height)
{
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)width / height, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}

void updateCamera()
{
    cam.Render();
}

void initLighting() {
    glEnable(GL_LIGHTING);

    for (int i = 0; i < 8; i++) {
        glEnable(GL_LIGHT0 + i);
    }

    GLfloat globalAmbient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    auto setLight = [](GLenum light, float x, float y, float z, float intensity) {
        GLfloat pos[] = { x, y, z, 1.0f };
        GLfloat dif[] = { intensity, intensity, intensity, 1.0f };
        GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        glLightfv(light, GL_POSITION, pos);
        glLightfv(light, GL_DIFFUSE, dif);
        glLightfv(light, GL_AMBIENT, amb);
        };

    setLight(GL_LIGHT0, 0.0f, 40.0f, 40.0f, 3.0f);    
    setLight(GL_LIGHT1, 150.0f, 40.0f, 0.0f, 0.8f);   
    setLight(GL_LIGHT2, -50.0f, 40.0f, 0.0f, 2.0f);   
    setLight(GL_LIGHT3, 50.0f, 80.0f, -50.0f, 0.6f);  
    setLight(GL_LIGHT4, 200.0f, 30.0f, 50.0f, 0.7f);  
    setLight(GL_LIGHT5, -100.0f, 30.0f, -50.0f, 0.7f); 
    setLight(GL_LIGHT6, 75.0f, 10.0f, 20.0f, 0.5f);   
    setLight(GL_LIGHT7, 0.0f, 100.0f, 0.0f, 0.4f);    
}

void LoadAllTextures()
{
    texture[0] = LoadTexture("model/wall1.bmp");
    texture[1] = LoadTexture("model/wall2.bmp");
    texture[2] = LoadTexture("model/wall3.bmp");
    texture[3] = LoadTexture("model/wall4.bmp");
    texture[4] = LoadTexture("model/wall5.bmp");
    texture[5] = LoadTexture("model/wall6.bmp");
    texture[6] = LoadTexture("model/wall7.bmp");
    texture[7] = LoadTexture("model/third_room_floor.bmp");
    texture[8] = LoadTexture("model/third_room_ceiling.bmp");
    texture[9] = LoadTexture("model/grass.bmp");
    texture[10] = LoadTexture("model/modernfloor.bmp");
    texture[11] = LoadTexture("model/ceiling.bmp");
    texture[12] = LoadTexture("model/road.bmp");


}

void LoadAllModels()
{
    lotus.Load("model/lotus/lotus_esprit_s1.3ds");
    lotus.scale = 12.5f;
    lotus.pos.x = 200.0f;
    lotus.pos.y = -11.0f;
    lotus.pos.z = -25.0f;

    Subaru.Load("model/Subaru/_Subaru-Loyale.3ds");
    Subaru.scale = 11.0f;
    Subaru.pos.x = 200.0f;
    Subaru.pos.y = -9.0f;
    Subaru.pos.z = 25.0f;

    SignStore.Load("model/Sign_Store/SIGN.3ds");
    SignStore.scale = 0.02f;
    SignStore.pos.x = -15.0f;
    SignStore.pos.y = 36.3f;	
    SignStore.pos.z = 0.0f;


    Jeep.Load("model/jeep/Jeep_Renegade_2016.3ds");
    Jeep.scale = 7.0f;
    Jeep.pos.x = 105.0f;
    Jeep.pos.y = -8.0f;
    Jeep.pos.z = -20.0f;

    tree.Load("model/tree1/Tree.3ds");
    tree.scale = 4.0f;
    tree.pos.x = 80.0f;
    tree.pos.y = 15.0f;
    tree.pos.z = 80.0f;

    suc.Load("model/suc1/car.3ds");
    suc.scale = 7.0f;
    suc.pos.x = 103.0f;
    suc.pos.y = -8.0f;
    suc.pos.z = 25.0f;

}

void InitCamera()
{
    cam.Position = Vector3dCreate(-100.0f, 2.0f, 0.0f);
    cam.View = Vector3dCreate(1.0f, 0.2f, 0.0f);  
}

int InitGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_SMOOTH);

    glClearColor(0, 0, 0, 1);
    InitCamera();
    LoadAllTextures();
    LoadAllModels();

    LoadSkybox(
        "model/sky/right.jpg",
        "model/sky/left.jpg",
        "model/sky/top.jpg",
        "model/sky/bottom.jpg",
        "model/sky/front.jpg",
        "model/sky/back.jpg"
    );

    return TRUE;
}

class HallsRenderer {
public:

    static void drawCelling(float w, float h, float d) {
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-w, h, -d);
        glTexCoord2f(1, 0); glVertex3f(w, h, -d);
        glTexCoord2f(1, 1); glVertex3f(w, h, d);
        glTexCoord2f(0, 1); glVertex3f(-w, h, d);
        glEnd();
    }

    static void drawFloor(float w, float h, float d) {
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-w, -h, -d);
        glTexCoord2f(1, 0); glVertex3f(w, -h, -d);
        glTexCoord2f(1, 1); glVertex3f(w, -h, d);
        glTexCoord2f(0, 1); glVertex3f(-w, -h, d);
        glEnd();
    }

    static void drawLeft(float w, float h, float d) {
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-w, -h, -d);
        glTexCoord2f(1, 0); glVertex3f(w, -h, -d);
        glTexCoord2f(1, 1); glVertex3f(w, h, -d);
        glTexCoord2f(0, 1); glVertex3f(-w, h, -d);
        glEnd();
    }

    static void drawRight(float w, float h, float d) {
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-w, -h, d);
        glTexCoord2f(1, 0); glVertex3f(w, -h, d);
        glTexCoord2f(1, 1); glVertex3f(w, h, d);
        glTexCoord2f(0, 1); glVertex3f(-w, h, d);
        glEnd();
    }

    static void drawFrontWithDoor(float w, float h, float d, int hallIndex) {
        glBindTexture(GL_TEXTURE_2D, texture[5]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, -0.3 * d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, -0.3 * d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texture[5]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, 0.3 * d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, 0.3 * d);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texture[4]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.5 * h + movingInsideDoor[hallIndex], -0.3 * d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, 0.5 * h + movingInsideDoor[hallIndex], 0.3 * d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, 2.55 * h + movingInsideDoor[hallIndex], 0.3 * d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, 2.55 * h + movingInsideDoor[hallIndex], -0.3 * d);
        glEnd();
    }

    static void drawglass(float width, float depth, float height, float doorWidth, float doorHeight, float doorOpenFactor)
    {
        float w = width / 2.0f;
        float d = depth / 2.0f;
        float h = height / 2.0f;

        float dw = doorWidth / 2.0f;
        float dh = doorHeight;
        float slideOffset = dw * doorOpenFactor;

        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.5f, 0.7f, 0.9f, 0.3f);
        glBegin(GL_QUADS);

        glVertex3f(-w + 0.01f, -h, 0.0f + slideOffset);
        glVertex3f(-w + 0.01f, -h, dw + slideOffset);
        glVertex3f(-w + 0.01f, -h + dh, dw + slideOffset);
        glVertex3f(-w + 0.01f, -h + dh, 0.0f + slideOffset);

        glVertex3f(-w + 0.01f, -h, 0.0f - slideOffset);
        glVertex3f(-w + 0.01f, -h, -dw - slideOffset);
        glVertex3f(-w + 0.01f, -h + dh, -dw - slideOffset);
        glVertex3f(-w + 0.01f, -h + dh, 0.0f - slideOffset);

        glVertex3f(-w, -h, -d);
        glVertex3f(w, -h, -d);
        glVertex3f(w, h, -d);
        glVertex3f(-w, h, -d);

        glVertex3f(-w, -h, d);
        glVertex3f(w, -h, d);
        glVertex3f(w, h, d);
        glVertex3f(-w, h, d);

        glEnd();

        glColor4f(0.7f, 0.7f, 0.7f, 0.3f);
        glBegin(GL_QUADS);

        glVertex3f(-w, -h, d); glVertex3f(-w, -h, dw); glVertex3f(-w, h, dw); glVertex3f(-w, h, d);
        glVertex3f(-w, -h, -dw); glVertex3f(-w, -h, -d); glVertex3f(-w, h, -d); glVertex3f(-w, h, -dw);
        glVertex3f(-w, -h + dh, -dw); glVertex3f(-w, -h + dh, dw); glVertex3f(-w, h, dw); glVertex3f(-w, h, -dw);

        glEnd();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);

        glColor3f(0.2f, 0.2f, 0.2f);
        glLineWidth(3.0f);

        glBegin(GL_LINE_LOOP);
        glVertex3f(-w + 0.02f, -h, 0.0f + slideOffset);
        glVertex3f(-w + 0.02f, -h, dw + slideOffset);
        glVertex3f(-w + 0.02f, -h + dh, dw + slideOffset);
        glVertex3f(-w + 0.02f, -h + dh, 0.0f + slideOffset);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(-w + 0.02f, -h, 0.0f - slideOffset);
        glVertex3f(-w + 0.02f, -h, -dw - slideOffset);
        glVertex3f(-w + 0.02f, -h + dh, -dw - slideOffset);
        glVertex3f(-w + 0.02f, -h + dh, 0.0f - slideOffset);
        glEnd();
    }

    static void drawHall1(float width, float depth, float height)
    {
        float w = width / 2.0f;
        float d = depth / 2.0f;
        float h = height / 2.0f;

        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        float tiles = 4.0f;

        glBindTexture(GL_TEXTURE_2D, texture[12]);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);      glVertex3f(-1 * w, -h, -4.5 * d);
        glTexCoord2f(1, 0);      glVertex3f(-1 * w, -h, 4.5 * d);
        glTexCoord2f(1, 1);  glVertex3f(-3 * w, -h, 4.5 * d);
        glTexCoord2f(0, 1);  glVertex3f(-3 * w, -h, -4.5 * d);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texture[10]);
        drawFloor(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[11]);
        drawCelling(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[2]);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-w, h, -0.99 * d);
        glTexCoord2f(1, 0); glVertex3f(-0.95 * w, h, -0.99 * d);
        glTexCoord2f(1, 1); glVertex3f(-0.95 * w, -h, -0.99 * d);
        glTexCoord2f(0, 1); glVertex3f(-w, -h, -0.99 * d);
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-w, h, 0.99 * d);
        glTexCoord2f(1, 0); glVertex3f(-0.95 * w, h, 0.99 * d);
        glTexCoord2f(1, 1); glVertex3f(-0.95 * w, -h, 0.99 * d);
        glTexCoord2f(0, 1); glVertex3f(-w, -h, 0.99 * d);
        glEnd();
    }

    static void drawHall2(float width, float depth, float height)
    {
        float w = width / 2.0f;
        float d = depth / 2.0f;
        float h = height / 2.0f;

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, texture[10]);
        drawFloor(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[11]);
        drawCelling(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[0]);
        drawLeft(w, h, d);
        drawRight(w, h, d);
    }

    static void drawHall3(float width, float depth, float height)
    {
        float w = width / 2.0f;
        float d = depth / 2.0f;
        float h = height / 2.0f;

        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);

        drawFloor(w, h, d);
        drawCelling(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[6]);
        glBegin(GL_QUADS);
        drawLeft(w, h, d);
        drawFrontWithDoor(w, h, d, 0);
        drawRight(w, h, d);
    }

    static void drawHall4(float width, float depth, float height)
    {
        float w = width / 2.0f;
        float d = depth / 2.0f;
        float h = height / 2.0f;

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, texture[7]);
        drawFloor(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[7]);
        drawCelling(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[1]);
        drawLeft(w, h, d);

        drawFrontWithDoor(w, h, d, 1);

        glBindTexture(GL_TEXTURE_2D, texture[1]);
        drawRight(w, h, d);
    }

    static void drawHall5(float width, float depth, float height)
    {
        float w = width / 2.0f;
        float d = depth / 2.0f;
        float h = height / 2.0f;

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, texture[7]);
        drawFloor(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[7]);
        drawCelling(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[2]);
        drawRight(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[2]);
        drawLeft(w, h, d);

        glBindTexture(GL_TEXTURE_2D, texture[7]);
        drawFrontWithDoor(w, h, d, 2);

        glBindTexture(GL_TEXTURE_2D, texture[3]);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w, h, -d);
        glEnd();
    }

	static void drawHalls(float hallW, float hallD, float hallH, float offsetX, float spacing)
    {
        glPushMatrix();
        glTranslatef(offsetX, 0, 0);
        drawHall1(hallW, hallD, hallH);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(offsetX + spacing, 0, 0);
        drawHall2(hallW, hallD, hallH);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(offsetX + spacing * 2, 0, 0);
        drawHall3(hallW, hallD, hallH);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(offsetX + spacing * 3, 0, 0);
        drawHall4(hallW, hallD, hallH);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(offsetX + spacing * 4, 0, 0);
        drawHall5(hallW, hallD, hallH);
        glPopMatrix();
    }
};

class CarsRenderer {
public:

    static void drawCube(float w, float h, float d)
    {
        w *= 0.5f;
        h *= 0.5f;
        d *= 0.5f;

        glBegin(GL_QUADS);

        glVertex3f(-w, -h, d);
        glVertex3f(w, -h, d);
        glVertex3f(w, h, d);
        glVertex3f(-w, h, d);

        glVertex3f(-w, -h, -d);
        glVertex3f(w, -h, -d);
        glVertex3f(w, h, -d);
        glVertex3f(-w, h, -d);

        glVertex3f(-w, -h, -d);
        glVertex3f(-w, -h, d);
        glVertex3f(-w, h, d);
        glVertex3f(-w, h, -d);

        glVertex3f(w, -h, -d);
        glVertex3f(w, -h, d);
        glVertex3f(w, h, d);
        glVertex3f(w, h, -d);

        glVertex3f(-w, h, -d);
        glVertex3f(w, h, -d);
        glVertex3f(w, h, d);
        glVertex3f(-w, h, d);

        glVertex3f(-w, -h, -d);
        glVertex3f(w, -h, -d);
        glVertex3f(w, -h, d);
        glVertex3f(-w, -h, d);

        glEnd();
    }

    static void DrawWheel3D(float radius, float width, int segments)
    {
        float halfW = width * 0.5f;

        glColor3f(0, 0, 0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, halfW);
        for (int i = 0; i <= segments; i++)
        {
            float angle = i * (2.0f * 3.14159f / segments);
            float x = cos(angle) * radius;
            float y = sin(angle) * radius;
            glVertex3f(x, y, halfW);
        }
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, -halfW);
        for (int i = 0; i <= segments; i++)
        {
            float angle = i * (2.0f * 3.14159f / segments);
            float x = cos(angle) * radius;
            float y = sin(angle) * radius;
            glVertex3f(x, y, -halfW);
        }
        glEnd();

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; i++)
        {
            float angle = i * (2.0f * 3.14159f / segments);
            float x = cos(angle) * radius;
            float y = sin(angle) * radius;

            glVertex3f(x, y, halfW);
            glVertex3f(x, y, -halfW);
        }
        glEnd();
        glColor3f(1, 1, 1);
    }
	// Hussam's Car
    static void drawingCar() {

        glPushMatrix();
        glTranslatef(35, -5, 0);
        glColor3f(0.05f, 0.05f, 0.05f);

        glBegin(GL_QUADS);
        glVertex3f(0, -4, -10);
        glVertex3f(0, -4, 10);
        glVertex3f(0, 4, 10);
        glVertex3f(0, 4, -10);
        glEnd();

        glTranslatef(-10, 0, 0);
        glBegin(GL_QUADS);
        glVertex3f(0, -4, -10);
        glVertex3f(0, -4, 10);
        glVertex3f(0, 4, 10);
        glVertex3f(0, 4, -10);
        glEnd();

        glTranslatef(5, -4, 0);
        glBegin(GL_QUADS);
        glVertex3f(5, 0, -10);
        glVertex3f(5, 0, 10);
        glVertex3f(-5, 0, 10);
        glVertex3f(-5, 0, -10);
        glEnd();

        glPushMatrix();
        glColor3f(0.05f, 0.05f, 0.05f);

        glTranslatef(0, 0, 10);
        glBegin(GL_QUADS);
        glVertex3f(5, 0, 0);
        glVertex3f(5, 5, 2);
        glVertex3f(-5, 5, 2);
        glVertex3f(-5, 0, 0);
        glEnd();

        glTranslatef(0, 5, 2);
        glBegin(GL_QUADS);
        glVertex3f(5, 0, 0);
        glVertex3f(5, 0, 7.5);
        glVertex3f(-5, 0, 7.5);
        glVertex3f(-5, 0, 0);
        glEnd();

        glTranslatef(0, 0, 9.5);
        glBegin(GL_QUADS);
        glVertex3f(5, -5, 0);
        glVertex3f(5, 0, -2);
        glVertex3f(-5, 0, -2);
        glVertex3f(-5, -5, 0);
        glEnd();

        glTranslatef(-5, 3, -11.5);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -3, 2);
        glVertex3f(0, -8, 0);
        glEnd();

        glTranslatef(0, 0, 2);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 7.5);
        glVertex3f(0, -3, 7.5);
        glVertex3f(0, -3, 0);
        glEnd();

        glTranslatef(0, 0, 7.5);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -8, 2);
        glVertex3f(0, -3, 0);
        glEnd();

        glTranslatef(0, 0, 2);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(3, -2, 10);
        glVertex3f(3, -4, 10);
        glVertex3f(0, -8, 0);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3f(0, -8, 0);
        glVertex3f(3, -4, 10);
        glVertex3f(7, -4, 10);
        glVertex3f(10, -8, 0);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(3, -2, 10);
        glVertex3f(7, -2, 10);
        glVertex3f(10, 0, 0);
        glEnd();

        glColor3f(1, 1, 1);
        glColor3f(0.05f, 0.05f, 0.05f);
        glBegin(GL_QUADS);
        glVertex3f(0, 4, 0);
        glVertex3f(0, 4, -42.5);
        glVertex3f(10, 4, -42.5);
        glVertex3f(10, 4, 0);
        glEnd();

        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        glColor4f(0.6f, 0.6f, 0.7f, 0.25f);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 4, 0);
        glVertex3f(10, 4, 0);
        glVertex3f(10, 0, 0);
        glEnd();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);

        glColor3f(0.05f, 0.05f, 0.05f);
        glTranslatef(0.25, 0, 0.25);
        drawCube(0.5, 8, 0.5);

        glTranslatef(-0.25, 0, -0.25);

        glTranslatef(0.25, 0, -10.25);
        drawCube(0.5, 8, 0.5);
        glTranslatef(-0.25, 0, 10.25);

        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glColor3f(1, 1, 1);

        glColor4f(0.6f, 0.6f, 0.7f, 0.25f);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, -10);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 4, 0);
        glVertex3f(0, 4, -10);
        glEnd();

        glTranslatef(0, 0, -20.5);
        glColor4f(0.6f, 0.6f, 0.7f, 0.25f);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, -22);
        glVertex3f(0, 0, 10);
        glVertex3f(0, 4, 10);
        glVertex3f(0, 4, -22);
        glEnd();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);

        glPopMatrix();

        glPushMatrix();
        glColor3f(0.05f, 0.05f, 0.05f);

        glTranslatef(10, 0, 10);
        glTranslatef(0, 5, 2);
        glTranslatef(0, 0, 9.5);
        glTranslatef(-5, 3, -11.5);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -3, 2);
        glVertex3f(0, -8, 0);
        glEnd();

        glTranslatef(0, 0, 2);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 7.5);
        glVertex3f(0, -3, 7.5);
        glVertex3f(0, -3, 0);
        glEnd();

        glTranslatef(0, 0, 7.5);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -8, 2);
        glVertex3f(0, -3, 0);
        glEnd();

        glTranslatef(0, 0, 2);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(-3, -2, 10);
        glVertex3f(-3, -4, 10);
        glVertex3f(0, -8, 0);
        glEnd();

        glTranslatef(0.5, 0, 0);
        glTranslatef(-0.5, 0, -10.25);
        glTranslatef(-0.5, 0, 0);
        drawCube(0.5, 8, 0.5);
        glTranslatef(0.5, 0, 0);

        glTranslatef(-0.25, 0, 10.25);
        drawCube(0.5, 8, 0.5);
        glTranslatef(0.25, 0, -10.25);

        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        glColor4f(0.6f, 0.6f, 0.7f, 0.25f);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 10);
        glVertex3f(0, 4, 10);
        glVertex3f(0, 4, 0);
        glEnd();

        glTranslatef(0, 0, -20.5);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, -11.8);
        glVertex3f(0, 0, 20.5);
        glVertex3f(0, 4, 20.5);
        glVertex3f(0, 4, -11.8);
        glEnd();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);

        glPopMatrix();

        glTranslatef(-10, 0, -12.5);
        glPushMatrix();
        glColor3f(0.05f, 0.05f, 0.05f);

        glTranslatef(5, 8, 1.5);
        glBegin(GL_QUADS);
        glVertex3f(0, 4, -10);
        glVertex3f(0, -8, -10);
        glVertex3f(10, -8, -10);
        glVertex3f(10, 4, -10);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, -31.5);
        glTranslatef(10, 0, 13);
        glTranslatef(0, 0, 10);
        glColor3f(0.05f, 0.05f, 0.05f);

        glBegin(GL_QUADS);
        glVertex3f(5, 0, 0);
        glVertex3f(5, 5, 2);
        glVertex3f(-5, 5, 2);
        glVertex3f(-5, 0, 0);
        glEnd();

        glTranslatef(0, 5, 2);
        glBegin(GL_QUADS);
        glVertex3f(5, 0, 0);
        glVertex3f(5, 0, 7.5);
        glVertex3f(-5, 0, 7.5);
        glVertex3f(-5, 0, 0);
        glEnd();

        glTranslatef(0, 0, 9.5);
        glBegin(GL_QUADS);
        glVertex3f(5, -5, 0);
        glVertex3f(5, 0, -2);
        glVertex3f(-5, 0, -2);
        glVertex3f(-5, -5, 0);
        glEnd();

        glTranslatef(-5, 3, -11.5);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -3, 2);
        glVertex3f(0, -8, 0);
        glEnd();

        glTranslatef(0, 0, 2);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 7.5);
        glVertex3f(0, -3, 7.5);
        glVertex3f(0, -3, 0);
        glEnd();

        glTranslatef(0, 0, 7.5);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -8, 2);
        glVertex3f(0, -3, 0);
        glEnd();

        glPopMatrix();

        glPushMatrix();
        glTranslatef(10, 0, 13);

        glPushMatrix();
        glTranslatef(5, 0, 16);
        glRotatef(90, 0, 1, 0);
        DrawWheel3D(4.5f, 2.0f, 32);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-5, 0, 15);
        glRotatef(90, 0, 1, 0);
        DrawWheel3D(4.5f, 2.0f, 32);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(5, 0, -16);
        glRotatef(90, 0, 1, 0);
        DrawWheel3D(4.5f, 2.0f, 32);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-5, 0, -16);
        glRotatef(90, 0, 1, 0);
        DrawWheel3D(4.5f, 2.0f, 32);
        glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(10, 5, -10);
        glTranslatef(10, 0, 13);
        glTranslatef(-5, 3, -11.5);
        glColor3f(0.05f, 0.05f, 0.05f);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -3, 2);
        glVertex3f(0, -8, 0);
        glEnd();

        glTranslatef(0, 0, 2);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 7.5);
        glVertex3f(0, -3, 7.5);
        glVertex3f(0, -3, 0);
        glEnd();

        glTranslatef(0, 0, 7.5);
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 2);
        glVertex3f(0, -8, 2);
        glVertex3f(0, -3, 0);
        glEnd();

        glPopMatrix();
        glPopMatrix();

        glPopMatrix();
        glColor3f(1, 1, 1);

    }

    static void drawLegacyCylinder(float radius, float length) {
        GLUquadric* q = gluNewQuadric();
        glPushMatrix();
        glTranslatef(0, 0, -length * 0.5f);
        gluCylinder(q, radius, radius, length, 32, 1);
        gluDisk(q, 0.0, radius, 32, 1);
        glTranslatef(0, 0, length);
        gluDisk(q, 0.0, radius, 32, 1);
        glPopMatrix();
        gluDeleteQuadric(q);
    }
	// Anas's Car
    static void drawProjectCar() {
        glPushMatrix();
        glTranslatef(150, -9, 0);
        glRotatef(-60, 0, 1, 0);
        
        glColor3f(0.85f, 0.10f, 0.12f);
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        drawCube(40.0f, 4.0f, 20.0f);
        glPopMatrix();

        glColor3f(0.70f, 0.08f, 0.10f);
        glPushMatrix();
        glTranslatef(12.0f, 2.5f, 0.0f);
        drawCube(15.0f, 2.0f, 18.0f);
        glPopMatrix();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glColor4f(0.5f, 0.8f, 1.0f, 0.4f);
        glPushMatrix();
        glTranslatef(-5.0f, 5.0f, 0.0f);
        drawCube(12.0f, 6.0f, 16.0f);
        glPopMatrix();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glColor3f(1.0f, 1.0f, 0.8f);
        glPushMatrix();
        glTranslatef(20.1f, 0.0f, 7.0f);
        drawCube(0.5f, 1.5f, 3.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(20.1f, 0.0f, -7.0f);
        drawCube(0.5f, 1.5f, 3.0f);
        glPopMatrix();

        glColor3f(0.1f, 0.1f, 0.1f);
        float wheelPos[4][3] = {
            {12, -2, 10}, {12, -2, -10}, {-12, -2, 10}, {-12, -2, -10}
        };
        for (int i = 0; i < 4; i++) {
            glPushMatrix();
            glTranslatef(wheelPos[i][0], wheelPos[i][1], wheelPos[i][2]);
            glRotatef(180, 0, 1, 0);
            drawLegacyCylinder(4.0f, 2.0f);
            glPopMatrix();
        }

        glPopMatrix();
    }
	// Yousef's Car
	static void drawingTruck() {
        glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(0, -5, 0);

        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
        glVertex3f(-20, -2, -10);
        glVertex3f(20, -2, -10);
        glVertex3f(20, -2, 10);
        glVertex3f(-20, -2, 10);

        glColor3f(0.2f, 0.3f, 0.8f);
        glVertex3f(10, 2, -10);
        glVertex3f(20, 2, -10);
        glVertex3f(20, 2, 10);
        glVertex3f(10, 2, 10);
        glEnd();

        glColor3f(0.2f, 0.3f, 0.8f);
        glBegin(GL_QUADS);
        glVertex3f(-20, -2, 10); glVertex3f(0, -2, 10); glVertex3f(0, 2, 10); glVertex3f(-20, 2, 10);
        glVertex3f(-20, -2, -10); glVertex3f(0, -2, -10); glVertex3f(0, 2, -10); glVertex3f(-20, 2, -10);
        glVertex3f(-20, -2, -10); glVertex3f(-20, -2, 10); glVertex3f(-20, 2, 10); glVertex3f(-20, 2, -10);

        glVertex3f(20, -2, -10);
        glVertex3f(20, -2, 10);
        glVertex3f(20, 2, 10);
        glVertex3f(20, 2, -10);

        glVertex3f(0, -2, 10);
        glVertex3f(20, -2, 10);
        glVertex3f(20, 2, 10);
        glVertex3f(0, 2, 10);

        glVertex3f(0, -2, -10);
        glVertex3f(20, -2, -10);
        glVertex3f(20, 2, -10);
        glVertex3f(0, 2, -10);
        glEnd();

        glColor3f(0.2f, 0.3f, 0.8f);
        glBegin(GL_QUADS);
        glVertex3f(-20, 2, 10);   glVertex3f(0, 2, 10);    glVertex3f(0, 2.5, 10);  glVertex3f(-20, 2.5, 10);
        glVertex3f(-20, 7.5, 10); glVertex3f(0, 7.5, 10);  glVertex3f(0, 8, 10);    glVertex3f(-20, 8, 10);
        glVertex3f(-20, 2.5, 10); glVertex3f(-19.5, 2.5, 10); glVertex3f(-19.5, 7.5, 10); glVertex3f(-20, 7.5, 10);
        glVertex3f(-0.5, 2.5, 10); glVertex3f(0, 2.5, 10);    glVertex3f(0, 7.5, 10);   glVertex3f(-0.5, 7.5, 10);

        glVertex3f(-20, 2, -10);   glVertex3f(0, 2, -10);    glVertex3f(0, 2.5, -10);  glVertex3f(-20, 2.5, -10);
        glVertex3f(-20, 7.5, -10); glVertex3f(0, 7.5, -10);  glVertex3f(0, 8, -10);    glVertex3f(-20, 8, -10);
        glVertex3f(-20, 2.5, -10); glVertex3f(-19.5, 2.5, -10); glVertex3f(-19.5, 7.5, -10); glVertex3f(-20, 7.5, -10);
        glVertex3f(-0.5, 2.5, -10); glVertex3f(0, 2.5, -10);    glVertex3f(0, 7.5, -10);   glVertex3f(-0.5, 7.5, -10);

        glVertex3f(-20, 2, -10);   glVertex3f(-20, 2, 10);   glVertex3f(-20, 2.5, 10); glVertex3f(-20, 2.5, -10);
        glVertex3f(-20, 7.5, -10); glVertex3f(-20, 7.5, 10); glVertex3f(-20, 8, 10);   glVertex3f(-20, 8, -10);
        glVertex3f(-20, 2.5, -10); glVertex3f(-20, 2.5, -9.5); glVertex3f(-20, 7.5, -9.5); glVertex3f(-20, 7.5, -10);
        glVertex3f(-20, 2.5, 9.5);  glVertex3f(-20, 2.5, 10);  glVertex3f(-20, 7.5, 10);   glVertex3f(-20, 7.5, 9.5);
        glEnd();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glColor4f(0.7f, 0.8f, 1.0f, 0.3f);
        glBegin(GL_QUADS);
        glVertex3f(-19.5, 2.5, 10); glVertex3f(-0.5, 2.5, 10); glVertex3f(-0.5, 7.5, 10); glVertex3f(-19.5, 7.5, 10);
        glVertex3f(-19.5, 2.5, -10); glVertex3f(-0.5, 2.5, -10); glVertex3f(-0.5, 7.5, -10); glVertex3f(-19.5, 7.5, -10);
        glVertex3f(-20, 2.5, -9.5); glVertex3f(-20, 2.5, 9.5); glVertex3f(-20, 7.5, 9.5); glVertex3f(-20, 7.5, -9.5);
        glEnd();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glColor4f(0.7f, 0.8f, 1.0f, 0.3f);
        glBegin(GL_QUADS);
        glVertex3f(10, 2, -10); glVertex3f(0, 8, -10); glVertex3f(0, 8, 10); glVertex3f(10, 2, 10);
        glEnd();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glColor3f(0.15f, 0.15f, 0.15f);
        glBegin(GL_QUADS);
        glVertex3f(-20, 8, -10); glVertex3f(0, 8, -10); glVertex3f(0, 8, 10); glVertex3f(-20, 8, 10);
        glVertex3f(0, 8, 10); glVertex3f(0, 8, 9.5); glVertex3f(10, 2, 9.5); glVertex3f(10, 2, 10);
        glVertex3f(0, 8, -10); glVertex3f(0, 8, -9.5); glVertex3f(10, 2, -9.5); glVertex3f(10, 2, -10);
        glEnd();

        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-12, -2, 10); glRotatef(90, 0, 1, 0); DrawWheel3D(4.0f, 2.0f, 32);
        glPopMatrix();
        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-12, -2, -10); glRotatef(90, 0, 1, 0); DrawWheel3D(4.0f, 2.0f, 32);
        glPopMatrix();
        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(12, -2, 10); glRotatef(90, 0, 1, 0); DrawWheel3D(4.0f, 2.0f, 32);
        glPopMatrix();
        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(12, -2, -10); glRotatef(90, 0, 1, 0); DrawWheel3D(4.0f, 2.0f, 32);
        glPopMatrix();
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
	
	static void drawLotus()
    {
        glPushMatrix();
        glTranslatef(lotus.pos.x, lotus.pos.y, lotus.pos.z);
        glRotatef(-30, 0, 1, 0);
        glScalef(lotus.scale, lotus.scale, lotus.scale);
        lotus.Draw();
        glPopMatrix();
    }

    static void drawSubaru()
    {
        glPushMatrix();
        glTranslatef(Subaru.pos.x, Subaru.pos.y, Subaru.pos.z);
        glRotatef(30, 0, 1, 0);
        glScalef(Subaru.scale, Subaru.scale, Subaru.scale);
        Subaru.Draw();
        glPopMatrix();
    }

    static void drawSign()
    {
        glPushMatrix();
        glTranslatef(SignStore.pos.x, SignStore.pos.y, SignStore.pos.z);
        glRotatef(-90, 0, 1, 0);
        glScalef(SignStore.scale, SignStore.scale, SignStore.scale);
        SignStore.Draw();
        glPopMatrix();
    }

    static void drawJeep()
    {
        glPushMatrix();
        glTranslatef(Jeep.pos.x, Jeep.pos.y, Jeep.pos.z);
        glScalef(Jeep.scale, Jeep.scale, Jeep.scale);
        Jeep.Draw();
        glPopMatrix();
    }

    static void drawSUC()
    {
        glPushMatrix();
        glTranslatef(suc.pos.x, suc.pos.y, suc.pos.z);
        glRotatef(-90, 0, 1, 0);
        glScalef(suc.scale, suc.scale, suc.scale);
        suc.Draw();
        glPopMatrix();
    }

    static void drawCustomCar()
    {
        glPushMatrix();
        glTranslatef(23, -2, -10);
        glRotatef(-20, 0, 1, 0);
        drawingCar();
        glPopMatrix();
    }
};

void drawTrees() {
    float frontZ = -70.0f;
    float backZ = 70.0f;
    float treeX_positions[3] = { -10.0f, 70.0f, 150.0f };

    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glTranslatef(treeX_positions[i], tree.pos.y, frontZ);
        glScalef(tree.scale, tree.scale, tree.scale);
        tree.Draw();
        glPopMatrix();
    }

    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glTranslatef(treeX_positions[i], tree.pos.y, backZ);
        glScalef(tree.scale, tree.scale, tree.scale);
        tree.Draw();
        glPopMatrix();
    }
}

void DrawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    updateCamera();
    initLighting();

    glPushMatrix();
    glTranslatef(100, 184, 0);
    DrawSkybox(200.0f);
    glPopMatrix();

    float roomW = 50.0f;
    float roomD = 90.0f;
    float roomH = 30.0f;
    float doorWidth = 18.0f;
    float doorHeight = 20.0f;

    HallsRenderer::drawHalls(roomW, roomD, roomH, 0.0f, roomW);

    glPushMatrix();
    HallsRenderer::drawglass(roomW, roomD, roomH, doorWidth, doorHeight, doorOpenFactor);
    glPopMatrix();

    CarsRenderer::drawLotus();
    CarsRenderer::drawSubaru();
    CarsRenderer::drawSign();
    CarsRenderer::drawJeep();
    CarsRenderer::drawSUC();
    CarsRenderer::drawCustomCar();
    CarsRenderer::drawProjectCar();

    glPushMatrix();
    glTranslatef(60.0f + carX, -2.0f + carY, -60.0f + carZ);
    glRotatef(truckRot - 45.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(30.0f, 0, 60.0f);
    CarsRenderer::drawingTruck();
    glPopMatrix();

    drawTrees();

    SwapBuffers(hDC);
}

void ProcessMouseLook(int mouseX, int mouseY)
{
    if (initMouse)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        initMouse = false;
        return;
    }

    float dx = (mouseX - lastMouseX) * 0.1f;
    float dy = (mouseY - lastMouseY) * 0.1f;

    cam.RotateYaw(-dx);
    cam.RotatePitch(-dy);

    int centerX = 1024 / 2;
    int centerY = 720 / 2;

    if (mouseX != centerX || mouseY != centerY) {
        POINT pt = { centerX, centerY };
        ClientToScreen(hWnd, &pt);
        SetCursorPos(pt.x, pt.y);

        lastMouseX = centerX;
        lastMouseY = centerY;
    }
}

bool isPlayerNearCar()
{
    float dx = cam.Position.x - carX;
    float dy = cam.Position.y - carY;
    float dz = cam.Position.z - carZ;

    float dist = sqrt(dx * dx + dy * dy + dz * dz);
    return dist < 10.0f;
}

void UpdateInsideDoors(float playerX, float playerZ)
{
    bool shouldOpen = false;

    if (playerX >= 0.0f && playerX <= 220.0f &&
        playerZ >= -45.0f && playerZ <= 45.0f)
    {
        for (int i = 0; i < 3; i++)
        {
            movingInsideDoor[i] += doorOpenSpeed[i];
            if (movingInsideDoor[i] > 20.0f)
                movingInsideDoor[i] = 20.0f;
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            movingInsideDoor[i] -= doorOpenSpeed[i];
            if (movingInsideDoor[i] < -20.0f)
                movingInsideDoor[i] = -20.0f;
        }
    }
}

void HandleMovement()
{
    float speed = 0.4f;
    float saftySpace = 3.0f;

    float oldX = cam.Position.x;
    float oldY = cam.Position.y;
    float oldZ = cam.Position.z;

    // Walking
    if (!insideCar && !insideTruck) {
        if (keys['W']) cam.MoveForward(speed);
        if (keys['S']) cam.MoveForward(-speed);
        if (keys['A']) cam.MoveRight(speed);
        if (keys['D']) cam.MoveRight(-speed);
        if (keys[VK_SPACE])   cam.MoveUpward(speed);
        if (keys[VK_CONTROL]) cam.MoveUpward(-speed);
    }

    // Truck rotation
    if (keys['J']) truckRot += 2.0f;
    if (keys['L']) truckRot -= 2.0f;

    float angleRad = (truckRot - 45.0f) * 3.14159f / 180.0f;

    // Truck movement
    if (keys['I']) {
        carX += cos(angleRad) * truckSpeed;
        carZ -= sin(angleRad) * truckSpeed;
    }
    if (keys['K']) {
        carX -= cos(angleRad) * truckSpeed;
        carZ += sin(angleRad) * truckSpeed;
    }

    // Inside truck camera
    if (insideTruck)
    {
        float bx = 60 + carX;
        float by = -2 + carY;
        float bz = -60 + carZ;

        float rx = 20 * cos(angleRad) + 60 * sin(angleRad);
        float rz = -20 * sin(angleRad) + 60 * cos(angleRad);

        cam.Position.x = bx + rx;
        cam.Position.y = by - 5 + 3.5f;
        cam.Position.z = bz + rz;

        cam.Yaw = truckRot - 45;
        cam.Pitch = 0;
        cam.UpdateViewVector();
    }

    float doorX = -25.0f;
    float doorZ = 0.0f;
    float trigger = 25.0f;

    float dx = cam.Position.x - doorX;
    float dz = cam.Position.z - doorZ;
    float dist = sqrt(dx * dx + dz * dz);

    static bool soundPlayedOpen = false;
    static bool soundPlayedClose = true;
    // Front glass door
    bool isOpening = (dist < trigger);
    bool isClosing = !isOpening;

    if (isOpening) {
        doorOpenFactor = min(1.0f, doorOpenFactor + 0.02f);

        if (!soundPlayedOpen) {
            PlaySound("audio/door.wav", NULL, SND_FILENAME | SND_ASYNC);
            soundPlayedOpen = true;
            soundPlayedClose = false;
        }
    }
    else {
        doorOpenFactor = max(0.0f, doorOpenFactor - 0.02f);

        if (!soundPlayedClose) {
            PlaySound("audio/door.wav", NULL, SND_FILENAME | SND_ASYNC);
            soundPlayedClose = true;
            soundPlayedOpen = false;
        }
    }

    // Inside doors
    UpdateInsideDoors(cam.Position.x, cam.Position.z);

    // Wall block
    float minX = -25.0f;
    float maxX = 225.0f;
    float minZ = -45.0f;
    float maxZ = 45.0f;

    float doorZmin = -8.0f;
    float doorZmax = 8.0f;

    bool wasInside =
        (oldX >= minX && oldX <= maxX &&
            oldZ >= minZ && oldZ <= maxZ);

    bool nowInside =
        (cam.Position.x >= minX && cam.Position.x <= maxX &&
            cam.Position.z >= minZ && cam.Position.z <= maxZ);

    if (!wasInside && nowInside)
    {
        bool inDoorZ = (cam.Position.z >= doorZmin && cam.Position.z <= doorZmax && cam.Position.x < 0);
        if (!inDoorZ) {
            cam.Position.x = oldX;
            cam.Position.z = oldZ;
            nowInside = false;
        }
    }

    if (wasInside)
    {
        bool inDoorZ_old = (oldZ >= doorZmin && oldZ <= doorZmax);
        // BACK WALL
        if (oldX >= minX + saftySpace && cam.Position.x < minX + saftySpace)
            if (!inDoorZ_old) cam.Position.x = oldX;
        // FRONT WALL
        if (oldX <= maxX - saftySpace && cam.Position.x > maxX - saftySpace)
            cam.Position.x = oldX;
        // RIGHT WALL
        if (oldZ >= minZ + saftySpace && cam.Position.z < minZ + saftySpace)
            cam.Position.z = oldZ;
        // LEFT WALL
        if (oldZ <= maxZ - saftySpace && cam.Position.z > maxZ - saftySpace)
            cam.Position.z = oldZ;
    }

    // Doors inside move
    float innerDoor1X = 75.0f;
    float innerDoor2X = 125.0f;
    float innerDoor3X = 175.0f;

    float innerDoorZmin = -10.0f;
    float innerDoorZmax = 10.0f;

    float doorsX[3] = { innerDoor1X, innerDoor2X, innerDoor3X };

    if (wasInside)
    {
        for (int i = 0; i < 3; i++)
        {
            float blockLeft = doorsX[i] - saftySpace;
            float blockRight = doorsX[i] + saftySpace;

            bool crossing =
                (oldX < blockLeft && cam.Position.x >= blockLeft) ||
                (oldX > blockRight && cam.Position.x <= blockRight);

            if (crossing)
            {
                bool inMiddle =
                    cam.Position.z >= innerDoorZmin &&
                    cam.Position.z <= innerDoorZmax;

                if (!inMiddle) cam.Position.x = oldX;
            }
        }
    }

    // Skybox bounding
    float SkyMinX = -100.0f + saftySpace;
    float SkyMaxX = 300.0f - saftySpace;
    float SkyMinY = -5.0f + saftySpace;
    float SkyMaxY = 15.0f - saftySpace;
    float SkyMinZ = -200.0f + saftySpace;
    float SkyMaxZ = 200.0f - saftySpace;

    if (cam.Position.x < SkyMinX) cam.Position.x = SkyMinX;
    if (cam.Position.x > SkyMaxX) cam.Position.x = SkyMaxX;
    if (cam.Position.y < SkyMinY) cam.Position.y = SkyMinY;
    if (cam.Position.y > SkyMaxY) cam.Position.y = SkyMaxY;
    if (cam.Position.z < SkyMinZ) cam.Position.z = SkyMinZ;
    if (cam.Position.z > SkyMaxZ) cam.Position.z = SkyMaxZ;

    if (keys[VK_ESCAPE]) PostQuitMessage(0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        active = !HIWORD(wParam);
        return 0;

    case WM_SYSCOMMAND:
        if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
            return 0;
        break;

    case WM_KEYDOWN:
        keys[wParam] = true;

        if (wParam == 'T')
        {
            if (!insideCar)
            {
                insideCar = true;
                insideTruck = false;

                cam.Position = Vector3dCreate(carX + 50.0f, carY + 2.0f, carZ - 10.0f);
                cam.View = Vector3dCreate(0.0f, 0.0f, -1.0f);

                cam.Yaw = 270.0f;
                cam.Pitch = 0.0f;
                cam.UpdateViewVector();
                initMouse = true;
            }
            else
            {
                insideCar = false;
                cam.Position.x = carX + 25.0f;
                cam.Position.y = -2.0f;
                initMouse = true;
            }
        }

        if (wParam == 'P')
        {
            if (!insideTruck)
            {
                insideTruck = true;
                insideCar = false;
                initMouse = true;
            }
            else
            {
                insideTruck = false;
                cam.Position.y = -2.0f;
                initMouse = true;
            }
        }

        return 0;

    case WM_KEYUP:
        keys[wParam] = false;
        return 0;

    case WM_MOUSEMOVE:
    {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);
        ProcessMouseLook(mx, my);
        return 0;
    }

    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    MSG msg;
    WNDCLASS wc = {};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "GLMuseum";
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);

    RegisterClass(&wc);

    hWnd = CreateWindow(
        "GLMuseum",
        "Museum Scene (Converted)",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1024, 720,
        NULL, NULL, hInst, NULL
    );

    hDC = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0,0,0,0,0,0,
        0,0,
        0,0,0,0,0,
        16,
        0,0,
        PFD_MAIN_PLANE,
        0,0,0,0
    };

    int pf = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pf, &pfd);

    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    ShowWindow(hWnd, SW_SHOW);

    InitGL();
    ReSizeGLScene(1024, 720);

    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            HandleMovement();
            DrawGLScene();
        }
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);

    return 0;
}