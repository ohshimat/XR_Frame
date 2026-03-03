#include "pch.h"
#include "VRFLConfig.h"
#include "VRFLError.h"
#include "VRFLTool.h"

#include <iostream>
#include <fstream>

namespace VRFL
{
	std::vector< CSVInfo > VRFLConfig::_infolist;
	int					   VRFLConfig::_maxStringSize;

	VRFLConfig::VRFLConfig(){}
	VRFLConfig::~VRFLConfig() {}

	bool VRFLConfig::LoadCSV(const char* fname, const char* relativebase)
	{
		std::ifstream ifs;
		std::string line;

		_infolist.clear();
		_maxStringSize = 0;

		try
		{
			char filename[MAX_PATH + 1];

			if (fname == NULL || strnlen(fname, MAX_PATH) == 0)
			{
				if (!VRFL::GetFullPath(".\\VRConfig.csv", filename, MAX_PATH, relativebase)) return false;
			}
			else
			{
				if (!VRFL::GetFullPath(fname, filename, MAX_PATH, relativebase)) return false;
			}

			ifs.open(filename);

			while (std::getline(ifs, line))
			{
				parseLine(line, relativebase);
			}
		}
		catch (std::exception &e)
		{
			std::string msg;
			msg.append("VRFLConfig::LoadCSV, ");
			msg.append((char*)e.what());
			VRFLError::ErrorMessage(msg.c_str());
		
			return false;
		}

		return true;
	}

	int  VRFLConfig::GetConfigCount()
	{
		return (int)_infolist.size();
	}
	int  VRFLConfig::GetMaxStringLength()
	{
		return _maxStringSize;
	}
	bool VRFLConfig::GetConfig(int index, int* id, float* scale, char* node, char* file, char* reserve, int charsize)
	{
		if ((int)_infolist.size() <= index) return false;
		if (charsize <= 0) return false;

		*id = _infolist[index].ID;
		*scale = _infolist[index].Scale;

		if (node != NULL)
		{
			node[0] = 0;
			if(!_infolist[index].NodeName.empty()) sprintf_s(node, charsize, "%s", _infolist[index].NodeName.c_str());
		}
		if (file != NULL)
		{
			file[0] = 0;
			if (!_infolist[index].FileName.empty()) sprintf_s(file, charsize, "%s", _infolist[index].FileName.c_str());
		}
		if (reserve != NULL)
		{
			reserve[0] = 0;
			if (!_infolist[index].Reserved.empty()) sprintf_s(reserve, charsize, "%s", _infolist[index].Reserved.c_str());
		}

		return true;
	}

	void VRFLConfig::parseLine(std::string line, const char* relativebase)
	{
		std::vector<std::string> tokens = SplitString(line, ',');

		CSVInfo info;

		if (tokens.size() >= 4)
		{
			info.ID = atoi(tokens[0].c_str());
			info.NodeName = tokens[1];
			info.Scale = (float)atof(tokens[3].c_str());

			char buff[MAX_PATH + 1];
			VRFL::GetFullPath(tokens[2].c_str(), buff, MAX_PATH, relativebase);

			info.FileName.assign(buff);

			if ((int)info.NodeName.length() >= _maxStringSize) _maxStringSize = (int)info.NodeName.length() + 1;
			if ((int)info.FileName.length() >= _maxStringSize) _maxStringSize = (int)info.FileName.length() + 1;

			if (tokens.size() >= 5)
			{
				info.Reserved = tokens[4];
				if ((int)info.Reserved.length() >= _maxStringSize) _maxStringSize = (int)info.Reserved.length() + 1;
			}

		}

		_infolist.push_back(info);
	}
}