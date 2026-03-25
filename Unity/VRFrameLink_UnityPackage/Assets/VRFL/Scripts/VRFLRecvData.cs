using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using UnityEngine;

[StructLayout(LayoutKind.Sequential, Pack = 4)]
public struct VRFLRecvData 
{
    public int ID;
    public int State;
    public int Visible;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public float[] Pos;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
    public float[] Rot;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
    public float[] Color;
}
