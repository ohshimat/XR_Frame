#include "pch.h"
#include "UnityObj.h"
#include "UnityObjLoader.h"

static objloader::uObjLoader* uo_loader = NULL;

void u_objInit()
{

}

void u_objCleanUp()
{

}

bool u_objCreateModel(char* filename, double scale)
{
    if (uo_loader != NULL) u_objDeleteModel();

    uo_loader = new objloader::uObjLoader();
    if (!uo_loader->read(filename)) return false;

    return true;
}

void u_objDeleteModel()
{
    if (uo_loader != NULL)
    {
        delete uo_loader;
        uo_loader = NULL;
    }
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

    return uo_loader->getArrayInfo(materialID, vertex, normal, uv);
}

bool u_objMaterialInfo(int materialID,
    int* hasAmbient, float* ambient,
    int* hasDiffuse, float* diffuse,
    int* hasSpecular, float* specular,
    int* hasTransparency, float* transparency,
    int* hasShininess, int* shininess,
    int* hasIllumination, int* illumination,
    int* hasTexture, int* textureID)
{
    if (uo_loader == NULL) return false;

    // 現状、マテリアル情報は取得できないため、すべての値を「なし」として返す
    *hasAmbient = 0;
    *hasDiffuse = 0;
    *hasSpecular = 0;
    *hasTransparency = 0;
    *hasShininess = 0;
    *hasIllumination = 0;
    *hasTexture = 0;

    return true;
}
