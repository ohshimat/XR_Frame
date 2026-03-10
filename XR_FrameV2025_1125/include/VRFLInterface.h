#pragma once

#ifdef VRFRAMELINK_EXPORTS
#define VRFL_DECLSPEC   __declspec(dllexport)
#else
#define VRFL_DECLSPEC    __declspec(dllimport)
#endif

// VRFrame Interface functions
namespace VRFL
{
#pragma pack(push, 4)
	struct ObjectData
	{
	public:
		int id;
		int state;
		int visible;
		float pos[3];
		float rot[4]; // quaternion
		float col[4]; // color (0.0 - 1.0)
	};
#pragma pack(pop)

	extern "C"
	{	
		//---------------------------------------------------------------------------
		// Common
		//---------------------------------------------------------------------------
		// Config(CSV)
		VRFL_DECLSPEC bool LoadConfig(const char* filename, const char* relativebase); // MultiByte
		VRFL_DECLSPEC int  GetConfigCount();
		VRFL_DECLSPEC int  GetMaxStringLength();
		VRFL_DECLSPEC bool GetConfig(int index, int* id, float* scale, char* node, char* file, char* reserve, int charsize);


		//---------------------------------------------------------------------------
		// For Sim
		//---------------------------------------------------------------------------
		// Initialize, Release
		VRFL_DECLSPEC bool InitializeSim(unsigned short remoteport);

		// Data Transfer
		VRFL_DECLSPEC void Send(void** pObjDataT, int length);


		//---------------------------------------------------------------------------
		// For Unity 
		//---------------------------------------------------------------------------
		// Initialize, Release
		VRFL_DECLSPEC bool InitializeUnity(unsigned short port);


		// Data Transfer
		VRFL_DECLSPEC int GetDataCount();
		VRFL_DECLSPEC ObjectData GetData();

		// MQO
		VRFL_DECLSPEC void MQOInit();
		VRFL_DECLSPEC void MQOCleanUp();

		VRFL_DECLSPEC bool MQOCreateModel(char* filename, double scale);
		VRFL_DECLSPEC void MQODeleteModel();

		VRFL_DECLSPEC int MQOObjectCount();
		VRFL_DECLSPEC int MQOMaterialCount(int objectID);
		VRFL_DECLSPEC bool MQOMaterialInfo(int objectID, int materialID,
			int* isvalid, int* hastexture,
			float* dif, float* emi, float* spe,
			int* textureID
		);

		VRFL_DECLSPEC int MQOArrayCount(int objectID, int materialID);
		VRFL_DECLSPEC bool MQOArrayInfo(int objectID, int materialID,
			float* vertex, float* normal, float* uv);

		VRFL_DECLSPEC int MQOTextureSize(int textureID);
		VRFL_DECLSPEC bool MQOTextureImage(int textureID, unsigned char* image);

		// OBJ
		VRFL_DECLSPEC void OBJInit();
		VRFL_DECLSPEC void OBJCleanUp();

		VRFL_DECLSPEC bool OBJCreateModel(char* filename, double scale);
		VRFL_DECLSPEC void OBJDeleteModel();

		VRFL_DECLSPEC int OBJMaterialCount();
		VRFL_DECLSPEC int OBJArrayCount(int materialID);
		VRFL_DECLSPEC bool OBJArrayInfo(int materialID, float* vertex, float* normal, float* uv);

		VRFL_DECLSPEC bool OBJMaterialInfo(int materialID,
			int* hasAmbient, float* ambient,
			int* hasDiffuse, float* diffuse,
			int* hasSpecular, float* specular,
			int* hasTransparency, float* transparency,
			int* hasShininess, int* shininess,
			int* hasIllumination, int* illumination,
			int* hasTexture, int* textureID,
            int* hasSphereTexture, int* sphereTextureID,
            int* hasCubeTexture, int* cubeTextureIDs);

		VRFL_DECLSPEC bool OBJTextureBufferInfo(int textureID,
			int* width, int* height,
			int* channels, int* bufferSize);
		VRFL_DECLSPEC bool OBJTextureBufferImage(int textureID, unsigned char* image, int bufferSize);
	}
}