#pragma once
#include <obj/alg/stdnotdef.h>
#include <vmath/vflocation.h>
#include <vmath/vfmesh.h>
#include <vstd/tisys.h>
#include <windows.h>
#include "./ialgobjectcontainer.h"

namespace NMALG_OBJECTCONTAINERPROXY
{
	class VCProxy
	{
	public:
		VCProxy();
		~VCProxy();

		void* CreateObj(const unsigned nObjEnum);
		void ReleaseObj(void* pObj);

		static void Initialize();
		static void Release();
	};
}
