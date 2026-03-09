using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class NodeObject
{
    public string NodeName;
    public GameObject Object;
}

public class VRFLModelManager : MonoBehaviour
{
    // Start is called before the first frame update
    public Material m_material; // ベースとなるマテリアル

    [SerializeField] private GameObject HeadObject; // head(id = 0)の情報を適用させる対象となるオブジェクト

    [SerializeField] private List<NodeObject> NodeObjectList; // node名に対応するオブジェクトの設定

    private Dictionary<int, VRFLModel> m_models = new Dictionary<int, VRFLModel>();
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public bool CreateModels(VRFLConfigFile config)
    {
        LoadMqo.CleanUp();
        LoadMqo.Initialize();

        // ID:0 は Head
        VRFLModel head = new VRFLModel();
        head.ID = 0;
        head.Materials = new List<Material>();
        if(HeadObject)
        {
            head.Object = HeadObject;
            head.Name = HeadObject.name;
        }
        m_models.Add(head.ID, head);

        foreach(VRFLConfigData cd in config.m_config)
        {
            VRFLModel m = new VRFLModel();

            if(m_models.ContainsKey(cd.ID)) continue; // 既に同IDの物があればスキップ

            m.ID = cd.ID;
            m.Name = cd.NodeName;
            m.Materials = new List<Material>(); // color変更対象のマテリアルリスト

            if (cd.FileName.Length != 0)
            {
                m.Object = LoadModel.LoadModelFile(cd.FileName, cd.Scale, m_material);
                m.Object.name = m.Name;
            }
            else
            {
                foreach(NodeObject no in NodeObjectList)
                {
                    if(no.NodeName == m.Name)
                        m.Object = no.Object;
                }
            }

            m_models.Add(m.ID, m);

            if (m.Object != null)
            {
                // マテリアル検索
                foreach (Transform ch in m.Object.transform)
                {
                    MeshRenderer mr = ch.gameObject.GetComponent<MeshRenderer>();
                    if (mr != null && mr.material != null) m.Materials.Add(mr.material);
                }
            }
        }

        return true;
    }

    public void UpdateModel(VRFLRecvData rd)
    {
        if (m_models.ContainsKey(rd.ID))
        {
            VRFLModel m = m_models[rd.ID];

            if (m.Object == null) return;

            Vector3 pos = new Vector3(rd.Pos[0], rd.Pos[1], -rd.Pos[2]); // 右手系 → 左手系
            Quaternion rot = new Quaternion(rd.Rot[0], rd.Rot[1], -rd.Rot[2], -rd.Rot[3]); // 右手系 → 左手系

            // 表示/非表示の適用
            if (rd.Visible > 0) m.Object.SetActive(true);
            else m.Object.SetActive(false);

            m.Object.transform.position = pos;
            m.Object.transform.rotation = rot;

            // メタセコイアモデルの場合、Sim側では
            // Color適用 → メタセコイアのマテリアルカラー適用
            // と、Colorパラメータでの設定がメタセコイアのマテリアルに上書きされていて表示には適用されていない
            // Unity側でも適用見送り
            /*
            Color col = new Color(rd.Color[0], rd.Color[1], rd.Color[2], rd.Color[3]);
            foreach(Material mat in m.Materials)
            {
                mat.color = col;
            }
            */
        }
    }
}
