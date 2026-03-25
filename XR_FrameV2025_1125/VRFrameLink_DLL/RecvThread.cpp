#include "pch.h"
#include "RecvThread.h"

#include "VRFLError.h"
#include "VRFLConfig.h"

#include <WinSock2.h>
#include <WS2tcpip.h>


namespace VRFL
{
	RecvThread::RecvThread()
	{
		m_socket = INVALID_SOCKET;
		m_sockAddr = NULL;

		m_timeOut = 1000;
	}
	RecvThread::~RecvThread()
	{
		Close();
	}

	bool RecvThread::Open(unsigned short port)
	{
		struct ::addrinfo hint;
		char portch[10];
		int res;

		if (IsAlive())
		{
			VRFLError::ErrorMessage("RecvThread::Open - Already opened.");
			return true;
		}

		memset(&hint, 0, sizeof(hint));
		hint.ai_family = AF_INET;
		hint.ai_socktype = SOCK_DGRAM;
		hint.ai_protocol = IPPROTO_UDP;

		m_port = port;

		m_socket = WSASocket(hint.ai_family, hint.ai_socktype, hint.ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_socket == INVALID_SOCKET)
		{
			VRFLError::ErrorMessage("RecvThread::Open - Create Socket failed.");
			return false;
		}

		_snprintf_s(portch, 10, _TRUNCATE, "%d", m_port);

		res = getaddrinfo("0.0.0.0", portch, &hint, &m_sockAddr);
		if (res != 0)
		{
			VRFLError::ErrorMessage("RecvThread::Open - Get Address Information failed.");
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			return false;
		}

		res = bind(m_socket, m_sockAddr->ai_addr, (int)m_sockAddr->ai_addrlen);
		if(res != 0)
		{
			VRFLError::ErrorMessage("RecvThread::Open - Bind socket failed.");
			closesocket(m_socket);
			freeaddrinfo(m_sockAddr);
			m_socket = INVALID_SOCKET;
			return false;
		}

		m_sockEvent = WSACreateEvent();
		WSAEventSelect(m_socket, m_sockEvent, FD_READ | FD_CLOSE);

		return true;
	}

	void RecvThread::Close()
	{
		Quit(); // Quit thread

		if(m_sockEvent != 0) WSACloseEvent(m_sockEvent);
		if(m_sockAddr != NULL) freeaddrinfo(m_sockAddr);
		if (m_socket != INVALID_SOCKET) closesocket(m_socket);

		m_sockAddr = NULL;
		m_sockEvent = 0;
		m_socket = INVALID_SOCKET;
	}

	int RecvThread::GetDataCount()
	{
		return m_datalist.size();
	}
	ObjectData RecvThread::GetData()
	{
		return popData();
	}

	bool RecvThread::ThreadProc()
	{
		int res = checkEvent();

		if (res == 1)
		{
			int size = recvData(m_buff, UDP_BUFF_SIZE);

			if (size > 0)
			{
				parseData(m_buff, size);
			}
		}

		return true;
	}

	int RecvThread::checkEvent()
	{
		WSANETWORKEVENTS nete;
		DWORD res = WaitForSingleObject(m_sockEvent, m_timeOut);
		DWORD ne;

		switch (res)
		{
		case WAIT_TIMEOUT: // Timeout
			return 0;

		case WAIT_OBJECT_0: // Event
			ne = WSAEnumNetworkEvents(m_socket, m_sockEvent, &nete);
			if (res == SOCKET_ERROR)
			{
				return -1;
			}

			if (nete.lNetworkEvents & FD_READ)
			{
				return 1; // read
			}
			if (nete.lNetworkEvents & FD_CLOSE)
			{
				// not impl (no connection)
			}
		}

		return -1;
	}
	
	int  RecvThread::recvData(char* buff, int size)
	{
		int piv = 0, res;
		for (;;) {
			res = ::recvfrom(m_socket, &buff[piv], size, 0, NULL, NULL);
			if (res == SOCKET_ERROR) {
				int err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK && err != WSAENOBUFS) return false;  // fatal error
			}
			else if (res == 0) return 0; // FIN(ACK)
			else {
				//size -= res;
				//piv += res;
				//if (size == 0) break;
				return res;
			}

			for (;;) {
				res = checkEvent();

				if (res == -1) return 0;
				if (res == 1) break;
				Sleep(0);
			}
		}

		return true;
	}

	bool RecvThread::parseData(char* data, int size)
	{
		SyncKeeper sk;

		int len = size / sizeof(ObjectData);

		for (int i = 0; i < len; i++)
		{
			ObjectData od;

			memcpy((void*)&od, &data[i * sizeof(ObjectData)], sizeof(ObjectData));

			addData(od);
		}

		return true;
	}

	void RecvThread::addData(ObjectData& data)
	{
		SyncKeeper sk(&m_datalock);

		sk.Lock();
		{
			m_datalist.push_back(data);
		}
		sk.UnLock();
	}

	ObjectData RecvThread::popData()
	{
		SyncKeeper sk(&m_datalock);
		ObjectData od;

		sk.Lock();
		{
			od = m_datalist.front();
			m_datalist.pop_front();
		}
		sk.UnLock();

		return od;
	}
}