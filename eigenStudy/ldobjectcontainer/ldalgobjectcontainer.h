#pragma once

#include <pkg/ldobjectcontainer.h>
#include <vector>


namespace NMALG_OBJECTCONTAINER
{	
	class VObjManager
	{
	public:
		VObjManager();
		virtual ~VObjManager();

		void* CreateObj(unsigned nObjEnum);
		void ReleaseObj(IVObject* pObj);

	private:
		void* OBJ_GetCutPathGenerator();
		//void* OBJ_GetStringMeshGenerator();
		void* OBJ_GetMeshConvexHull();
		void* OBJ_GetMeshSimplify();
		void* OBJ_GetOcclusalPadGen2();
		void* OBJ_GetDeterminemarkposition();
		void* OBJ_GetFillwax2();
		void* OBJ_GetMeshrayintersection();
		void* OBJ_GetAutoalinement();		
		void* OBJ_GetCutpathabb();
		void* OBJ_GetJoinbndry();
		void* OBJ_GetDentalarchcurve2();
		void* OBJ_GetMeshsimplify2();
		void* OBJ_GetDentalarchcurveNew();


	private:
		std::vector<IVObject*> m_vObj;
	};

}
