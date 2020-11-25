#include "stdafx.h"
#include "lsalgtools/ilsalgtools.h"

/////////////////////////////////////////////////////////////////
// Class SmarteeTime
/////////////////////////////////////////////////////////////////
VSTimeTracker::VSTimeTracker() : m_dbPerformanceFrequency(0.0), m_nStartTime(0)
{
#ifdef WIN32
	LARGE_INTEGER  large_interger;

	QueryPerformanceFrequency(&large_interger);
	m_dbPerformanceFrequency = (double)large_interger.QuadPart;
#endif
}

VSTimeTracker::~VSTimeTracker()
{
}

void VSTimeTracker::TM_Start(void)
{
#ifdef WIN32
	LARGE_INTEGER  large_interger;
	QueryPerformanceCounter(&large_interger);
	m_nStartTime = large_interger.QuadPart;
#endif
}

double VSTimeTracker::TM_Stop(void)
{
	double dbTimeS = 0.0;
#ifdef WIN32
	LARGE_INTEGER  large_interger;
	QueryPerformanceCounter(&large_interger);
	__int64 nTime = large_interger.QuadPart - m_nStartTime;
	dbTimeS = nTime * 1000.0 / m_dbPerformanceFrequency;
#endif
	return dbTimeS;
}
