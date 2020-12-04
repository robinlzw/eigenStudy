#pragma once
#include "VBPCACompress.h"
#include <sys/alg/isysmesh.h>

//PCAΩ‚—π

class VBPCARestore
{
public:
	VBPCARestore() {}
	~VBPCARestore() {}

	void Build(VSDnsMatrixF & framePos, const UDVmat & udv)
	{
		Restore(udv);

		framePos.row   = udv.U.row;
		framePos.col   = udv.V.row;
		framePos.pData = &m_vFramePos.front();
	}

private:
	void  Restore(const UDVmat & udv);

private:
	//std::vector<VSConstBuffer<float>> a;
	//std::vector<std::vector<float>>   m_vFramePos;
	std::vector<float>                m_vFramePos;
};