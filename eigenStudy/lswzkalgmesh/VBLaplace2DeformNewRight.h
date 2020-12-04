#pragma once

#include<vmath/vfmesh.h>
//#include"../libalgeq/VJResolver.h"
#include <sys/alg/isysmesh.h>
#include <obj\ialgeq\ialgeq.h>

//二阶拉普拉斯方程右边系数

class VBLaplace2DeformNewRight
{
public:
	VBLaplace2DeformNewRight() {}
	~VBLaplace2DeformNewRight() {}

	void Build( VSDnsMatData< float , false > & b, const unsigned & vertCount, const VSConstBuffer< unsigned, VFVector3 > & destVert)
	{
		m_vB.resize(vertCount);
        if( vertCount )
            memset( &m_vB.front() , 0 , sizeof( VFVECTOR3 ) * vertCount ) ;

        for (unsigned i = 0; i < destVert.len; i++)
		{
			m_vB[destVert.pData1[i]] = destVert.pData2[i];
		}

		b.row = vertCount;
		b.col = 3 ;
		b.pData = &m_vB[0].x;
	}

private:
	std::vector<VFVECTOR3>    m_vB;
};

