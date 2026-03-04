#pragma once
#include <string>
#include <vector>

struct vrconfigdata
{
	int id;
	float scale;
	std::string node;
	std::string file;
	std::string reserve;
};

class vrconfig
{
public:
	static std::vector<vrconfigdata> m_datalist;

public:
	static bool LoadConfig(const char* filename);

	static int GetID(const char* nodename);
	static const char* GetFileName(const char* nodename);
	static const char* GetFileName(int id);
	static float GetScale(const char* nodename);
};