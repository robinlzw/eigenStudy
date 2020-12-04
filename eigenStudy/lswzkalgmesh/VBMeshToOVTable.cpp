#include "stdafx.h"
#include "VBMeshToOVTable.h"

void VBMeshToOVTable::MemberInit()
{
	m_vOTable.clear();
	m_vVTable.clear();
	m_vVert.clear();
	m_vCornerFlag.clear();
}

void VBMeshToOVTable::Translate(const VSGraphMesh & gm)
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

void VBMeshToOVTable::SurfCornerProc(const VSGraphMesh & gm, unsigned sid)
{
	for (unsigned i = 0; i < 3; i++)
	{
		if (!m_vCornerFlag[3 * sid + i])
		{
			VNVECTOR2UI vc = OppositeCorner(gm, sid, gm.pSurfaces[sid][i]);
			unsigned cid = 3 * sid + i, ocid = 3 * vc.x + vc.y;
			m_vVTable[ cid ] = gm.pSurfaces[sid][i];
			m_vOTable[ cid ] = ocid;
			m_vVTable[ ocid ] = gm.pSurfaces[vc.x][vc.y];
			m_vOTable[ ocid ] = cid;
			m_vCornerFlag[ cid ] = true;
			m_vCornerFlag[ ocid ] = true;
		}
	}
}

VNVECTOR2UI VBMeshToOVTable::OppositeCorner(const VSGraphMesh & gm, unsigned sid, unsigned vid)   //返回对角的三角形索引和序号
{
	unsigned vOrder = VertOrder(gm, sid, vid);
	unsigned eid = gm.GetEdgeIndxOfSurf(sid)[vOrder];
	VNVECTOR2UI voe = gm.GetVertIndxOfEdge(eid);
	unsigned surfOrder = (gm.pSurfaces[sid][(vOrder + 1) % 3] == voe[0] ? 1 : 0);
	unsigned oppSid = gm.GetSurfIndxOfEdge(eid)[surfOrder];
	//unsigned eOrder = EdgeOrder(gm, oppSid, eid);
	unsigned cvOrder = CVOrder(gm, eid, oppSid);

	VNVECTOR2UI vc(oppSid, cvOrder);
	return vc;
}

unsigned VBMeshToOVTable::CVOrder(const VSGraphMesh & gm, unsigned eid, unsigned sid)
{
	VNVECTOR2UI voe = gm.GetVertIndxOfEdge(eid);
	for (unsigned i = 0; i < 3; i++)
	{
		if (gm.pSurfaces[sid][i] != voe[0] && gm.pSurfaces[sid][i] != voe[1])
			return i;
	}

	return VD_INVALID_INDEX;
}

inline unsigned VBMeshToOVTable::EdgeOrder(const VSGraphMesh & gm, unsigned sid, unsigned eid)
{
	if (eid = gm.GetEdgeIndxOfSurf(sid)[0])
		return 0;

	return (eid == gm.GetEdgeIndxOfSurf(sid)[1] ? 1 : 2);
}

inline unsigned VBMeshToOVTable::VertOrder(const VSGraphMesh & gm, unsigned sid, unsigned vid)
{
	if (vid == gm.pSurfaces[sid][0])
		return 0;

	return  (vid == gm.pSurfaces[sid][1] ? 1 : 2);
}
//VD_EXPORT_SYSTEM_SIMPLE(VBMeshToOVTable, VNALGMESH::VRMeshToOVTable);