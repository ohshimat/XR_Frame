using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Rendering;

public class LoadObj
{
    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void OBJInit();
    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void OBJCleanUp();

    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool OBJCreateModel(string filename, double scale);
    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void OBJDeleteModel();

    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int OBJMaterialCount();

    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool OBJMaterialInfo(int materialID,
            out int hasAmbient, [Out] float[] ambient,
            out int hasDiffuse, [Out] float[] diffuse,
            out int hasSpecular, [Out] float[] specular,
            out int hasTransparency, out float transparency,
            out int hasShininess, out int shininess,
            out int hasIllumination, out int illumination,
            out int hasTexture, out int textureID,
            out int hasSphereTexture, out int sphereTextureID,
            out int hasCubeTexture, [Out] int[] cubeTextureIDs);

    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int OBJArrayCount(int materialID);
    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool OBJArrayInfo(int materialID,
            [Out] float[] vertex, [Out] float[] normal, [Out] float[] uv);

    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool OBJTextureBufferInfo(int textureID,
            out int width, out int height,
            out int channels, out int bufferSize);
    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool OBJTextureBufferImage(int textureID, [Out] byte[] image, int bufferSize);

    public static void Initialize()
    {
        OBJInit();
    }

    public static void CleanUp()
    {
        OBJCleanUp();
    }

    public static GameObject LoadObjFile(string filename, float scale, Material basemat, Dictionary<string, string> attributes)
    {
        GameObject obj = new();

        if (IsSequence(attributes))
        {
            SequenceModelInfo sequenceInfo = SequenceModelInfo.Parse(attributes);
            List<GameObject> frames = new();

            SequenceModelAnimator animator = obj.AddComponent<SequenceModelAnimator>();
            animator.Initialize(sequenceInfo, frames);

            List<Material> cachedMaterials = null;

            int firstFrame = 1;
            for (int i = firstFrame; i <= sequenceInfo.FrameCount; i++)
            {
                GameObject frameObj = new("Frame_" + i);
                frameObj.transform.parent = obj.transform;
                frames.Add(frameObj);
                string frameFileName = sPrintf(filename, i);

                if (!OBJCreateModel(frameFileName, scale))
                {
                    Debug.LogError("Failed to load OBJ file: " + frameFileName);
                    continue;
                }

                if (i == firstFrame)
                {
                    cachedMaterials = LoadMaterials(basemat);
                }

                List<Material> materials;
                if (sequenceInfo.ReuseMaterial)
                {
                    materials = cachedMaterials;
                }
                else
                {
                    materials = LoadMaterials(basemat);
                }
                LoadMesh(frameObj, materials);

                OBJDeleteModel();
            }
        }
        else
        {
            if (!OBJCreateModel(filename, scale))
            {
                Debug.LogError("Failed to load OBJ file: " + filename);
                return obj;
            }

            List<Material> materials = LoadMaterials(basemat);
            LoadMesh(obj, materials);

            OBJDeleteModel();
        }

        return obj;
    }

    private static void LoadMesh(GameObject parent, List<Material> materials)
    {
        int cnt = OBJMaterialCount();
        for (int i = 0; i < cnt; i++)
        {
            int arraysize = OBJArrayCount(i);
            if (arraysize <= 0) continue;

            float[] vert = new float[arraysize * 3];
            float[] norm = new float[arraysize * 3];
            float[] uv = new float[arraysize * 2];

            if (OBJArrayInfo(i, vert, norm, uv))
            {
                GameObject meshobj = new("Mesh_" + i);
                MeshFilter mf = meshobj.AddComponent<MeshFilter>();
                MeshRenderer mr = meshobj.AddComponent<MeshRenderer>();
                Mesh mesh = mf.mesh;
                if (mesh == null) mesh = new Mesh();
                mesh.indexFormat = arraysize > 65535 ? IndexFormat.UInt32 : IndexFormat.UInt16;

                Vector3[] vvec, vnorm;
                Vector2[] vuv;

                vvec = LoadUtil.ConvertFloatToVector3(vert, arraysize);
                vnorm = LoadUtil.ConvertFloatToVector3(norm, arraysize);

                mesh.vertices = vvec;
                mesh.normals = vnorm;

                vuv = LoadUtil.ConvertFloatToVector2(uv, arraysize);
                mesh.uv = vuv;

                int[] indices = LoadUtil.CreateIndices(arraysize, MeshTopology.Triangles, reverse: true);
                mesh.SetIndices(indices, MeshTopology.Triangles, 0);
                mesh.RecalculateBounds();

                mr.material = materials[i];
                mf.mesh = mesh;
                meshobj.transform.parent = parent.transform;
            }
        }
    }

    private static List<Material> LoadMaterials(Material basemat)
    {
        int cnt = OBJMaterialCount();
        List<Material> materials = new(cnt);
        Dictionary<int, Texture2D> textureList = new();
        Dictionary<string, Cubemap> cubemapList = new();

        for (int i = 0; i < cnt; i++)
        {
            Material mat = new(basemat)
            {
                name = "Material_" + i
            };
            mat.SetFloat("_SphereBlend", 0.0f);
            mat.SetFloat("_CubeBlend", 0.0f);
            materials.Add(mat);

            float[] ambient = new float[3];
            float[] diffuse = new float[3];
            float[] specular = new float[3];
            int[] cubeTextureIDs = new int[6];
            if (OBJMaterialInfo(i,
                    out int hasAmbient, ambient,
                    out int hasDiffuse, diffuse,
                    out int hasSpecular, specular,
                    out int hasTransparency, out float transparency,
                    out int hasShininess, out int shininess,
                    out int hasIllumination, out int illumination,
                    out int hasTexture, out int textureID,
                    out int hasSphereTexture, out int sphereTextureID,
                    out int hasCubeTexture, cubeTextureIDs))
            {
                if (hasAmbient != 0) mat.SetColor("_AmbientColor", new Color(ambient[0], ambient[1], ambient[2]));
                if (hasDiffuse != 0) mat.color = new Color(diffuse[0], diffuse[1], diffuse[2]);
                if (hasSpecular != 0) mat.SetColor("_SpecularColor", new Color(specular[0], specular[1], specular[2]));
                if (hasTransparency != 0) mat.SetFloat("_Transparency", transparency);
                if (hasShininess != 0) mat.SetFloat("_Shininess", shininess);
                if (hasIllumination != 0) mat.SetInt("_Illumination", illumination);
                if (hasTexture != 0)
                {
                    Texture2D tex = GetOrLoadTexture(textureList, textureID);
                    if (tex != null) mat.mainTexture = tex;
                }

                if (hasSphereTexture != 0)
                {
                    Texture2D sphereTex = GetOrLoadTexture(textureList, sphereTextureID);
                    if (sphereTex != null)
                    {
                        mat.SetTexture("_SphereTex", sphereTex);
                        mat.SetFloat("_SphereBlend", 0.35f);
                    }
                }

                if (hasCubeTexture != 0)
                {
                    Cubemap cubeTex = GetOrLoadCubemap(cubemapList, textureList, cubeTextureIDs);
                    if (cubeTex != null)
                    {
                        mat.SetTexture("_CubeTex", cubeTex);
                        mat.SetFloat("_CubeBlend", 0.35f);
                    }
                }

                ConfigureBlendMode(mat, hasTransparency != 0 ? transparency : 1.0f);
            }
        }

        return materials;
    }

    private static void ConfigureBlendMode(Material mat, float transparency)
    {
        // Keep opaque as default so depth sorting is stable for most OBJ materials.
        bool isTransparent = transparency < 0.999f;

        if (isTransparent)
        {
            mat.SetInt("_SrcBlend", (int)BlendMode.SrcAlpha);
            mat.SetInt("_DstBlend", (int)BlendMode.OneMinusSrcAlpha);
            mat.SetInt("_ZWrite", 0);
            mat.SetOverrideTag("RenderType", "Transparent");
            mat.renderQueue = (int)RenderQueue.Transparent;
        }
        else
        {
            mat.SetInt("_SrcBlend", (int)BlendMode.One);
            mat.SetInt("_DstBlend", (int)BlendMode.Zero);
            mat.SetInt("_ZWrite", 1);
            mat.SetOverrideTag("RenderType", "Opaque");
            mat.renderQueue = (int)RenderQueue.Geometry;
        }
    }

    private static Texture2D LoadTexture(int textureID)
    {
        if (!OBJTextureBufferInfo(textureID,
                out int width, out int height,
                out int channels, out int bufferSize)) return null;

        if (width <= 0 || height <= 0 || bufferSize <= 0) return null;

        TextureFormat format;
        int bytesPerPixel;
        if (channels == 3)
        {
            format = TextureFormat.RGB24;
            bytesPerPixel = 3;
        }
        else if (channels == 4)
        {
            format = TextureFormat.RGBA32;
            bytesPerPixel = 4;
        }
        else
        {
            Debug.LogWarning("Unsupported OBJ texture channels: " + channels + " (textureID=" + textureID + ")");
            return null;
        }

        int expectedSize = width * height * bytesPerPixel;
        if (bufferSize != expectedSize)
        {
            Debug.LogWarning("Unexpected OBJ texture buffer size: expected=" + expectedSize + ", actual=" + bufferSize + " (textureID=" + textureID + ")");
            return null;
        }

        byte[] image = new byte[bufferSize];
        if (!OBJTextureBufferImage(textureID, image, bufferSize)) return null;

        Texture2D tex = new Texture2D(width, height, format, false);
        tex.name = "OBJTexture_" + textureID;
        tex.LoadRawTextureData(image);
        tex.Apply(false, false);

        return tex;
    }

    private static Texture2D GetOrLoadTexture(Dictionary<int, Texture2D> textureList, int textureID)
    {
        if (textureID < 0) return null;

        if (textureList.TryGetValue(textureID, out Texture2D tex)) return tex;

        tex = LoadTexture(textureID);
        if (tex != null) textureList[textureID] = tex;
        return tex;
    }

    private static Cubemap GetOrLoadCubemap(Dictionary<string, Cubemap> cubemapList, Dictionary<int, Texture2D> textureList, int[] cubeTextureIDs)
    {
        if (cubeTextureIDs == null || cubeTextureIDs.Length < 6) return null;

        string cubeKey = string.Join("_", cubeTextureIDs);
        if (cubemapList.TryGetValue(cubeKey, out Cubemap cached)) return cached;

        Texture2D[] faces = new Texture2D[6];
        for (int i = 0; i < 6; i++)
        {
            faces[i] = GetOrLoadTexture(textureList, cubeTextureIDs[i]);
            if (faces[i] == null) return null;
        }

        int size = faces[0].width;
        TextureFormat format = faces[0].format;
        for (int i = 0; i < 6; i++)
        {
            if (faces[i].width != size || faces[i].height != size)
            {
                Debug.LogWarning("Cube texture face size mismatch.");
                return null;
            }
        }

        Cubemap cubemap = new Cubemap(size, format, false)
        {
            name = "OBJCubemap_" + cubeKey
        };

        for (int i = 0; i < 6; i++)
        {
            cubemap.SetPixels(faces[i].GetPixels(), (CubemapFace)i);
        }
        cubemap.Apply(false, false);

        cubemapList[cubeKey] = cubemap;
        return cubemap;
    }

    private static bool IsSequence(Dictionary<string, string> attributes)
    {
        if (attributes.TryGetValue("Sequence", out string sequenceValue))
        {
            return sequenceValue == "true" || sequenceValue == "1";
        }
        return false;
    }

    private static string sPrintf(string format, int index)
    {
        return format.Replace("%i", index.ToString());
    }
}
