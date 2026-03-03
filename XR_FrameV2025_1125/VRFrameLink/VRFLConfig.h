#pragma once

#include <vector>
#include <string>

namespace VRFL
{
	struct CSVInfo
	{
	public:
		int ID;
		float Scale;
		std::string NodeName;
		std::string FileName;
		std::string Reserved;
	};

	class VRFLConfig
	{
	private:
		static std::vector< CSVInfo > _infolist;
		static int _maxStringSize;

	private:
		VRFLConfig();
		~VRFLConfig();

		static void parseLine(std::string line, const char* relativebase);

	public:
		static bool LoadCSV(const char* fname, const char* relativebase);

		static int  GetConfigCount();
		static int  GetMaxStringLength();
		static bool GetConfig(int index, int* id, float* scale, char* node, char* file, char* reserve, int charsize);
	};
}

