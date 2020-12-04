#include "stdafx.h"
#include "VBPCARestore.h"

void VBPCARestore::Restore(const UDVmat & udv)
{
	m_vFramePos.resize(udv.U.row * udv.V.col);

	unsigned frameNum = udv.U.row / 3;
	float sum = 0.f;
	for (unsigned i = 0; i < udv.U.row; i++)
	{
		for (unsigned j = 0; j < udv.V.col; j++)
		{
			sum = 0.f;
			for (unsigned k = 0; k < udv.U.col; k++)
			{
				sum += udv.U.pData[i * udv.U.col + k] * udv.V.pData[j * udv.U.col + k] * udv.D.pData[k];
			}
			//m_vFramePos[i][j] = sum;
			m_vFramePos[i * udv.V.col + j] = sum + udv.mean.pData[i][i / frameNum];
		}
	}
}

//VD_EXPORT_SYSTEM_SIMPLE(VBPCARestore, VNALGMESH::VRPCARestore);