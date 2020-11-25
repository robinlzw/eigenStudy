#include "stdafx.h"
#include "lsalgtools/ilsalgtools.h"

#define LOG_WRITE_ENABLE
static CRITICAL_SECTION	g_scLogLock;
static BOOL				g_blLockInit = FALSE;
void LOG_GetTime(char* pszTime, const int nSize)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf_s(pszTime, nSize, "%4d-%d-%d %d:%d:%d=> ",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wMilliseconds);
}

void LOG_Reset(const char* pszLogFileName, int nMode)
{
	if (FALSE == g_blLockInit)
	{
		InitializeCriticalSection(&g_scLogLock);
	}
#ifdef LOG_WRITE_ENABLE
	char szBuf[256] = { 0 };	
	char szTime[256] = { 0 };
#ifdef WIN32
	sprintf_s(szBuf, 256, "%s log\n", pszLogFileName);

	std::ofstream	logFile;
	logFile.open(pszLogFileName, nMode);
	LOG_GetTime(szTime, 256);
	logFile << szTime << " Create " << pszLogFileName << " log\n";
	logFile.close();
#endif	
#endif
}

void LOG_Write(const char* pszLogFileName, char * format, ...)
{
	EnterCriticalSection(&g_scLogLock);
#ifdef LOG_WRITE_ENABLE
	char buffer[1024] = { 0 };
	char szTime[256] = { 0 };
#ifdef WIN32
	va_list argptr;
	va_start(argptr, format);
	vsprintf_s(buffer, format, argptr);
	va_end(argptr);

	std::ofstream	logFile;
	logFile.open(pszLogFileName, std::ios::app);
	LOG_GetTime(szTime, 256);
	logFile << szTime;
	logFile << buffer;
	logFile.close();
#endif
#endif
	LeaveCriticalSection(&g_scLogLock);
}

void LOG_Release()
{
	DeleteCriticalSection(&g_scLogLock);
}

void LOG_DumpError(char * format, ...)
{	
	char buffer[1024] = { 0 };
	va_list argptr;
	va_start(argptr, format);
	vsprintf_s(buffer, format, argptr);
	va_end(argptr);
	MessageBoxA(NULL, "Smartee", buffer, MB_OK);
}
