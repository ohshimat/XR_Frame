using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
using System.Runtime.InteropServices;
using UnityEngine;

public class VRFLConfigData
{
    public int ID;
    public float Scale;
    public string NodeName;
    public string FileName;
    public string Reserved;
}

public class VRFLConfigFile
{
    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool LoadConfig(string filename, string relativebase);

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern int GetConfigCount();

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern int GetMaxStringLength();

    [DllImport("VRFrameLink.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool GetConfig(int index, ref int id, ref float scale,
                                         StringBuilder node, StringBuilder file, StringBuilder reserve, int stringlen);

    [HideInInspector]
    public List<VRFLConfigData> m_config = new List<VRFLConfigData>();

    public bool LoadConfigFile(string filename)
    {
#if UNITY_EDITOR
        if(!LoadConfig(filename, Application.dataPath + "/../")) return false;
#else
        if(!LoadConfig(filename, null)) return false;
#endif
        int cnt = GetConfigCount();
        int len = GetMaxStringLength();
        int id = 0;
        float scale = 0;

        for (int i = 0; i < cnt; i++)
        {
            StringBuilder sbn = new StringBuilder(len);
            StringBuilder sbf = new StringBuilder(len);
            StringBuilder sbr = new StringBuilder(len);

            if(GetConfig(i, ref id, ref scale, sbn, sbf, sbr, len))
            {
                VRFLConfigData cd = new VRFLConfigData();
                cd.ID = id;
                cd.Scale = scale;
                cd.NodeName = sbn.ToString();
                cd.FileName = sbf.ToString();
                cd.Reserved = sbr.ToString();

                m_config.Add(cd);
            }
        }

        return true;
    }
}
