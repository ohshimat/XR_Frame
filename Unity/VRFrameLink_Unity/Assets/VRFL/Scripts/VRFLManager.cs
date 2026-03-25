using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class VRFLManager : MonoBehaviour
{
    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool InitializeUnity(ushort port);

    // Dllの文字列はマルチバイト(CharSet.Ansi)
    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern int GetDataCount();

    [DllImport("VRFrameLink_DLL.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern VRFLRecvData GetData();

    [SerializeField] private ushort ReceivePort; // 受信ポート、Sim側と数値を合わせる
    [SerializeField] private string ConfigFile;

    [SerializeField] private VRFLModelManager m_modelManager;

    private VRFLConfigFile m_config = new VRFLConfigFile();

    // Start is called before the first frame update
    void Start()
    {
        Initialize();
    }

    // Update is called once per frame
    void Update()
    {
        VRFLRecvData rd;

        // 受信したデータをすべて読み切る
        while(GetDataCount() > 0)
        {
            rd = GetData();
            m_modelManager.UpdateModel(rd);
        }
    }

    void Initialize()
    {
        InitializeUnity(ReceivePort); // VRFL初期設定、受信ポートのバインド

        m_config.LoadConfigFile(ConfigFile); // 設定ファイルの読込

        m_modelManager.CreateModels(m_config); // 設定ファイルに基づいたモデルデータの読込
    }


}
