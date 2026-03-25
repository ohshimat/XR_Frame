using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LoadMqoTest : MonoBehaviour
{
    public string filename;

    public Material MqoMaterial;

    // Start is called before the first frame update
    void Start()
    {
        LoadMqo.Initialize();
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyUp(KeyCode.L))
            Load();
    }

    void Load()
    {
        GameObject obj = LoadMqo.LoadMqoFile(filename, 1.0f, MqoMaterial);
    }
}
