using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GroundGrid : MonoBehaviour
{
    public GameObject ParentObject;

    [Header("Line Grid")]
    public Material LineMaterial;
    public float LineGridLength = 100.0f;
    public float LineGridStep = 5.0f;
    public Color LineGridColor = Color.white;

    [Header("Cross Grid")]
    public Material CrossMaterial;
    public float CrossGridLength = 0.2f;
    public Color CrossGridColor = Color.white;

    private GameObject lineobj;

    // Start is called before the first frame update
    void Start()
    {
        // line grid
        //GameObject lineobj = new GameObject();
        lineobj = new GameObject();
        MeshFilter mf = lineobj.AddComponent<MeshFilter>();
        mf.sharedMesh = makeLineGrid();
        MeshRenderer mr = lineobj.AddComponent<MeshRenderer>();
        mr.sharedMaterial = LineMaterial;

        lineobj.transform.parent = ParentObject.transform;


    }

    public void SetGridHeight(float height)
    {
        Vector3 pos = Vector3.zero;
        pos.y = height;
        lineobj.transform.position = pos;
    }

    private Mesh makeLineGrid()
    {
        int bid, id;
        bid = (int)(LineGridLength / LineGridStep);

        int pointcnt = (bid * 2 + 1) * 2 * 2;

        int[] indices = new int[pointcnt];
        Vector3[] lines = new Vector3[pointcnt];

        // z-lines
        for (int i = -bid; i <= bid; i++)
        {
            id = i + bid;
            lines[id * 2 + 0].x = i * LineGridStep;
            lines[id * 2 + 0].z = -LineGridLength;
            lines[id * 2 + 1].x = i * LineGridStep;
            lines[id * 2 + 1].z = LineGridLength;
        }

        // x-lines
        int baseid = pointcnt / 2;
        for (int i = -bid; i <= bid; i++)
        {
            id = i + bid;
            lines[baseid + id * 2 + 0].x = -LineGridLength;
            lines[baseid + id * 2 + 0].z = i * LineGridStep;
            lines[baseid + id * 2 + 1].x = LineGridLength;
            lines[baseid + id * 2 + 1].z = i * LineGridStep;
        }

        for (int i = 0; i < pointcnt; i++)
            indices[i] = i;

        Mesh m = new Mesh();

        m.SetVertices(lines);
        m.SetIndices(indices, MeshTopology.Lines, 0);

        return m;
    }

    //private Mesh makeCrossGrid()
    //{

    //}
}
