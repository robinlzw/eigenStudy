#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <unordered_map>
#include <set>
#include <algorithm>  
#include <obj/ialgeq/ialgeq.h>
#include "lsalgobjfile.h"


template<>
struct std::hash<VFVECTOR3>
{
	size_t operator() (const VFVECTOR3& v) const noexcept
	{
		return std::hash<decltype(v.x)>()(v.x)
			+ std::hash<decltype(v.y)>()(v.y)
			+ std::hash<decltype(v.z)>()(v.z);
	}
};
template<>
struct std::equal_to<VFVECTOR3>
{
	bool operator() (const VFVECTOR3& v0, const VFVECTOR3& v1) const noexcept
	{
		return (fabs(v0.x - v1.x) < VF_EPS_2)
			&& (fabs(v0.y - v1.y) < VF_EPS_2)
			&& (fabs(v0.z - v1.z) < VF_EPS_2);
	}
};

namespace NM_TOOLS
{
#define VERTICE_COOR_MIN (-10000.0f)
#define VERTICE_COOR_MAX (10000.0f)
	//////////////////////////////////////////////////////////////////////////
	// Bit operator
	enum E_BIT
	{
		E_BIT0 = 0x00000001,
		E_BIT1 = 0x00000002,
		E_BIT2 = 0x00000004,
		E_BIT3 = 0x00000008,
		E_BIT4 = 0x00000010,
		E_BIT5 = 0x00000020,
		E_BIT6 = 0x00000040,
		E_BIT7 = 0x00000080,
		E_BIT8 = 0x00000100,
		E_BIT9 = 0x00000200,
		E_BIT10 = 0x00000400,
		E_BIT11 = 0x00000800,
		E_BIT12 = 0x00001000,
		E_BIT13 = 0x00002000,
		E_BIT14 = 0x00004000,
		E_BIT15 = 0x00008000,
		E_BIT16 = 0x00010000,
		E_BIT17 = 0x00020000,
		E_BIT18 = 0x00040000,
		E_BIT19 = 0x00080000,
		E_BIT20 = 0x00100000,
		E_BIT21 = 0x00200000,
		E_BIT22 = 0x00400000,
		E_BIT23 = 0x00800000,
		E_BIT24 = 0x01000000,
		E_BIT25 = 0x02000000,
		E_BIT26 = 0x04000000,
		E_BIT27 = 0x08000000,
		E_BIT28 = 0x10000000,
		E_BIT29 = 0x20000000,
		E_BIT30 = 0x40000000,
		E_BIT31 = 0x80000000,
	};

#define BitIsSet(nFlag, nBit)  (((nFlag) & (nBit)) != 0) ? true : false
#define BitSet(nFlag, nBit)  (nFlag) |= (nBit)
#define BitClean(nFlag, nBit)  (nFlag) &= (~(nBit))
	enum RAYPLANE_CROSS_E {
		E_PARALLEL = 1,
		E_CROSS_POSITIVE,
		E_CROSS_NEGATIVE,
	};
}

#ifndef DELETEOBJ
#define DELETEOBJ(obj) \
	if(NULL != (obj))	\
	{	\
		delete (obj);	\
		(obj) = NULL;	\
	}
#endif

#define GETPERFECTMESH(perfMesh, simpMesh, extSys, rb) \
	TVExtSource<VNALGMESH::VRPerfMesh> perf_##extSys; \
	TVR2B<VNALGMESH::VRPerfMesh> perf_##rb(*(perf_##extSys)); \
	perf_##rb.Build(perfMesh, simpMesh);
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// 字符串转换
/////////////////////////////////////////////////////////////////
std::string STR_UnicodeToANSI(const std::wstring& str);
std::string STR_UnicodeToUTF8(const std::wstring& str);

std::wstring STR_ANSIToUnicode(const std::string& str);
std::wstring STR_UTF8ToUnicode(const std::string& str);

/////////////////////////////////////////////////////////////////
// 要使用日志功能，在包括次头文件之前添加宏定义 LOG_WRITE_ENABLE
/////////////////////////////////////////////////////////////////
extern void LOG_Reset(const char* pszLogFileName, int nMode = std::ios::out);
extern void LOG_Write(const char* pszLogFileName, char * format, ...);
extern void LOG_Release();
extern void LOG_DumpError(char * format, ...);
/////////////////////////////////////////////////////////////////
// environment
/////////////////////////////////////////////////////////////////
extern void ENV_SetCurrentDirAsExe(void);
extern void DBGTrace(char *format, ...);

/////////////////////////////////////////////////////////////////
// Time
/////////////////////////////////////////////////////////////////
class VSTimeTracker
{
public:
	VSTimeTracker();
	~VSTimeTracker();

	void TM_Start(void);
	double TM_Stop(void);

private:
	double		m_dbPerformanceFrequency;
	long long	m_nStartTime;
};

/////////////////////////////////////////////////////////////////
// File operator
/////////////////////////////////////////////////////////////////
extern unsigned FILE_GetLength(std::ifstream& fileStream);
extern void FILE_GetDirFiles(std::vector<std::string>& vFileName, const std::string& path);
extern void FILE_GetPathLastName(std::string& strName, const std::string& strPath);
/////////////////////////////////////////////////////////////////
// Config file operator
/////////////////////////////////////////////////////////////////
extern DWORD INIGetInt(const TCHAR* pszKey, const TCHAR* pszConfFile);
extern float INIGetFloat(const TCHAR* pszKey, const TCHAR* pszConfFile);
extern void INIGetString(TCHAR* pszOut, const TCHAR* pszKey, const TCHAR* pszConfFile);
extern void INISetInt(const TCHAR* pszKey, const DWORD dwVal, const TCHAR* pszConfFile);
extern void INISetString(const TCHAR* pszKey, const TCHAR* pszVal, const TCHAR* pszConfFile);
extern void INISetFloat(const TCHAR* pszKey, const float flVal, const TCHAR* pszConfFile);
