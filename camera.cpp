#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include "camera.h"

#define PI 3.14159f
float toRad(float a) { return a * PI / 180.0f; }

Vector3dStruct Vector3dCreate(float x, float y, float z)
{
    Vector3dStruct v = { x, y, z };
    return v;
}

Vector3dStruct Vector3dAdd(Vector3dStruct a, Vector3dStruct b)
{
    return Vector3dCreate(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3dStruct Vector3dScale(Vector3dStruct v, float s)
{
    return Vector3dCreate(v.x * s, v.y * s, v.z * s);
}

Vector3dStruct Vector3dNormalize(Vector3dStruct v)
{
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len == 0) return v;
    return Vector3dCreate(v.x/len, v.y/len, v.z/len);
}

Camera::Camera()
{
    Position = Vector3dCreate(0, 0, 0);
    View     = Vector3dCreate(0, 0, -1);
    Up       = Vector3dCreate(0, 1, 0);

    Yaw   = 0;
    Pitch = 0;
}

void Camera::UpdateViewVector()
{
    View.x = cos(toRad(Pitch)) * cos(toRad(Yaw));
    View.y = sin(toRad(Pitch));
    View.z = cos(toRad(Pitch)) * -sin(toRad(Yaw));

    View = Vector3dNormalize(View);
}


void Camera::Render()
{
    Vector3dStruct center = Vector3dAdd(Position, View);

    gluLookAt(
        Position.x, Position.y, Position.z,
        center.x,   center.y,   center.z,
        Up.x,       Up.y,       Up.z
    );
}

void Camera::RotateYaw(float angle)
{
    Yaw += angle;
    UpdateViewVector();
}

void Camera::RotatePitch(float angle)
{
    Pitch += angle;

 if (Pitch > 120) Pitch = 120;
if (Pitch < -120) Pitch = -120;


    UpdateViewVector();
}

void Camera::MoveForward(float d)
{
    Vector3dStruct flat = { View.x, 0, View.z };
    flat = Vector3dNormalize(flat);

    Position = Vector3dAdd(Position, Vector3dScale(flat, d));
}

void Camera::MoveRight(float d)
{
    Vector3dStruct right;
    right.x =  View.z;
    right.y =  0;
    right.z = -View.x;

    right = Vector3dNormalize(right);

    Position = Vector3dAdd(Position, Vector3dScale(right, d));
}

void Camera::MoveUpward(float d)
{
    Position.y += d;
}
