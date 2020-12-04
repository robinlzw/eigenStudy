#pragma once
#include "stdafx.h"
#include <vmath\vfmesh.h>
#include <sys/alg/isysmesh.h>

//¹ÂÁ¢µã¼ì²â

class VBIsolateVertCheck
{
public:
	VBIsolateVertCheck() {}
	~VBIsolateVertCheck() {}

	void Build(VSConstBuffer<unsigned> & isolate, const VSGraphMesh & gm)
	{
		for (unsigned i = 0; i < gm.GetVertexCount(); i++)
		{
			if (0 == gm.GetSurfCountOfVetex(i))
				m_vIsolateVert.push_back(i);
		}

		isolate = VD_V2CB(m_vIsolateVert);
	}

private:
	std::vector<unsigned>    m_vIsolateVert;
};