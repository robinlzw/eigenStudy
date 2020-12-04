#pragma once

#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
#include "VBMeshToOVTable.h"

class VBMeshWithHole2OV
{
public:
	VBMeshWithHole2OV() {}
	~VBMeshWithHole2OV() {}

	void Build(OVTable & ov, const VSGraphMesh & gm)
	{
		MemberInit();
		Translate(gm);

		ov.vNum = gm.GetVertexCount();
		ov.OTable = VD_V2CB(m_vOTable);
		ov.VTable = VD_V2CB(m_vVTable);
		ov.VertCoord = VD_V2CB(m_vVert);
	}

private:
	void            MemberInit();
	void            Translate(const VSGraphMesh & gm);
	void            SurfCornerProc(const VSGraphMesh & gm, unsigned sid);
	VNVECTOR2UI     OppositeCorner(const VSGraphMesh & gm, unsigned sid, unsigned vid);
	unsigned        CVOrder(const VSGraphMesh & gm, unsigned eid, unsigned sid);
	inline unsigned EdgeOrder(const VSGraphMesh & gm, unsigned sid, unsigned eid);
	inline unsigned VertOrder(const VSGraphMesh & gm, unsigned sid, unsigned vid);

private:
	std::vector<unsigned>          m_vOTable;
	std::vector<unsigned>          m_vVTable;
	std::vector<VFVECTOR3>         m_vVert;

	std::vector<bool>              m_vCornerFlag;
};