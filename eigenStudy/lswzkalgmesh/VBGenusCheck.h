#pragma once
#include "stdafx.h"
#include <vmath\vfmesh.h>
#include <sys/alg/isysmesh.h>

//Íø¸ñ¿÷¸ñ¼ì²é

class VBGenusCheck
{
public:
	VBGenusCheck() {}
	~VBGenusCheck() {}

	void Build(unsigned & genus, const VNWZKALG::VSMeshInfo & info)
	{
		unsigned V = info.gm.GetVertexCount();
		unsigned F = info.gm.GetSurfCount();
		unsigned E = info.gm.GetEdgeCount();
		int g = info.connNum - ((V + F - E) + info.bndryNum) / 2;

		genus = std::abs(g);
	}
};