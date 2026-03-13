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
			if (!ifs.is_open()) return false;

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
	bool VRFLConfig::GetConfig(int index, int* id, float* scale, char* node, char* file, int charsize)
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

		return true;
	}

	int VRFLConfig::GetConfigAttributeCount(int index)
	{
		if ((int)_infolist.size() <= index) return -1;

		return (int)_infolist[index].Attributes.size();
	}

	bool VRFLConfig::GetConfigAttribute(int index, int attrIndex, char *attrName, char *attrValue, int charsize)
	{
		if ((int)_infolist.size() <= index) return false;
		if (charsize <= 0) return false;

		if (_infolist[index].Attributes.empty()) return false;
		if (attrIndex < 0 || (int)_infolist[index].Attributes.size() <= attrIndex) return false;

		if (attrName != NULL)
		{
			attrName[0] = 0;
			sprintf_s(attrName, charsize, "%s", _infolist[index].Attributes[attrIndex].first.c_str());
		}

		if (attrValue != NULL)
		{
			attrValue[0] = 0;
			sprintf_s(attrValue, charsize, "%s", _infolist[index].Attributes[attrIndex].second.c_str());
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
				parseAttributes(tokens[4], &info);
			}

		}

		_infolist.push_back(info);
	}

	void VRFLConfig::parseAttributes(std::string attrs, CSVInfo* info)
	{
		std::vector<std::string> attrtokens = SplitString(attrs, ';');

		for (int i = 0; i < (int)attrtokens.size(); i++)
		{
			std::vector<std::string> kv = SplitString(attrtokens[i], '=');
			if (kv.size() == 2)
			{
				info->Attributes.push_back(std::make_pair(kv[0], kv[1]));
				if ((int)kv[0].length() >= _maxStringSize) _maxStringSize = (int)kv[0].length() + 1;
				if ((int)kv[1].length() >= _maxStringSize) _maxStringSize = (int)kv[1].length() + 1;
			}
		}
	}
}