using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class LoadMqo {

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void MQOInit();
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern void MQOCleanUp();

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern bool MQOCreateModel(string filename, double scale);
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern void MQODeleteModel();

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern int MQOObjectCount();
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern int MQOMaterialCount(int objectID);
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern bool MQOMaterialInfo(int objectID, int materialID,
			out int isvalid, out int hastexture,
			[Out] float[] dif, [Out] float[] emi, [Out] float[] spe,
			out int textureID
		);
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern int MQOArrayCount(int objectID, int materialID);
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern bool MQOArrayInfo(int objectID, int materialID,
			[Out] float[] vertex, [Out] float[] normal, [Out] float[] uv);

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int MQOTextureSize(int textureID);
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool MQOTextureImage(int textureID, [Out] byte[] image);


    private static Dictionary<int, Texture2D> textureList = new Dictionary<int, Texture2D>();

    public static void Initialize()
    {
        MQOInit();
    }
    public static void CleanUp()
    {
        // テクスチャ情報の削除
        MQOCleanUp();
    }
    public static GameObject LoadMqoFile(string filename, float scale, Material basemat)
    {
        GameObject obj = new GameObject();

        if(!MQOCreateModel(filename, scale)) return null;

        LoadObjects(obj, basemat);

        // モデル情報の削除
        MQODeleteModel();

        return obj;
    }

    public static void LoadObjects(GameObject obj, Material basemat)
    {
        int cnt = MQOObjectCount();

        for (int i = 0; i < cnt; i++)
        {
            int mcnt = MQOMaterialCount(i);

            for (int j = 0; j < mcnt; j++)
            {
                int isvalid, hastexture;
                float[] dif = new float[4];
                float[] emi = new float[4];
                float[] spc = new float[4];
                int textureID;

                Material mat = new Material(basemat);

                if(MQOMaterialInfo(i,j,
                    out isvalid, out hastexture,
                    dif, emi, spc,
                    out textureID
                ))
                {
                    if(isvalid != 0)
                    {
                        if(hastexture != 0)
                        {
                            if(!textureList.ContainsKey(textureID)) LoadTexture(textureID);
                            mat.mainTexture = textureList[textureID];
                        }

                        mat.color = new Color(dif[0], dif[1], dif[2], dif[3]);
                      
                    }
                }

                int arraysize = MQOArrayCount(i, j);
                if (arraysize > 0)
                {
                    float[] vert = new float[arraysize * 3];
                    float[] norm = new float[arraysize * 3];
                    float[] uv = new float[arraysize * 2];

                    if(MQOArrayInfo(i,j,vert,norm,uv))
                    {
                        GameObject meshobj = new GameObject();
                        meshobj.name = "Mesh_" + i.ToString();

                        MeshFilter mf = meshobj.AddComponent<MeshFilter>();
                        MeshRenderer mr = meshobj.AddComponent<MeshRenderer>();

                        Mesh mesh = mf.mesh;
                        if (mesh == null) mesh = new Mesh();

                        Vector3[] vvec, vnorm;
                        Vector2[] vuv;

                        vvec = convertFloatToVector3(vert, arraysize);
                        vnorm = convertFloatToVector3(norm, arraysize);

                        mesh.vertices = vvec;
                        mesh.normals = vnorm;

                        if(isvalid!= 0 && hastexture != 0)
                        {
                            vuv = convertFloatToVector2(uv, arraysize);
                            mesh.uv = vuv;
                        }
                        
                        int[] indices = createIndices(arraysize);

                        mesh.SetIndices(indices, MeshTopology.Triangles, 0);
                        mesh.RecalculateBounds();

                        if(isvalid != 0) mr.material = mat;

                        mf.mesh = mesh;

                        meshobj.transform.parent = obj.transform;
                    }
                }
            }
        }
    }

    public static void LoadTexture(int textureID)
    {
        if(textureList.ContainsKey(textureID)) return;

        int texsize = MQOTextureSize(textureID);

        if(texsize != 0)
        {
            byte[] image = new byte[texsize * texsize * 4]; // w * h * rgba

            if(MQOTextureImage(textureID, image))
            {
                Texture2D tex = new Texture2D(texsize, texsize, TextureFormat.RGBA32, false);

                Color32[] cols = new Color32[texsize * texsize];

                for (int i = 0; i < texsize * texsize; i++)
                {
                    cols[i].r = image[i * 4 + 0];
                    cols[i].g = image[i * 4 + 1];
                    cols[i].b = image[i * 4 + 2];
                    cols[i].a = image[i * 4 + 3];
                }

                tex.SetPixels32(cols);
                tex.Apply();

                textureList.Add(textureID, tex);
            }
        }
    }

    public static int[] createIndices(int size)
    {
        int[] indices = new int[size];

        for(int i=0; i<size; i++) indices[i] = i;

        return indices;
    }

    private static Vector3[] convertFloatToVector3(float[] src, int size)
    {
        Vector3[] dst = new Vector3[size];

        for (int i = 0; i < size; i++)
        {
            dst[i].x = src[i * 3 + 0];
            dst[i].y = src[i * 3 + 1];
            dst[i].z = -src[i * 3 + 2]; // 右手系 Y-up → 左手系 Y-up
        }

        return dst;
    }

        private static Vector2[] convertFloatToVector2(float[] src, int size)
    {
        Vector2[] dst = new Vector2[size];

        for (int i = 0; i < size; i++)
        {
            dst[i].x = src[i * 2 + 0];
            dst[i].y = src[i * 2 + 1];
        }

        return dst;
    }
}
