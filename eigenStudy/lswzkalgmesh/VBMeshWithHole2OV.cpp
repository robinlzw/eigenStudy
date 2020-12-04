#include "stdafx.h"
#include "VBMeshWithHole2OV.h"

void VBMeshWithHole2OV::MemberInit()
{
	m_vOTable.clear();
	m_vVTable.clear();
	m_vVert.clear();
	m_vCornerFlag.clear();
}

void VBMeshWithHole2OV::Translate(const VSGraphMesh & gm)
{
	unsigned sNum = gm.GetSurfCount();
	m_vCornerFlag.resize(3 * sNum);
	m_vOTable.resize(3 * sNum);
	m_vVTable.resize(3 * sNum);
	m_vVert.resize(gm.GetVertexCount());

	memcpy(&m_vVert.front(), gm.pVertices, sizeof(VFVECTOR3) * gm.GetVertexCount());

	for (unsigned i = 0; i < sNum; i++)
	{
		SurfCornerProc(gm, i);
	}

	m_vCornerFlag.clear();
}

void VBMeshWithHole2OV::SurfCornerProc(const VSGraphMesh & gm, unsigned sid)
{
	for (unsigned i = 0; i < 3; i++)
	{
		if (!m_vCornerFlag[3 * sid + i])
		{
			VNVECTOR2UI vc = OppositeCorner(gm, sid, gm.pSurfaces[sid][i]);
			if (vc.y != VD_INVALID_INDEX)                            //无孔洞
			{
				unsigned cid = 3 * sid + i, ocid = 3 * vc.x + vc.y;
				m_vVTable[cid] = gm.pSurfaces[sid][i];
				m_vOTable[cid] = ocid;
				m_vVTable[ocid] = gm.pSurfaces[vc.x][vc.y];
				m_vOTable[ocid] = cid;
				m_vCornerFlag[cid] = true;
				m_vCornerFlag[ocid] = true;
			}
			else   //孔洞边界
			{
				unsigned cid = 3 * sid + i;
				m_vVTable[cid] = gm.pSurfaces[sid][i];
				m_vOTable[cid] = VD_INVALID_INDEX;
				m_vCornerFlag[cid] = true;
			}
		}
	}
}

VNVECTOR2UI VBMeshWithHole2OV::OppositeCorner(const VSGraphMesh & gm, unsigned sid, unsigned vid)   //返回对角的三角形索引和序号
{
	unsigned vOrder = VertOrder(gm, sid, vid);
	unsigned eid = gm.GetEdgeIndxOfSurf(sid)[vOrder];

	VNVECTOR2UI soe = gm.GetSurfIndxOfEdge(eid);
	unsigned surfOrder = (soe[0] == sid ? 1 : 0);
	unsigned oppSid = gm.GetSurfIndxOfEdge(eid)[surfOrder];
	unsigned cvOrder = CVOrder(gm, eid, oppSid);

	VNVECTOR2UI vc(oppSid, cvOrder);
	return vc;
}

unsigned VBMeshWithHole2OV::CVOrder(const VSGraphMesh & gm, unsigned eid, unsigned sid)
{
	VNVECTOR2UI voe = gm.GetVertIndxOfEdge(eid);
	if (sid != VD_INVALID_INDEX)
	{
		for (unsigned i = 0; i < 3; i++)
		{
			if (gm.pSurfaces[sid][i] != voe[0] && gm.pSurfaces[sid][i] != voe[1])
				return i;
		}
	}	

	return VD_INVALID_INDEX;
}

inline unsigned VBMeshWithHole2OV::EdgeOrder(const VSGraphMesh & gm, unsigned sid, unsigned eid)
{
	if (eid = gm.GetEdgeIndxOfSurf(sid)[0])
		return 0;

	return (eid == gm.GetEdgeIndxOfSurf(sid)[1] ? 1 : 2);
}

inline unsigned VBMeshWithHole2OV::VertOrder(const VSGraphMesh & gm, unsigned sid, unsigned vid)
{
	if (vid == gm.pSurfaces[sid][0])
		return 0;

	return  (vid == gm.pSurfaces[sid][1] ? 1 : 2);
}