#pragma once
#include <LibStatic/include/objreference.h>
#include <sys/alg/isysmesh.h>
#include <pkg/ldwzkupdater.h>
#include "../include/algcutpathgen.h"
#include "VCFindCutPath.h"

namespace NMALG_CUTPATHGEN
{
	class VSCutPathGenerator : public NM_COMMON::VObjRef, public IVCutPathGenerator
	{
	public:
		VSCutPathGenerator(const unsigned nGuid);
		virtual ~VSCutPathGenerator();

		virtual void Gen(VSCutPathInfo& pathInfo, const VSCutPathGenInput& input);
		virtual void GenAttCutPath(VSCutPathInfo& pathInfo, const VFRay& ray, const VSSimpleMeshF& simpMesh,
			NMALG_MESHCONVEXHULL::IVMeshConvexHull* pMeshConvexHull);
	private:
		std::vector<VFVECTOR3> m_vCutVertices;
		std::vector<VFVECTOR3> m_vCutNormals;
		VCFindCutPath m_findCutPath;
		void OptimizeCutPath(std::vector<VFVECTOR3>& vCutPath, std::vector<unsigned>& vOptimizeVertIndex,
			const VSConstBuffer<unsigned>& cbVertIndex, const VSConstBuffer<VFVECTOR3>& cbCutPath);
		bool CalcVertNormals(std::vector<VFVECTOR3>& vCutNormals,
			const VSConstBuffer<VFVECTOR3>& cbCutVertices,
			const VSConstBuffer<unsigned>& cbVertIndex, 
			const VSPerfectMesh& perfMesh, 
			const VSCutPathGenInput& input);
	private:
		float m_flMaxRatio;
		float m_flMinRatio;
	};
}
