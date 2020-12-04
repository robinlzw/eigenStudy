#pragma once

#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj\ialgeq\ialgeq.h>

//һ��������˹���󣬷Ǿ���

class VBLaplaceFirst
{
public:
	VBLaplaceFirst() {}
	~VBLaplaceFirst() {}

	void Build( VSSprsMatData< double > & A, const VSGraphMesh & gm)
	{
        m_vFirstLapTripl.clear() ;
        m_vTriInfo.clear() ;           //�±���������һ��
        m_vVertArea.clear() ;          //�����ڽ�����Ƭ�����֮��
        m_vDiagTemp.clear() ;

		nonNormalization(gm);

		A.row = gm.GetVertexCount();
		A.col = gm.GetVertexCount();
		A.val.len = m_vFirstLapTripl.size();
		A.val.pData = &m_vFirstLapTripl[0];
	}

private:
	void TriAngleSqr(const VSGraphMesh & gm);
	void VertNbrSurfArea(const VSGraphMesh & gm);
	void NoDiagElem(const VSGraphMesh & gm);     //�ǶԽ�Ԫ��
	void DiagElem(const VSGraphMesh & gm);      //�Խ�Ԫ��
	void nonNormalization(const VSGraphMesh & gm);

private:
	struct TriInfo
	{
		VFVECTOR3  triCot;    //����ֵ
		double     triSqure;  //���
	};

	struct VSTripletAdp : public VSTriplet< double >
	{
		int col() const { return VSTriplet::col; }
		int row() const { return VSTriplet::row; }
		double value() const { return VSTriplet::val; }
	};

private:
	std::vector< VSTriplet< double > >        m_vFirstLapTripl;
	std::vector< TriInfo    >                m_vTriInfo;           //�±���������һ��
	std::vector<     double      >            m_vVertArea;          //�����ڽ�����Ƭ�����֮��
	std::vector< VSTripletF >                m_vDiagTemp;
};

