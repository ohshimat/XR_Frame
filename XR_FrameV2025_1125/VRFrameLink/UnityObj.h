#pragma once

void u_objInit();
void u_objCleanUp();
bool u_objCreateModel(char* filename, double scale);
void u_objDeleteModel();
int u_objMaterialCount();

int u_objArrayCount(int materialID);
bool u_objArrayInfo(int materialID, float* vertex, float* normal, float* uv);

bool u_objMaterialInfo(int materialID,
    int* hasAmbient, float* ambient,
    int* hasDiffuse, float* diffuse,
    int* hasSpecular, float* specular,
    int* hasTransparency, float* transparency,
    int* hasShininess, int* shininess,
    int* hasIllumination, int* illumination,
    int* hasTexture, int* textureID);
