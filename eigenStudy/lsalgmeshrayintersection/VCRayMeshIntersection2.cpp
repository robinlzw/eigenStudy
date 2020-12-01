#include "stdafx.h"
#include <interface/iobjreference.h>
#include <obj/lsalgtools/profile.h>
#include "VCRayMeshIntersection2.h"
//#define RUNTIME_TEST
#ifdef RUNTIME_TEST
#ifdef VD_F_PROFILE_START
#undef  VD_F_PROFILE_START
#endif
#ifdef VD_F_PROFILE_FINISH
#undef  VD_F_PROFILE_FINISH
#endif
#define VD_F_PROFILE_START( profile ) VSFProfile::StartProfile(#profile,"E:\\testdata\\profile.txt");
#define VD_F_PROFILE_FINISH( profile ) VSFProfile::FinishProfile(#profile,"E:\\testdata\\profile.txt");
#endif


namespace NMALG_MESHRAYINTERSECTION2
{
	IVObject* GetGenerator(const unsigned nGuid)
	{
		return static_cast<IVMeshRayIntersection*>(new VCRayMeshIntersection2(nGuid));
	}

	VCRayMeshIntersection2::VCRayMeshIntersection2(const unsigned nGuid)
		: NM_COMMON::VObjRef(nGuid)		
	{
	}

	VCRayMeshIntersection2::~VCRayMeshIntersection2()
	{
	
	}

	void VCRayMeshIntersection2::Build(VSMeshRayIntersectOutput& output, const VSMeshRayIntersectInput& input)
	{
		if (input.simpMesh.nVertCount == 0 ||
			input.simpMesh.nTriangleCount == 0 ||
			input.rays.len == 0)
			return;
		VD_F_PROFILE_START(VSMeshRayIntersection::Build);
		
		CalcInsert(input.simpMesh, input.rays);

		output.cbRayLen = VD_V2CB(m_vCbRayLen);
		output.cbSurfIdx = VD_V2CB(m_vCbSurfIdx);
		output.cbOpRayLen = VD_V2CB(m_vCbOpRayLen);
		output.cbOpSurfIdx = VD_V2CB(m_vCbOpSurfIdx);
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::Build);
	}


	void VCRayMeshIntersection2::CalcInsert(const VSSimpleMeshF& mesh, const VSConstBuffer<VFRay>& rays)
	{
		VD_F_PROFILE_START(VSMeshRayIntersection::CalcInsert);
		
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::CalcInsert);
	}

}
