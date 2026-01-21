#ifndef CAMERA_H
#define CAMERA_H

struct Vector3dStruct
{
    float x, y, z;
};

Vector3dStruct Vector3dCreate(float x, float y, float z);
Vector3dStruct Vector3dAdd(Vector3dStruct a, Vector3dStruct b);
Vector3dStruct Vector3dScale(Vector3dStruct v, float s);
Vector3dStruct Vector3dNormalize(Vector3dStruct v);

class Camera
{
public:
    Vector3dStruct Position;
    Vector3dStruct View;
    Vector3dStruct Up;

    float Yaw;      // Left-right rotation
    float Pitch;    // Gentle vertical tilt

    Camera();

    void Render();
    void UpdateViewVector();

    void RotateYaw(float angle);
    void RotatePitch(float angle);

    void MoveForward(float d);
    void MoveRight(float d);
    void MoveUpward(float d);
};

#endif
