// ldteethprocess.cpp : Defines the exported functions for the DLL application.

#include "stdafx.h"
#include "ldalgobjectcontainer.h"
#include "interface/iobjreference.h"
#include "LibStatic/include/algcutpathgen.h"
#include "LibStatic/include/algstringmeshgen.h"
#include "LibStatic/include/algmeshconvexhull.h"
#include "LibStatic/include/algmeshsimplify.h"
#include "LibStatic/include/algocclusalpadgen.h"

#include "LibStatic/include/algdeterminemarkposition.h"
#include "LibStatic/include/algfillwax2.h"
#include "LibStatic/include/algmeshrayintersection.h"
#include "LibStatic/include/algautoalinement.h"
#include "LibStatic/include/algcutpathabb.h"
#include "LibStatic/include/algjoinbndry.h"
#include "LibStatic/include/algdentalarchcurve2.h"
#include "LibStatic/include/algmeshsimplify2.h"
#include "LibStatic/include/algdentalarchcurveNew.h"
#define GUID_BASE 0x80000000



namespace NMALG_OBJECTCONTAINER
{
	VObjManager::VObjManager()
	{
	}
	VObjManager::~VObjManager()
	{
		for (unsigned i = 0; i < m_vObj.size(); i++)
		{
			if (NULL != m_vObj[i])
				delete m_vObj[i];
		}
		m_vObj.clear();
	}

	void VObjManager::ReleaseObj(IVObject* pObj)
	{
		if (pObj == nullptr)
			return;
#if 0
		auto obj = dynamic_cast<NM_COMMON::IVObjRef*>(pObj);
		if (obj == nullptr)
			return;

		unsigned nGuid = obj->GetGuid() - GUID_BASE;
		if (NULL != m_vObj[nGuid])
		{
			delete m_vObj[nGuid];
			m_vObj[nGuid] = NULL;
		}
#else
		for (size_t i = 0; i < m_vObj.size(); i++)
		{
			if (pObj == m_vObj[i])
			{
				delete m_vObj[i];
				m_vObj[i] = NULL;
			}
		}
#endif
	}

	void* VObjManager::CreateObj(unsigned nObjEnum)
	{
		switch (nObjEnum)
		{
			case OBJ_CUTPATH_GEN_E:
				return OBJ_GetCutPathGenerator();
			//case OBJ_STRMESH_GEN_E:
			//	return OBJ_GetStringMeshGenerator();
			case OBJ_MESHCONVEXHULL_GEN_E:
				return OBJ_GetMeshConvexHull();
			case OBJ_MESHSIMPLIFY_GEN_E:
				return OBJ_GetMeshSimplify();
			case OBJ_OCCLUSALPAD_GEN_E:
				return OBJ_GetOcclusalPadGen2();
			case OBJ_DETERMINEMARKPOSITION_GEN_E:
				return OBJ_GetDeterminemarkposition();
			case OBJ_FILLWAX2_GEN_E:
				return OBJ_GetFillwax2();
			case OBJ_MESHRAYINTERSECTION_GEN_E:
				return OBJ_GetMeshrayintersection();
			case OBJ_AUTOALINEMENT_GEN_E:
				return OBJ_GetAutoalinement();
			case OBJ_CUTPATHABB_GEN_E:
				return OBJ_GetCutpathabb();
			case OBJ_JOINBNDRY_GEN_E:
				return OBJ_GetJoinbndry();
			case OBJ_DENTALARCHCURVE2_GEN_E:
				return OBJ_GetDentalarchcurve2();
			case OBJ_MESHSIMPLIFY2_GEN_E:
				return OBJ_GetMeshsimplify2();
			case OBJ_DENTALARCHCURVENEW_GEN_E:
				return OBJ_GetDentalarchcurveNew();
			default:
				return NULL;
		}
	}
	void* VObjManager::OBJ_GetCutPathGenerator()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_CUTPATHGEN::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetDentalarchcurveNew()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_DENTALARCHCURVENEW::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetMeshsimplify2()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_MESHSIMPLIFY2::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetDentalarchcurve2()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_DENTALARCHCURVE2::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetJoinbndry()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_JOINBNDRY::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetCutpathabb()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_CUTPATHABB::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetAutoalinement()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_AUTOALINEMENT::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetFillwax2()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_FILLWAX2::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetMeshrayintersection()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_MESHRAYINTERSECTION::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetDeterminemarkposition()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_DETERMINEMARKPOSITION::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	//void* VObjManager::OBJ_GetStringMeshGenerator()
	//{
	//	unsigned nIdx = m_vObj.size();
	//	m_vObj.push_back(NMALG_STRINGMESHGEN::GetGenerator(nIdx + GUID_BASE));
	//	return m_vObj[nIdx];
	//}

	void* VObjManager::OBJ_GetMeshConvexHull()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_MESHCONVEXHULL::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetMeshSimplify()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_MESHSIMPLIFY::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}

	void* VObjManager::OBJ_GetOcclusalPadGen2()
	{
		unsigned nIdx = m_vObj.size();
		m_vObj.push_back(NMALG_OCCLUSALPADGEN2::GetGenerator(nIdx + GUID_BASE));
		return m_vObj[nIdx];
	}
}
