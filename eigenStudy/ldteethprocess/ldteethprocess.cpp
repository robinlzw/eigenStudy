#include "stdafx.h"
#include "lsalgteethprocess.h"
#include <pkg/ldteethprocess.h>
#define GUID_BASE 0x80000000



// ldteethprocess.cpp
/*
		定义了本模块输出DLL文件中的所有输出函数。
		实现了lsalgteethprocess.h里定义的VSysManager类
*/




static NMALG_TEETHPATCHING::VSysManager g_sysTeethManager;






// 输出库————#define LDTEETHPROCESS_API __declspec(dllexport)
LDTEETHPROCESS_API NMALG_TEETHPATCHING::IVBTeethPatching* VLIBEXPFUNCNAMECREATE()
{
	return g_sysTeethManager.CreateSys();
}





LDTEETHPROCESS_API void VLIBEXPFUNCNAMEDELETE(NMALG_TEETHPATCHING::IVBTeethPatching* teethPathing)
{
	g_sysTeethManager.DeleteSys(teethPathing);
}







namespace NMALG_TEETHPATCHING
{
	VSysManager::VSysManager()
	{

	}


	VSysManager::~VSysManager()
	{
		for (unsigned i = 0; i < m_vSysTeethPatching.size(); i++)
		{
			if (NULL != m_vSysTeethPatching[i])
				delete m_vSysTeethPatching[i];
		}
		m_vSysTeethPatching.clear();
	}



	VBTeethPatching* VSysManager::CreateSys()
	{
		unsigned nIdx = m_vSysTeethPatching.size();
		m_vSysTeethPatching.push_back(new VBTeethPatching(nIdx + GUID_BASE));
		return m_vSysTeethPatching[nIdx];
	}

	
	void VSysManager::DeleteSys(IVBTeethPatching* teethPathing)
	{
		if (NULL != m_vSysTeethPatching[((VBTeethPatching*)teethPathing)->GetGuid() - GUID_BASE])
		{
			unsigned nIdx = ((VBTeethPatching*)teethPathing)->GetGuid() - GUID_BASE;
			delete m_vSysTeethPatching[nIdx];
			m_vSysTeethPatching[nIdx] = NULL;
		}
	}
}
