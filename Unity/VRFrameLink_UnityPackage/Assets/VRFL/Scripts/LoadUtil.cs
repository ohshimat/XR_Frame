using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class LoadUtil
{
    public static Vector3[] ConvertFloatToVector3(float[] src, int size)
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

    public static Vector2[] ConvertFloatToVector2(float[] src, int size)
    {
        Vector2[] dst = new Vector2[size];

        for (int i = 0; i < size; i++)
        {
            dst[i].x = src[i * 2 + 0];
            dst[i].y = src[i * 2 + 1];
        }

        return dst;
    }

    public static int[] CreateIndices(int size, MeshTopology topology, bool reverse)
    {
        int[] indices = new int[size];

        if (reverse)
        {
            if (topology == MeshTopology.Quads)
            {
                for (int i = 0; i < size; i += 4)
                {
                    indices[i + 0] = i + 0;
                    indices[i + 1] = i + 3;
                    indices[i + 2] = i + 2;
                    indices[i + 3] = i + 1;
                }
            }
            else if (topology == MeshTopology.Triangles)
            {
                for (int i = 0; i < size; i += 3)
                {
                    indices[i + 0] = i + 0;
                    indices[i + 1] = i + 2;
                    indices[i + 2] = i + 1;
                }
            }
            else
            {
                // fallback
                for (int i = 0; i < size; i++) indices[i] = i;
            }
        }
        else
        {
            for (int i = 0; i < size; i++) indices[i] = i;
        }

        return indices;
    }
}
