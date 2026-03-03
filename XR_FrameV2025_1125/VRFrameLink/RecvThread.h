#pragma once
#include "ThreadBase.h"
#include "framework.h"
#include "VRFLConfig.h"
#include "VRFLInterface.h"
#include <list>
#include "syncobject.h"

namespace VRFL
{
	class RecvThread :
		public ThreadBase
	{
	private:
		SOCKET m_socket;
		unsigned short m_port;
		HANDLE m_sockEvent;

		struct ::addrinfo *m_sockAddr;

		long m_timeOut;

		char m_buff[UDP_BUFF_SIZE]; // larger than 65535

		SyncInterlock m_datalock;
		std::list<ObjectData> m_datalist;

	public:
		RecvThread();
		virtual ~RecvThread();

		bool Open(unsigned short port);
		void Close();

		int GetDataCount();
		ObjectData GetData();

	protected:
		virtual bool ThreadProc();
		int checkEvent();
		int recvData(char* buff, int size);
		bool parseData(char* data, int size);
		void addData(ObjectData& data);
		ObjectData popData();
	};
}
