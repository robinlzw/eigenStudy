#pragma once
#include <erp/erp.h>
#include "VBEDDecCombHandle.h"

//Ԥ���������ѹ

class VBPreProcDecompress
{
public:
	VBPreProcDecompress() {}
	~VBPreProcDecompress() {}

	void Build(VSSimpleMeshF & smesh, const VNMesh::VSCompressedMesh & cmp)
	{
		Process(cmp);

		smesh = res;
	}

	void Process(const VNMesh::VSCompressedMesh & cmp);
	void Data(const VNMesh::VSCompressedMesh & cmp);

private:
	VBEDDecCombHandle                Dec;
	VNWZKALG::VSEdgeBreakerDecInfo   info;
	VSSimpleMeshF                    res;
	std::vector<VSCompressRes>       m_vTemp;
};
