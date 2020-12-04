#include "stdafx.h"
#include "VJPointWtToCurve.h"
 
namespace NMALG_CUTPATHGEN
{
	void VJPointWtToCurve::Build(VNALGMESH::VSCurveOnMesh & cv, const VSGraphMesh & gm,
		const VSConstBuffer< float > & weight)
	{
		m_vCurve.clear();
		m_vCircle.clear();

		m_vFlags.resize(gm.GetEdgeCount());
		memset(&m_vFlags[0], 0, sizeof(unsigned char) * m_vFlags.size());
		VASSERT(gm.GetVertexCount() == weight.len);

		VNALGMESH::VSPointOnEdge pntSrch, pntFst;
		unsigned      nNxtDir;

		pntFst = findFirstSplitPoint(gm, weight.pData, nNxtDir, m_vFlags);
		m_vCircle.push_back(0);

		while (pntFst.EdgeIndx != VD_INVALID_INDEX)
		{
			pntSrch = pntFst;
			VASSERT(pntFst.EdgeIndx != VD_INVALID_INDEX);

			do
			{
				m_vCurve.push_back(pntSrch);
				if (nNxtDir == 1)
					pntSrch = findNextEdge< 1 >(gm, weight.pData, pntSrch, nNxtDir);
				else
					pntSrch = findNextEdge< 0 >(gm, weight.pData, pntSrch, nNxtDir);

				m_vFlags[pntSrch.EdgeIndx] = 1;

			} while (pntSrch.EdgeIndx != pntFst.EdgeIndx);

			m_vCircle.push_back(m_vCurve.size());
			pntFst = findFirstSplitPoint(gm, weight.pData, nNxtDir, m_vFlags);
		}

		for (auto & v : m_vCurve)
		{
			v.Alpha = calcEdgeAlpha(gm, weight.pData, v.EdgeIndx);
		}

		cv.edgePoints = VD_V2CB(m_vCurve);
		cv.circles = VD_V2CB(m_vCircle);
	}

	VJPointWtToCurve::PntOnEdge VJPointWtToCurve::findFirstSplitPoint(const VSGraphMesh & gm, const float * pweight, unsigned & nRight, std::vector< unsigned char > & vFlags)
	{
		VJPointWtToCurve::PntOnEdge pnt;
		unsigned EC = gm.GetEdgeCount();

		makeInvalidateEdge(pnt);

		for (unsigned i = 0; i < EC; i++)
		{
			if (vFlags[i])
				continue;

			vFlags[i] = 1;

			auto & vie = gm.GetVertIndxOfEdge(i);

			float v0 = pweight[vie[0]];
			float v1 = pweight[vie[1]];

			if (IsOutter(v0))
			{
				if (IsInner(v1))
				{
					nRight = 1;
					pnt.EdgeIndx = i;
					break;
				}
			}
			else if (IsOutter(v1))
			{
				nRight = 0;
				pnt.EdgeIndx = i;
				break;
			}
		}

		return pnt;
	}
}
