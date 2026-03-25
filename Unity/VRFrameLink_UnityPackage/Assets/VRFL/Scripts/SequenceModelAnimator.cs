using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SequenceModelAnimator : MonoBehaviour
{
    private SequenceModelInfo modelInfo;
    private List<GameObject> frames;

    private int currentFrameIndex;
    private float elapsedTime;

    public void Initialize(SequenceModelInfo info, List<GameObject> frames)
    {
        this.modelInfo = info;
        this.frames = frames;
        this.currentFrameIndex = 0;
        this.elapsedTime = 0.0f;

        for (int i = 0; i < frames.Count; i++)
        {
            frames[i].SetActive(i == 0);
        }
    }

    private void Update()
    {
        if (frames == null || frames.Count == 0) return;

        elapsedTime += Time.deltaTime;
        int currentFrame = (int)(elapsedTime * modelInfo.FPS) % frames.Count;
        if (currentFrame != currentFrameIndex)
        {
            frames[currentFrameIndex].SetActive(false);
            frames[currentFrame].SetActive(true);
            currentFrameIndex = currentFrame;
        }
    }
}
