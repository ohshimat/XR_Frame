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
		std::vector<std::pair<std::string, std::string>> Attributes;
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
		static void parseAttributes(std::string attrs, CSVInfo* info);

	public:
		static bool LoadCSV(const char* fname, const char* relativebase);

		static int  GetConfigCount();
		static int  GetMaxStringLength();
		static bool GetConfig(int index, int* id, float* scale, char* node, char* file, int charsize);
		static int  GetConfigAttributeCount(int index);
		static bool GetConfigAttribute(int index, int attrIndex, char *attrName, char *attrValue, int charsize);
	};
}

