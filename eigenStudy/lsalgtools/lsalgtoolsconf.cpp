#include "stdafx.h"
#include "lsalgtools/ilsalgtools.h"

static void GetConfigDir(std::wstring& confDir, const TCHAR* pszConfFile)
{
	std::wstring strRet(pszConfFile);
	if ((std::string::npos != strRet.find(L"./")) ||
		(std::string::npos != strRet.find(L".\\")))
	{
		TCHAR szCurDir[256] = { 0 };
		GetCurrentDirectory(256, szCurDir);
		confDir = szCurDir;
		confDir += strRet.substr(1, strRet.length());
		return;
	}
	confDir = strRet;
	return;
}

DWORD INIGetInt(const TCHAR* pszKey, const TCHAR* pszConfFile)
{
	std::wstring strFileName;
	GetConfigDir(strFileName, pszConfFile);
	return GetPrivateProfileInt(L"ZHENGYADENTALCONFIG", pszKey, 0, strFileName.c_str());
}

void INIGetString(TCHAR* pszOut, const TCHAR* pszKey, const TCHAR* pszConfFile)
{
	std::wstring strFileName;
	GetConfigDir(strFileName, pszConfFile);
	GetPrivateProfileString(L"ZHENGYADENTALCONFIG", pszKey, L"", pszOut, 256, strFileName.c_str());
}

void INISetInt(const TCHAR* pszKey, const DWORD dwVal, const TCHAR* pszConfFile)
{
	TCHAR szVal[256] = { 0 };
	swprintf_s(szVal, 256, L"%d", dwVal);
	INISetString(pszKey, szVal, pszConfFile);
}

void INISetString(const TCHAR* pszKey, const TCHAR* pszVal, const TCHAR* pszConfFile)
{
	std::wstring strFileName;
	GetConfigDir(strFileName, pszConfFile);
	WritePrivateProfileString(L"JIAXINGDENTALCUSTOMIZE", pszKey, pszVal, strFileName.c_str());
}


float INIGetFloat(const TCHAR* pszKey, const TCHAR* pszConfFile)
{
	std::wstring strFileName;
	GetConfigDir(strFileName, pszConfFile);
	TCHAR szValue[256] = { 0 };
	GetPrivateProfileString(L"ZHENGYADENTALCONFIG", pszKey, L"", szValue, 256, strFileName.c_str());
#ifdef UNICODE
	return _wtof(szValue);
#else
	return atof(szValue);
#endif
}

void INISetFloat(const TCHAR* pszKey, const float flVal, const TCHAR* pszConfFile)
{
	std::wstring strFileName;
	GetConfigDir(strFileName, pszConfFile);
	TCHAR szValue[256] = { 0 };	
#ifdef UNICODE
	swprintf_s(szValue, 256, L"%f",flVal);
#else
	sprintf_s(szValue, 256, "%f", flVal);
#endif	
	WritePrivateProfileString(L"JIAXINGDENTALCUSTOMIZE", pszKey, szValue, strFileName.c_str());
}