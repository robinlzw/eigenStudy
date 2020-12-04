#pragma once

#include <pkg/ldck4cce.h>
#include <vmath/vtopomesh.h>

namespace NMALG_CUTPATHGEN
{
	class VJPointWtToCurve
	{
	public:
		typedef VNALGMESH::VSPointOnEdge PntOnEdge;

		struct VSPntwToCurve
		{
			VSGraphMesh   mesh;
			const float  *pVertWeight;
		};

	public:
		VJPointWtToCurve(const float flThreshold = 0.0f):
			m_flThreshold(flThreshold)
		{
		}
		~VJPointWtToCurve() {}

	public:
		void Build(VNALGMESH::VSCurveOnMesh & cv, const VSGraphMesh & gm, 
			const VSConstBuffer< float > & weight);
		bool IsInner(float v) const { return v > m_flThreshold; };
		bool IsOutter(float v) const { return !IsInner(v); }
		float InnerVal() const { return 1.f; }
		float OutterVal() const { return -1.f; }


	private:
		PntOnEdge findFirstSplitPoint(const VSGraphMesh & gm, const float * pweight, unsigned & bRight, std::vector< unsigned char > & vFlags);
		void      makeInvalidateEdge(PntOnEdge & e) const { e.EdgeIndx = VD_INVALID_INDEX; }
		float     calcEdgeAlpha(const VSGraphMesh & gm, const float * pweight, unsigned edgeIndx) const;

		template< unsigned nRight >
		PntOnEdge findNextEdge(const VSGraphMesh & gm, const float * pweight, const PntOnEdge & preEdge, unsigned & nNxtDir);

	private:
		std::vector< PntOnEdge     > m_vCurve;
		std::vector< unsigned      > m_vCircle;
		std::vector< unsigned char > m_vFlags;
		float m_flThreshold;
	};

	inline float VJPointWtToCurve::calcEdgeAlpha(const VSGraphMesh & gm, const float * pweight, unsigned edgeIndx) const
	{
		auto & vie = gm.GetVertIndxOfEdge(edgeIndx);

		float v0 = pweight[vie[0]];
		float v1 = pweight[vie[1]];

		float dist = v1 - v0;

		if (dist == 0)
			return 0.5f;

		float alpha = (m_flThreshold - v0) / dist;

		if (alpha < 0.001f)
			alpha = 0.001f;
		else if (alpha > 0.999f)
			alpha = 0.999f;

		return alpha;
	}

	template< unsigned nRight >
	inline VJPointWtToCurve::PntOnEdge VJPointWtToCurve::findNextEdge(const VSGraphMesh & gm, const float * pweight, const PntOnEdge & preEdge, unsigned & nNxtDir)
	{
		VJPointWtToCurve::PntOnEdge pnt;

		makeInvalidateEdge(pnt);

		auto & vie = gm.GetVertIndxOfEdge(preEdge.EdgeIndx);
		auto & sie = gm.GetSurfIndxOfEdge(preEdge.EdgeIndx);
		auto & nise = gm.GetSurfNbrIndxOfEdge(preEdge.EdgeIndx);

		VASSERT((IsOutter(pweight[vie[0]]) && IsInner(pweight[vie[1]]) && nRight == 1)
			|| (IsOutter(pweight[vie[1]]) && IsInner(pweight[vie[0]]) && nRight == 0));

		unsigned rsurf = sie[nRight];
		VASSERT(rsurf != VD_INVALID_INDEX);
		unsigned nchk = nise[nRight];
		unsigned npre = (nchk + 2) % 3;
		unsigned nnxt = (nchk + 1) % 3;

		auto & vis = gm.pSurfaces[rsurf];
		float    w = pweight[vis[nchk]];

		VASSERT(IsOutter(pweight[vis[npre]])); // 前一个顶点应该是外侧点
		VASSERT(IsInner(pweight[vis[nnxt]])); // 下一个顶点应该是内侧点

		auto & eis = gm.GetEdgeIndxOfSurf(rsurf);
		auto & nieos = gm.GetEdgeNbrIndxOfSurf(rsurf);

		if (IsInner(w))
		{
			pnt.EdgeIndx = eis[nnxt];
			nNxtDir = 1 - nieos[nnxt];
		}
		else
		{
			pnt.EdgeIndx = eis[npre];
			nNxtDir = 1 - nieos[npre];
		}

		return pnt;
	}

	class VJLineSelector
	{
	public:
		bool Build(VSConstBuffer< VNALGMESH::VSPointOnEdge > & crvSelected, const VNALGMESH::VSCurveOnMesh & crvs)
		{
			VASSERT(crvs.circles.len > 1);
			if (crvs.circles.len > 2)
			{				
#if 0
				crvSelected.len = crvs.circles.pData[1];
				crvSelected.pData = crvs.edgePoints.pData;
				for (unsigned i = 1; i < crvs.circles.len - 1; i++)
				{
					unsigned head = crvs.circles.pData[i];
					unsigned len = crvs.circles.pData[i + 1] - head;
					if (crvSelected.len < len)
					{
						crvSelected.len = len;
						crvSelected.pData = crvs.edgePoints.pData + head;
					}
				}
#endif
				return false;
			}

			crvSelected = crvs.edgePoints;
			return true;
		}
	};
}
