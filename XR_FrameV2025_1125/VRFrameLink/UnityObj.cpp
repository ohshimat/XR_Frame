#include "pch.h"
#include "UnityObj.h"
#include "UnityObjLoader.h"

static objloader::uObjLoader* uo_loader = NULL;
static double uo_scale = 1.0;

void u_objInit()
{

}

void u_objCleanUp()
{

}

bool u_objCreateModel(char* filename, double scale)
{
    if (uo_loader != NULL) u_objDeleteModel();

    uo_scale = scale;
    uo_loader = new objloader::uObjLoader();
    if (!uo_loader->read(filename))
    {
        u_objDeleteModel();
        return false;
    }

    return true;
}

void u_objDeleteModel()
{
    if (uo_loader != NULL)
    {
        delete uo_loader;
        uo_loader = NULL;
    }

    uo_scale = 1.0;
}

int u_objMaterialCount()
{
    if (uo_loader == NULL) return -1;
    return uo_loader->getMaterialCount();
}

int u_objArrayCount(int materialID)
{
    if (uo_loader == NULL) return -1;

    return uo_loader->getArrayCount(materialID);
}

bool u_objArrayInfo(int materialID, float* vertex, float* normal, float* uv)
{
    if (uo_loader == NULL) return false;

    if (!uo_loader->getArrayInfo(materialID, vertex, normal, uv)) return false;

    if (vertex != NULL && uo_scale != 1.0)
    {
        const int arrayCount = uo_loader->getArrayCount(materialID);
        if (arrayCount > 0)
        {
            const float scale = (float)uo_scale;
            for (int i = 0; i < arrayCount * 3; i++)
            {
                vertex[i] *= scale;
            }
        }
    }

    return true;
}

bool u_objMaterialInfo(int materialID,
    int* hasAmbient, float* ambient,
    int* hasDiffuse, float* diffuse,
    int* hasSpecular, float* specular,
    int* hasTransparency, float* transparency,
    int* hasShininess, int* shininess,
    int* hasIllumination, int* illumination,
    int* hasTexture, int* textureID,
    int* hasSphereTexture, int* sphereTextureID,
    int* hasCubeTexture, int* cubeTextureIDs)
{
    if (uo_loader == NULL) return false;

    return uo_loader->getMaterialInfo(materialID,
        hasAmbient, ambient,
        hasDiffuse, diffuse,
        hasSpecular, specular,
        hasTransparency, transparency,
        hasShininess, shininess,
        hasIllumination, illumination,
        hasTexture, textureID,
        hasSphereTexture, sphereTextureID,
        hasCubeTexture, cubeTextureIDs);
}

bool u_objTextureBufferInfo(int textureID,
    int* width, int* height,
    int* channels, int* bufferSize)
{
    if (uo_loader == NULL) return false;

    return uo_loader->getTextureBufferInfo(textureID,
        width, height,
        channels, bufferSize);
}

bool u_objTextureBufferImage(int textureID, unsigned char* image, int bufferSize)
{
    if (uo_loader == NULL) return false;

    return uo_loader->getTextureBufferImage(textureID, image, bufferSize);
}
