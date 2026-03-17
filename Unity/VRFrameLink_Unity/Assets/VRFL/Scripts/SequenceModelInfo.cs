using System.Collections.Generic;

public class SequenceModelInfo
{
    public int FPS { get; set; } = 30;
    public int FrameCount { get; set; } = 1;
    public bool ReuseMaterial { get; set; } = true;

    public static SequenceModelInfo Parse(Dictionary<string, string> attributes)
    {
        SequenceModelInfo info = new();

        if (attributes.TryGetValue("FPS", out string fpsValue) && int.TryParse(fpsValue, out int fps))
        {
            info.FPS = fps;
        }

        if (attributes.TryGetValue("FrameCount", out string frameCountValue) && int.TryParse(frameCountValue, out int frameCount))
        {
            info.FrameCount = frameCount;
        }

        if (attributes.TryGetValue("ReuseMaterial", out string reuseMaterialValue) && bool.TryParse(reuseMaterialValue, out bool reuseMaterial))
        {
            info.ReuseMaterial = reuseMaterial;
        }

        return info;
    }
}
