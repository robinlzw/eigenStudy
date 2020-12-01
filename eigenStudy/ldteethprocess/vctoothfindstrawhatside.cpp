#include "stdafx.h"
#include "vctoothfindstrawhatside.h"

void FindStrawHatSideVerts(std::vector<unsigned>& vStrawHatSideVerts,
	std::vector<unsigned>& vFixedVerts, const VNALGMESH::VSMeshGeodic& mg)
{
	unsigned mToothCount = mg.lstDist.len;
	//search max value
	float flMax = mg.lstDist.pData[0];
	for (unsigned i = 1; i < mToothCount; i++)
	{
		if (flMax < mg.lstDist.pData[i])
		{
			flMax = mg.lstDist.pData[i];
		}
	}
	float flThreshold = flMax * 0.1f;
	//search Less than one tenth value ,and push_back vVertices
	for (unsigned j = 0; j < mToothCount; j++)
	{
		if (mg.lstDist.pData[j] < flThreshold)
			vStrawHatSideVerts.push_back(j);
		else
			vFixedVerts.push_back(j);
	}
}
