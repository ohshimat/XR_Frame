#include "pch.h"
#include "VRFLTool.h"

namespace VRFL
{
	bool GetFullPath(const char* filename, char* dstbuf, int bufsize, const char* relativebase)
	{
		if (filename == NULL || strnlen(filename, bufsize) == 0) return false;

		if (!PathIsRelative(filename))
		{
			_snprintf_s(dstbuf, bufsize, _TRUNCATE, "%s", filename);
		}
		else
		{
			if (relativebase != NULL)
			{
				_snprintf_s(dstbuf, bufsize, _TRUNCATE, "%s%s", relativebase, filename);
			}
			else
			{
				char mpath[MAX_PATH + 1];
				char drive[_MAX_DRIVE + 1];
				char dir[_MAX_DIR + 1];
				char fname[_MAX_FNAME + 1];
				char ext[_MAX_EXT + 1];

				if (GetModuleFileName(NULL, mpath, MAX_PATH) == 0) return false;

				_splitpath(mpath, drive, dir, fname, ext);

				_snprintf_s(dstbuf, bufsize, _TRUNCATE, "%s%s%s", drive, dir, filename);
			}
		}

		return true;
	}
}