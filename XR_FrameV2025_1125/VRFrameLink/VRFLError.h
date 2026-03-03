#pragma once

#include <iostream>
#include <memory>

#include "syncobject.h"

namespace VRFL
{
	class VRFLError
	{
	private:
		static std::shared_ptr<std::ostream> _outstream;

	private: 
		VRFLError();
		~VRFLError();

		static std::string formatMsg(const char* msg);

		static SyncInterlock _outlock;

	public:
		static void SetOutputStream(std::shared_ptr<std::ostream> outstream);

		static void ErrorMessage(const char* msg);
		//static void ErrorMessage(std::string msg);
	};
}