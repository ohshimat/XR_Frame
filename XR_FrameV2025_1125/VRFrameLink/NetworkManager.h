#pragma once
#include "RecvThread.h"
#include "SendThread.h"

#include "calc.h"

namespace VRFL
{
	class NetworkManager
	{
	private: 
		static RecvThread m_recv;
		static SendThread m_send;

	public:

		static void InitializeUnity(unsigned short port);
		static void InitializeSim(unsigned short remoteport);

		static int GetDataCount();
		static ObjectData GetData();

		static void Send(ObjDataT** data, int length);
	};

}