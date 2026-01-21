#ifndef MODEL_3DS_H
#define MODEL_3DS_H

#include "3DTexture.h"
#include <stdio.h>

class Model_3DS  
{
public:

    struct Vector { float x, y, z; };
    struct Vertex { float x, y, z; };
    struct Color  { float r, g, b, a; };

    struct Material {
        char name[80];
        GLTexture tex;
        bool textured;
        Color ambient, diffuse, specular;
    };

    struct ChunkHeader {
        unsigned short id;
        unsigned long  len;
    };

    struct MaterialFaces {
        unsigned short* subFaces;
        int numSubFaces;
        int MatIndex;
    };

    struct Object {
        char name[80];
        float* Vertexes;
        float* Normals;
        float* TexCoords;
        unsigned short* Faces;
        int numFaces;
        int numMatFaces;
        int numVerts;
        int numTexCoords;
        bool textured;
        MaterialFaces* MatFaces;

        Vector pos;
        Vector rot;
    };

    struct Vec3 { float x, y, z; };
    Vec3 center;

    char* modelname;
    char* path;

    int numObjects;
    int numMaterials;
    int totalVerts;
    int totalFaces;

    bool shownormals;
    bool lit;
    bool visible;

    Material* Materials;
    Object* Objects;

    Vector pos;
    Vector rot;
    float scale;

    bool textured;

    Model_3DS();
    ~Model_3DS();

    void Load(char* name);
    void Draw();

private:
    FILE* bin3ds;

    void MainChunkProcessor(long length, long findex);
    void EditChunkProcessor(long length, long findex);

    void MaterialChunkProcessor(long length, long findex, int matindex);
    void MaterialNameChunkProcessor(long length, long findex, int matindex);
    void AmbientColorChunkProcessor(long length, long findex, int matindex);
    void DiffuseColorChunkProcessor(long length, long findex, int matindex);
    void SpecularColorChunkProcessor(long length, long findex, int matindex);
    void TextureMapChunkProcessor(long length, long findex, int matindex);
    void MapNameChunkProcessor(long length, long findex, int matindex);

    void ObjectChunkProcessor(long length, long findex, int objindex);
    void TriangularMeshChunkProcessor(long length, long findex, int objindex);
    void VertexListChunkProcessor(long length, long findex, int objindex);
    void TexCoordsChunkProcessor(long length, long findex, int objindex);
    void FacesDescriptionChunkProcessor(long length, long findex, int objindex);
    void FacesMaterialsListChunkProcessor(long length, long findex, int objindex, int subfacesindex);

    void FloatColorChunkProcessor(long length, long findex, Color& c);
    void IntColorChunkProcessor(long length, long findex, Color& c);

    void CalculateNormals();
};

#endif
