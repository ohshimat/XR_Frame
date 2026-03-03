#include "pch.h"
#include "ThreadBase.h"
#include "VRFLError.h"

namespace VRFL
{
	ThreadBase::ThreadBase() :
		m_hThread(0),
		m_threadID(0),
		m_quitFlag(false)
	{

	}
	ThreadBase::~ThreadBase()
	{
		Quit();
	}


	bool ThreadBase::Run()
	{
		if (m_hThread != 0)
		{
			VRFLError::ErrorMessage("ThreadBase::Run - Thread is alive.");
			return true;
		}

		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadBase::entry, (void*)this, 0, &m_threadID);
		if (m_hThread == 0) return false;
		return true;
	}
	bool ThreadBase::Quit()
	{
		if (IsAlive()) {
			m_quitFlag = true;
			DWORD ecode;
			if (GetExitCodeThread(m_hThread, &ecode)) {
				if (ecode == STILL_ACTIVE) {
					if (WaitForSingleObject(m_hThread, 3000) != WAIT_OBJECT_0) {
						return false;
					}
				}
			}
		}
		return true;
	}
	bool ThreadBase::IsAlive() 
	{
		if (m_hThread == 0) return false;
		DWORD ecode;
		if (!GetExitCodeThread(m_hThread, &ecode)) return false;
		if (ecode != STILL_ACTIVE) return false;
		return true;
	}

	unsigned int __stdcall ThreadBase::entry(void* param)
	{
		ThreadBase* pthread = (ThreadBase*)param;
		
		while(!pthread->m_quitFlag) pthread->ThreadProc();

		_endthreadex(0);
		return 0;
	}

}