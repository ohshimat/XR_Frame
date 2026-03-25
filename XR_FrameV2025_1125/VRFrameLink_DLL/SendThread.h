#pragma once
#include "ThreadBase.h"
#include <list>
#include <vector>

#include "VRFLInterface.h"
#include "calc.h"
#include "syncobject.h"
#include "object.h"
#include "gmath/Vector3.hpp"
#include "gmath/Quaternion.hpp"

namespace VRFL
{
	class SendThread : public ThreadBase
	{
	private:
		SOCKET m_socket;
		struct sockaddr_in m_addr;

		std::list< std::vector<ObjectData>* > m_sendList;
		SyncInterlock m_sendLock;

		HANDLE m_sendEvent;
		int m_timeOut;
		std::vector<ObjectData>* m_senddata;

	public:
		SendThread();
		virtual ~SendThread();

		void Initialize(unsigned short remoteport);

		void Send(ObjDataT** data, int length);

	protected:
		virtual bool ThreadProc();
		void addData(std::vector<ObjectData>* data);
		std::vector<ObjectData>* popData();

		//void makeTransData(std::vector<ObjectData>* datas, ObjDataT* src, Vector3 offpos, Quaternion offrot);
	};
}
