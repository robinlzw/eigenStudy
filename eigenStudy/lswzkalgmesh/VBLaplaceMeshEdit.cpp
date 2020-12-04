#include "stdafx.h"
#include "VBLaplaceMeshEdit.h"


void VBLaplaceMeshEdit::CalSurfNormal(const VSGraphMesh & gm)
{
	m_vSurfNormal.resize(gm.GetSurfCount());

	for (unsigned i = 0; i < gm.GetSurfCount(); i++)
	{
		m_vSurfNormal[i] = tgl.CalcSurfaceNormal(gm.pSurfaces[i], gm.pVertices);
	}
}

void VBLaplaceMeshEdit::CalVertNormal(const VSGraphMesh & gm)
{
	CalSurfNormal(gm);
	m_vVertNormal.resize(gm.GetVertexCount());

	for (unsigned i = 0; i < gm.GetVertexCount(); i++)
	{
		m_vVertNormal[i] = tgl.CalcVertexNormal(gm, gm.pVertices, gm.pSurfaces, i);
	}
}

void VBLaplaceMeshEdit::FindOrthoEdge(const VSGraphMesh & gm)
{
	CalVertNormal(gm);

	m_vOrthoEdge.resize(gm.GetVertexCount());
	for (unsigned i = 0; i < gm.GetVertexCount(); i++)
	{
		std::pair<unsigned, float>  orthoEdge(VD_INVALID_INDEX, 10000.f);
		unsigned eCount = gm.GetEdgeCountOfVetex(i);
		for (unsigned j = 0; j < eCount; j++)
		{
			VTopoGraph::VOE voe = gm.GetEdgeOfVertex(i, j).Opposite();
			unsigned vidx = gm.GetVertIndxOfEdge(voe.edgeIndx)[voe.vertInEdge];
			VFVECTOR3 temp = gm.pVertices[vidx] - gm.pVertices[i];
			float dotVal = m_vVertNormal[i].Dot(temp);
			if (abs(orthoEdge.second) > abs(dotVal))
			{
				orthoEdge.first = vidx;
				orthoEdge.second = dotVal;				
			}
		}
		m_vOrthoEdge[i] = orthoEdge.first;		
	}
}

void VBLaplaceMeshEdit::OrthoEdgeProject(const VSGraphMesh & gm)
{
	m_vOrtho.resize(gm.GetVertexCount());

	for (unsigned i = 0; i < gm.GetVertexCount(); i++)
	{
		VFVECTOR3 vji = gm.pVertices[m_vOrthoEdge[i]] - gm.pVertices[i];
		VFVECTOR3 vec = vji - vji.Dot(m_vVertNormal[i]) * m_vVertNormal[i];
		VFVECTOR3 uji = vec * (1.f / vec.Magnitude());
		
		m_vOrtho[i] = uji;
	}	
}

void VBLaplaceMeshEdit::CalRotateParam(const VSGraphMesh & gm)
{
	m_vRotateParam.resize(gm.GetVertexCount());

	VFVECTOR3 temp = VFVECTOR3::ZERO;
	for (unsigned i = 0; i < gm.GetVertexCount(); i++)
	{
		temp.x = gm.pVertices[i].Dot(m_vVertNormal[i]);
		temp.y = gm.pVertices[i].Dot( m_vOrtho[i] );
		temp.z = gm.pVertices[i].Dot(m_vVertNormal[i].Cross(m_vOrtho[i]));

		m_vRotateParam[i] = temp;
	}	
}

void VBLaplaceMeshEdit::CreatSprsmat(const VSGraphMesh & gm, const VSConstBuffer<unsigned, VFVECTOR3>& destVert)
{
	m_vFlag.resize(gm.GetVertexCount());

	for (unsigned i = 0; i < destVert.len; i++)
		m_vFlag[destVert.pData1[i]] = 1;

	flag.len = VD_V2CB(m_vFlag).len;
	flag.pData = VD_V2CB(m_vFlag).pData;

	//Laplace.Build(matOut, gm, flag);
	matOut = m_objLaplace.Run(gm, flag).Get< VSSprsMatrixColMajor< float > >();
}

void VBLaplaceMeshEdit::CoordTransform(const VSGraphMesh & gm)
{
	m_vLaplaceVerts.resize(gm.GetVertexCount());

	for (unsigned i = 0; i < gm.GetVertexCount(); i++)
	{
		VFVECTOR3 sum = VFVECTOR3::ZERO;
		unsigned eCount = gm.GetEdgeCountOfVetex(i);
		for (unsigned j = 0; j < eCount; j++)
		{
			auto eov = gm.GetEdgeOfVertex(i, j).Opposite();
			unsigned vidx = gm.GetVertIndxOfEdge(eov.edgeIndx)[eov.vertInEdge];
			sum += gm.pVertices[vidx];
		}
		m_vLaplaceVerts[i] = gm.pVertices[i] - sum * (1.f / static_cast<float>(eCount));
	}
}

void VBLaplaceMeshEdit::CalDifferCord(const VSGraphMesh & gm)
{
	for (unsigned i = 0; i < gm.GetVertexCount(); i++)
	{
		m_vLaplaceVerts[i] = m_vRotateParam[i].x * m_vVertNormal[i] +
			                 m_vRotateParam[i].y * m_vOrtho[i]  +
			                 m_vRotateParam[i].z * (m_vVertNormal[i].Cross(m_vOrtho[i]));
	}
}

void VBLaplaceMeshEdit::EditVertPos( const VSGraphMesh & gmSrc , const VSConstBuffer<unsigned, VFVECTOR3> & destVert)
{
	VSGraphMesh gm( gmSrc ) ;
	
	FindOrthoEdge(gm);
	OrthoEdgeProject(gm);
	CoordTransform(gm);
	gm.pVertices = &m_vLaplaceVerts[0];
	CalRotateParam(gm);		
	CreatSprsmat(gm, destVert);

	unsigned count = 0;
	while (count < 3)
	{		
		CalDifferCord(gm);		
		for (unsigned i = 0; i < destVert.len; i++)
			m_vLaplaceVerts[destVert.pData1[i]] = destVert.pData2[i];

		mB.row = gm.GetVertexCount();
		mB.col = 3;
		mB.pData = &m_vLaplaceVerts[0].x;

		solve.Build(X, matOut, mB);
		_copy(m_vSolve, X);
		gm.pVertices = &m_vSolve[0];	
		CalVertNormal(gm);
		OrthoEdgeProject(gm);
		count++;
	}
}
//VD_EXPORT_SYSTEM_SIMPLE(VBLaplaceMeshEdit, VNALGMESH::VRLaplaceMeshEdit);
VD_EXPORT_SYSTEM_SIMPLE_L2(VBLaplaceMeshEdit, VNALGMESH::VRLaplaceMeshEditCreator);