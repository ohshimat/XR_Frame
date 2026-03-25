#include "pch.h"
#include "NetworkManager.h"

namespace VRFL
{
	RecvThread NetworkManager::m_recv;
	SendThread NetworkManager::m_send;

	void NetworkManager::InitializeUnity(unsigned short port)
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2, 0), &data);

		m_recv.Open(port);
		m_recv.Run();
	}

	void NetworkManager::InitializeSim(unsigned short remoteport)
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2, 0), &data);

		m_send.Initialize(remoteport);
		m_send.Run();
	}

	int NetworkManager::GetDataCount()
	{
		return m_recv.GetDataCount();
	}

	ObjectData NetworkManager::GetData()
	{
		return m_recv.GetData();
	}

	void NetworkManager::Send(ObjDataT** data, int length)
	{
		m_send.Send(data, length);
	}
}