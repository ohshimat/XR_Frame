#pragma once

#include <Windows.h>
#include <process.h>

namespace VRFL
{ 
	class ThreadBase
	{
	protected:
		HANDLE	m_hThread;
		unsigned int m_threadID;

		bool	m_quitFlag;
	
	public:
		ThreadBase();
		virtual ~ThreadBase();

		virtual bool Run();
		virtual bool Quit();
		virtual bool IsAlive();
	
	protected:
		static unsigned int __stdcall entry(void* param);
		virtual bool ThreadProc() = 0;
	};
}

