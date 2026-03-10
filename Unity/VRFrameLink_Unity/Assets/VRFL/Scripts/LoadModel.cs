using UnityEngine;

public class LoadModel
{
    public static void Initialize()
    {
        LoadMqo.Initialize();
        LoadObj.Initialize();
    }

    public static void CleanUp()
    {
        LoadMqo.CleanUp();
        LoadObj.CleanUp();
    }

    public static GameObject LoadModelFile(string filename, float scale, Material basemat)
    {
        if (filename.EndsWith(".mqo"))
        {
            return LoadMqo.LoadMqoFile(filename, scale, basemat);
        }
        else if (filename.EndsWith(".obj"))
        {
            return LoadObj.LoadObjFile(filename, scale, basemat);
        }
        else
        {
            Debug.LogError("Unsupported file format: " + filename);
            // サポートしていないファイル形式の場合は、キューブを返す
            return GameObject.CreatePrimitive(PrimitiveType.Cube);
        }
    }
}
