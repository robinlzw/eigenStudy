#include "stdafx.h"
#include "VCFindCutPath.h"
#include "VJPointWtToCurve.h"

namespace NMALG_CUTPATHGEN
{
	VCFindCutPath::VCFindCutPath()
	{

	}

	VCFindCutPath::~VCFindCutPath()
	{

	}

	void VCFindCutPath::Gen(VSCutPathInfo& pathInfo, std::vector<unsigned>& vCutVertIndex,
		const VSPerfectMesh& mesh, const VSConstBuffer<float>& cbLevelSet, const float flThreshold)
	{
		VJPointWtToCurve pointToCurve(flThreshold);
		VJLineSelector lineSelector;
		VNALGMESH::VSCurveOnMesh curveOnMesh;
		VSConstBuffer<VNALGMESH::VSPointOnEdge> cbPointOnEdge;
		pointToCurve.Build(curveOnMesh, mesh, cbLevelSet);
		bool blRet = lineSelector.Build(cbPointOnEdge, curveOnMesh);
		if (false == blRet)
		{
			pathInfo.blValid = ErrorCode::CUTPATH_GEN_FAIL_FIND;
			return;
		}

		unsigned vtNum = cbPointOnEdge.len;
		m_vCutVertices.resize(vtNum);
		vCutVertIndex.resize(vtNum);
		for (unsigned i = 0; i < vtNum; i++)
		{
			const VNVECTOR2UI &vi = mesh.GetVertIndxOfEdge(cbPointOnEdge.pData[i].EdgeIndx);
			m_vCutVertices[i] = (mesh.pVertices[vi[1]] - mesh.pVertices[vi[0]]) * cbPointOnEdge.pData[i].Alpha + mesh.pVertices[vi[0]];	
			vCutVertIndex[i] = vi[0];
		}
		pathInfo.cbCutVertices = VD_V2CB(m_vCutVertices);
	}
}
