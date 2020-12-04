#pragma once

#include<vmath/vfmesh.h>
#include"../lswzkalgeq/VBEquationResolver.h"
#include"../lswzkalgeq/VBTripletMatrix.h"
#include <sys/alg/isysmesh.h>
#include <obj\ialgeq\ialgeq.h>

//�������񶥵�ı���ֵ

class VVertScalar
{
public:
	VVertScalar() {}
	~VVertScalar() {}

	void Build(VSConstBuffer<float> & vScalar, const VSGraphMesh & gm, const VSConstBuffer< std::pair<unsigned, float> > & VertScalar)
	{
		EqSolver(gm, VertScalar);
		lap.Build(A1, A);
		solv.Build(x, A1, b);

		vScalar.len = x.row * x.col;
		vScalar.pData = x.pData;
	}

private:
	void MatRight(const VSGraphMesh & gm, const VSConstBuffer< std::pair<unsigned, float> > & VertScalar);
	void TriTanSqr(const VSGraphMesh & gm);
	void VertNbrSurfArea(const VSGraphMesh & gm);
	void LapNoDiagElem(const VSGraphMesh & gm);
	void LapDiagElem(const VSGraphMesh & gm);
	void EqSolver(const VSGraphMesh & gm, const VSConstBuffer< std::pair<unsigned, float> > & VertScalar);

private:
	static float threshold;
	VSSprsMatData< float >  A;
	VSSprsMatrixColMajor< float >  A1;
	VBTripletMatrix<float>  lap;
	VSDnsMatData< float, true > b;
	VSDnsMatData< float, true > x;
	VBEquationResolver<float, true>   solv;

	struct TriInfo
	{
		VFVECTOR3  triCot;    //����ֵ
		float      triSqure;  //���
	};
	
	std::vector< VSTripletF >  m_vFirstLapTripl;	

	std::vector< TriInfo    >  m_vTriInfo;           //�±���������һ��
	std::vector< float      >  m_vVertArea;          //�����ڽ�����Ƭ�����֮��
	std::vector< unsigned	>  m_vFlagNbrVert;
	std::vector< float      >  m_vDiagTemp;
	std::vector< bool       >  m_vVertFlag;
	std::vector< float      >  m_vR;
};

