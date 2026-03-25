#include "pch.h"

#include "VRFLInterface.h"

#include "UnityMetaseq.h"

#include "NetworkManager.h"

#include "object.h"

#include "UnityObj.h"

namespace VRFL
{
	// Initialize, Release
	bool InitializeUnity(unsigned short port)
	{
		NetworkManager::InitializeUnity(port);

		return true;
	}

	bool InitializeSim(unsigned short remoteport)
	{
		NetworkManager::InitializeSim(remoteport);
		return false;
	}


	bool LoadConfig(const char* filename, const char* relativebase)
	{
		return VRFLConfig::LoadCSV(filename, relativebase);
	}
	int  GetConfigCount()
	{
		return VRFLConfig::GetConfigCount();
	}
	int  GetMaxStringLength()
	{
		return VRFLConfig::GetMaxStringLength();
	}
	bool GetConfig(int index, int* id, float* scale, char* node, char* file, int charsize)
	{
		return VRFLConfig::GetConfig(index, id, scale, node, file, charsize);
	}
	int GetConfigAttributeCount(int index)
	{
		return VRFLConfig::GetConfigAttributeCount(index);
	}
	bool GetConfigAttribute(int index, int attrIndex, char *attrName, char *attrValue, int charsize)
	{
		return VRFLConfig::GetConfigAttribute(index, attrIndex, attrName, attrValue, charsize);
	}


	// Data Transfer
	int GetDataCount()
	{
		return NetworkManager::GetDataCount();
	}
	ObjectData GetData()
	{
		return NetworkManager::GetData();
	}
	void Send(void** pObjDataT, int length)
	{
		ObjDataT** pdata = (ObjDataT**)pObjDataT;

		NetworkManager::Send(pdata, length);
	}


	// MQO (for Unity)
	void MQOInit() { u_mqoInit(); }
	void MQOCleanUp() { u_mqoCleanup(); }

	bool MQOCreateModel(char* filename, double scale)
	{
		return u_mqoCreateModel(filename, scale);
	}
	void MQODeleteModel()
	{
		u_mqoDeleteModel();
	}

	int MQOObjectCount() { return u_mqoObjectCount(); }
	int MQOMaterialCount(int objectID)
	{
		return u_mqoMaterialCount(objectID);
	}
	bool MQOMaterialInfo(int objectID, int materialID,
		int* isvalid, int* hastexture,
		float* dif, float* emi, float* spe,
		int* textureID
	)
	{
		return u_mqoMaterialInfo(objectID, materialID,
			isvalid, hastexture,
			dif, emi, spe,
			textureID);
	}

	int MQOArrayCount(int objectID, int materialID)
	{
		return u_mqoArrayCount(objectID, materialID);
	}
	bool MQOArrayInfo(int objectID, int materialID,
		float* vertex, float* normal, float* uv)
	{
		return u_mqoArrayInfo(objectID, materialID,
			vertex, normal, uv);
	}

	int MQOTextureSize(int textureID)
	{
		return u_mqoTextureSize(textureID);
	}
	bool MQOTextureImage(int textureID, unsigned char* image)
	{
		return u_mqoTextureImage(textureID, image);
	}

	// OBJ (for Unity)
	void OBJInit()
	{

	}
	void OBJCleanUp()
	{

	}

	bool OBJCreateModel(char* filename, double scale)
	{
		return u_objCreateModel(filename, scale);
	}
	void OBJDeleteModel()
	{
		u_objDeleteModel();
	}

	int OBJMaterialCount()
	{
		return u_objMaterialCount();
	}

	int OBJArrayCount(int materialID)
	{
		return u_objArrayCount(materialID);
	}
	
	bool OBJArrayInfo(int materialID, float* vertex, float* normal, float* uv)
	{
		return u_objArrayInfo(materialID, vertex, normal, uv);
	}

	bool OBJMaterialInfo(int materialID,
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
		return u_objMaterialInfo(materialID,
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

	bool OBJTextureBufferInfo(int textureID,
		int* width, int* height,
		int* channels, int* bufferSize)
	{
		return u_objTextureBufferInfo(textureID,
			width, height,
			channels, bufferSize);
	}

	bool OBJTextureBufferImage(int textureID, unsigned char* image, int bufferSize)
	{
		return u_objTextureBufferImage(textureID, image, bufferSize);
	}
}