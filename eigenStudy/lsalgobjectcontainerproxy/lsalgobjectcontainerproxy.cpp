#include "stdafx.h"
#include <windows.h>
#include "lsalgobjectcontainerproxy.h"
#include <set>



namespace NMALG_OBJECTCONTAINERPROXY
{
	HMODULE g_hObjectcontainer = NULL;
	
	
	
	FUNCReleaseObj* g_FuncReleaseObj = NULL;
	
	
	
	FUNCCreateObj* g_FuncCreateObj = NULL;


	unsigned g_nObjectcontainerReCount= 0;
	


	VCProxy::VCProxy()
	{	
	}
	
	
	
	VCProxy::~VCProxy()
	{
	
	}



	void VCProxy::Initialize()
	{
		if (NULL != g_hObjectcontainer)
		{
			return;
		}
		g_hObjectcontainer = LoadLibrary(L"ldobjectcontainer.dll");


		if (NULL == g_hObjectcontainer)
		{
			DWORD dwErr = GetLastError();
			TCHAR szBuf[256] = { 0 };
			wsprintf(szBuf, L"load library ldobjectcontainer.dll failed, err: %d", dwErr);
			MessageBox(NULL, szBuf, L"Smartee", MB_OK);
			return;
		}
		g_FuncReleaseObj = (FUNCReleaseObj*)GetProcAddress(g_hObjectcontainer, "OBJ_Release");
		if (NULL == g_FuncReleaseObj)
		{
			DWORD dwErr = GetLastError();
			TCHAR szBuf[256] = { 0 };
			wsprintf(szBuf, L"GetProcAddress OBJ_Release failed, err: %d", dwErr);
			MessageBox(NULL, szBuf, L"Smartee", MB_OK); 
			return;
		}
		g_FuncCreateObj = (FUNCCreateObj*)GetProcAddress(g_hObjectcontainer, "OBJ_Create");
		if (NULL == g_FuncCreateObj)
		{
			DWORD dwErr = GetLastError();
			TCHAR szBuf[256] = { 0 };
			wsprintf(szBuf, L"GetProcAddress OBJ_Create failed, err: %d", dwErr);
			MessageBox(NULL, szBuf, L"Smartee", MB_OK); 
			return;
		}
	}
	
	
	
	void VCProxy::Release()
	{
		if (NULL != g_hObjectcontainer)
		{			
			g_FuncReleaseObj = NULL;
			g_FuncCreateObj = NULL;
			FreeLibrary(g_hObjectcontainer);		
			g_hObjectcontainer = NULL; 
		}
	}



	void VCProxy::ReleaseObj(void* pObj)
	{
		if (NULL != g_FuncReleaseObj)
		{
			g_FuncReleaseObj(pObj);
		}
	}



	void* VCProxy::CreateObj(const unsigned nObjEnum)
	{
		if (NULL != g_FuncCreateObj)
		{
			return g_FuncCreateObj(nObjEnum);
		}
		return NULL;
	}

}
