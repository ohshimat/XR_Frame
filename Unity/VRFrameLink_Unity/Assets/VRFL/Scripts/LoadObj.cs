using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;
using UnityEngine.Rendering;

public class LoadObj
{
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void OBJInit();
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void OBJCleanUp();

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool OBJCreateModel(string filename, double scale);
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void OBJDeleteModel();

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int OBJMaterialCount();

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int OBJArrayCount(int materialID);
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool OBJArrayInfo(int materialID,
            [Out] float[] vertex, [Out] float[] normal, [Out] float[] uv);

    public static GameObject LoadObjFile(string filename, float scale, Material basemat)
    {
        GameObject obj = new GameObject();

        if (!OBJCreateModel(filename, scale))
        {
            Debug.LogError("Failed to load OBJ file: " + filename);
            return null;
        }

        LoadObjects(obj, basemat);

        OBJDeleteModel();

        return obj;
    }

    private static void LoadObjects(GameObject parent, Material basemat)
    {
        int cnt = OBJMaterialCount();

        for (int i = 0; i < cnt; i++)
        {
            int arraysize = OBJArrayCount(i);
            if (arraysize <= 0) continue;

            float[] vert = new float[arraysize * 3];
            float[] norm = new float[arraysize * 3];
            float[] uv = new float[arraysize * 2];

            if (!OBJArrayInfo(i, vert, norm, uv)) continue;

            GameObject meshobj = new GameObject("Mesh_" + i);
            MeshFilter mf = meshobj.AddComponent<MeshFilter>();
            MeshRenderer mr = meshobj.AddComponent<MeshRenderer>();
            Mesh mesh = mf.mesh;
            if (mesh == null)
            {
                mesh = new Mesh();
                mf.mesh = mesh;
            }

            Vector3[] vvec, vnorm;
            Vector2[] vuv;

            vvec = LoadUtil.ConvertFloatToVector3(vert, arraysize);
            vnorm = LoadUtil.ConvertFloatToVector3(norm, arraysize);

            mesh.vertices = vvec;
            mesh.normals = vnorm;

            vuv = LoadUtil.ConvertFloatToVector2(uv, arraysize);
            mesh.uv = vuv;

            // TODO : Triangle か Quad のチェックが必要
            int[] indices = LoadUtil.CreateIndices(arraysize, MeshTopology.Quads, reverse: false);
            mesh.SetIndices(indices, MeshTopology.Quads, 0);
            mesh.RecalculateBounds();

            meshobj.transform.parent = parent.transform;
        }
    }
}
