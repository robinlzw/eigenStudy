#pragma once

#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj\ialgeq\ialgeq.h>

//一阶拉普拉斯矩阵，非均匀

class VBLaplaceFirst
{
public:
	VBLaplaceFirst() {}
	~VBLaplaceFirst() {}

	void Build( VSSprsMatData< double > & A, const VSGraphMesh & gm)
	{
        m_vFirstLapTripl.clear() ;
        m_vTriInfo.clear() ;           //下标与面索引一致
        m_vVertArea.clear() ;          //顶点邻接三角片的面积之和
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
	void NoDiagElem(const VSGraphMesh & gm);     //非对角元素
	void DiagElem(const VSGraphMesh & gm);      //对角元素
	void nonNormalization(const VSGraphMesh & gm);

private:
	struct TriInfo
	{
		VFVECTOR3  triCot;    //余切值
		double     triSqure;  //面积
	};

	struct VSTripletAdp : public VSTriplet< double >
	{
		int col() const { return VSTriplet::col; }
		int row() const { return VSTriplet::row; }
		double value() const { return VSTriplet::val; }
	};

private:
	std::vector< VSTriplet< double > >        m_vFirstLapTripl;
	std::vector< TriInfo    >                m_vTriInfo;           //下标与面索引一致
	std::vector<     double      >            m_vVertArea;          //顶点邻接三角片的面积之和
	std::vector< VSTripletF >                m_vDiagTemp;
};

