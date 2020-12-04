#pragma once

#include"VBLaplaceFirst.h"
#include"../lswzkalgeq/VBTripletMatrix.h"
#include"../lswzkalgeq/VBLaplaceMultiply.h"
#include"VBLaplace2DeformMat.h"
#include"VBLaplace2DeformNewRight.h"
#include"../lswzkalgeq/VBEquationResolver.h"
#include <sys/alg/isysmesh.h>

//二阶拉普拉斯形变组合

class VBLapComb
{
public:
	VBLapComb() {}
	~VBLapComb() {}

	void Build(VSConstBuffer< VFVector3 , char > & vertPos, const VSGraphMesh & gm, const VSConstBuffer< unsigned, VFVector3 > & destVert)
	{
        m_flag.resize( gm.GetVertexCount() ) ;
        memset( & m_flag[0] , 0 , sizeof(char)* m_flag.size() ) ;

        for( unsigned i = 0 ; i < destVert.len ; i ++ )
        {
            m_flag[ destVert.pData1[i] ] = 1 ;
        }

		VSSprsMatData< double >            firstATpl;
		VSSprsMatrixColMajor< double >     firstA;
		VSSprsMatrixColMajor< double >     secondA;
		VSSprsMatrixColMajor< double >     modifiedSecLap;
		VSDnsMatData< float, false >      mB;
		VSDnsMatData< double , true >       x;

		firstLap.Build(firstATpl, gm);
		firstLapSparse.Build(firstA, firstATpl);
		firstLapMulty.Build(secondA, firstA);

		secondLap.Build(modifiedSecLap, secondA, VD_V2CB( m_flag ) );

		eqRight.Build(mB, gm.GetVertexCount(), destVert);

        _solve_usedouble( x , modifiedSecLap, mB ) ;

		_copy(m_vSolve, x);

		vertPos = VD_V2CB( m_vSolve , m_flag ) ;
	}

private:
    void _solve_usedouble( VSDnsMatData< double , true > & x, const VSSprsMatrixColMajor< double > & A , const VSDnsMatData< float , false > & mb )
    { 
        VSDnsMatData< double , false > newB ;
        std::vector< double > vBuff( mb.col * mb.row ) ;

        for( unsigned i = 0 ; i < vBuff.size() ; i ++ )
        {
            vBuff[i] = mb.pData[i] ;
        }

        newB.col = mb.col ;
        newB.row = mb.row ;
        newB.pData = & vBuff.front() ;

        solve.Build(x, A , newB ) ;
    }

	void _copy(std::vector< VFVECTOR3 > & arr, VSDnsMatData< double , true > & solv)
	{
		unsigned vc  = solv.row;
		unsigned dvc = 2 * vc;

		arr.resize(vc);

		for (unsigned i = 0; i < vc; i++)
		{
			arr[i].x = static_cast< float >( solv.pData[i      ] ) ;
			arr[i].y = static_cast< float >( solv.pData[i + vc ] ) ;
			arr[i].z = static_cast< float >( solv.pData[i + dvc] ) ;
		}
	}

private:
	VBLaplaceFirst                        firstLap;
	VBTripletMatrix<double>               firstLapSparse;
	VBLaplaceMultiply<double>             firstLapMulty;
	VBLaplace2DeformMat                   secondLap;
	VBLaplace2DeformNewRight              eqRight;
	VBEquationResolver< double , false >  solve;

private:		
	std::vector< char >               m_flag  ;
	std::vector<  VFVECTOR3   >       m_vSolve;
};

