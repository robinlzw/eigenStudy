#include "stdafx.h"
#include "VBMeshCurvature.h"

//
//VBMeshCurvature::VBMeshCurvature(const VSGraphMesh &mesh, VTopoGraphUtil &gutil)
//{
//    CalEigen(mesh, gutil);
//}

float VBMeshCurvature::CalAverEdgeLen(const VSGraphMesh & mesh)
{
	const unsigned & eCount = mesh.GetEdgeCount();
	float sum = 0.f;
	for (unsigned i = 0; i < eCount; i++)
	{
		const VNVECTOR2UI & vied = mesh.GetVertIndxOfEdge(i);
		sum += (mesh.pVertices[vied[0]] - mesh.pVertices[vied[1]]).Magnitude();
	}

	return sum / static_cast<float>(eCount);
}

float VBMeshCurvature::SurfNormalAngle(const VSGraphMesh &mesh, const unsigned &eidx)

{
	const unsigned & sCount = mesh.GetSurfCount();
	const VNVECTOR2UI & sied = mesh.GetSurfIndxOfEdge(eidx);

	while (sied[1] <= sCount)
	{
		const VNVECTOR2UI &vied = mesh.GetVertIndxOfEdge(eidx);

		const VNVECTOR3UI & vis0 = mesh.pSurfaces[sied[0]];
		const VNVECTOR3UI & vis1 = mesh.pSurfaces[sied[1]];

		VFVECTOR3 d0 = VTopoGraphUtil::CalcSurfaceNormal(vis0, mesh.pVertices);
		VFVECTOR3 d1 = VTopoGraphUtil::CalcSurfaceNormal(vis1, mesh.pVertices);

		VFVECTOR3 v1 = mesh.pVertices[vied[0]] - mesh.pVertices[vied[1]];
		VFVECTOR3 v2 = d0.Cross(d1);

		float vb = d0.Dot(d1) / (d0.Magnitude() * d1.Magnitude());
		float vDot = v1.Dot(v2);       //判断凹边或者凸边

		//确保vb的值位于[-1, 1]之间，防止浮点数造成越界
		vb = vb > 1.f ? 1.f : vb;
		vb = vb < -1.f ? -1.f : vb;
		float beta = acosf(vb);

		if (vDot > 0)
		{
			beta = -beta;
		}

		return beta;
	}

	return 0.f;
}

VFMatrix3 VBMeshCurvature::CommonEdgeMat(const VSGraphMesh &mesh, unsigned &eidx)
{
	const VNVECTOR2UI & vi = mesh.GetVertIndxOfEdge(eidx);
	VFVECTOR3         & ev = mesh.pVertices[vi[0]] - mesh.pVertices[vi[1]];

	float elen = ev.Magnitude();
	const VFVECTOR3 & ted = ev.Direction();

	float Beta = SurfNormalAngle(mesh, eidx);

	VFMatrix3 tempM(ted[0] * ted[0], ted[0] * ted[1], ted[0] * ted[2],
		            ted[1] * ted[0], ted[1] * ted[1], ted[1] * ted[2],
		            ted[2] * ted[0], ted[2] * ted[1], ted[2] * ted[2]);
	VFMatrix3 eMat = Beta * elen * tempM;

	return eMat;
}

VFMatrix3 VBMeshCurvature::SingleVertMat(const VSGraphMesh &mesh, const unsigned &vidx)
{
	VFMatrix3  gamaMatV(MAT3_ZERO);
	VFMatrix3  temp(MAT3_ZERO);

	const unsigned & N = mesh.GetEdgeCountOfVetex(vidx);
	unsigned boundECount = 0;
	for (unsigned i = 0; i < N; i++)
	{
		unsigned eid = mesh.GetEdgeIndxOfVertex(vidx, i);
		temp = CommonEdgeMat(mesh, eid);
		gamaMatV = (temp + gamaMatV);

		//判断是否存在边界边，若存在将边界边去除
		const unsigned & sCount = mesh.GetSurfCount();
		VNVECTOR2UI sidx = mesh.GetSurfIndxOfEdge(eid);
		if (sidx[1] > sCount)
		{
			boundECount++;
		}
	}
	unsigned subN = N - boundECount;
	subN = (subN > 0) ? subN : 1;

	return 1.f / static_cast<float>(subN) * gamaMatV;
}

void VBMeshCurvature::SmoothMat(const VSGraphMesh &mesh)
{
	VFMatrix3  temp(MAT3_ZERO);
	const unsigned & vn = mesh.GetVertexCount();

	m_vSmoothMat.resize(vn);
	m_vTempMat.resize(vn);

	float avgLen = CalAverEdgeLen(mesh);
	for (unsigned i = 0; i < vn; i++)
	{
		temp = SingleVertMat(mesh, i);
		temp = 1.f / avgLen * temp; 
		m_vSmoothMat[i] = temp;
	}
	unsigned mt = 0;
	while (mt < 3)
	{
		for (unsigned i = 0; i < vn; i++)
		{
			VFMatrix3  sumM(MAT3_ZERO);

			const unsigned & N = mesh.GetEdgeCountOfVetex(i);
			for (unsigned j = 0; j < N; j++)
			{
				const unsigned    & eid = mesh.GetEdgeIndxOfVertex(i, j);
				const VNVECTOR2UI & ei = mesh.GetVertIndxOfEdge(eid);

				if (ei[0] != i)
				{
					sumM = sumM + m_vSmoothMat[ei[0]];
				}
				else
				{
					sumM = sumM + m_vSmoothMat[ei[1]];
				}
			}
			sumM = sumM + m_vSmoothMat[i];
			m_vTempMat[i] = 1.f / static_cast<float>(N + 1) * sumM;
		}
		for (unsigned i = 0; i < vn; i++)
		{
			m_vSmoothMat[i] = m_vTempMat[i];
		}
		mt++;
	}
}

void VBMeshCurvature::CalEigen(const VSGraphMesh &mesh)
{
	VNWZKALG::VSVertEigenInfo  veif;

	const unsigned & vnum = mesh.GetVertexCount();
	m_vEigVert.resize(vnum);

	SmoothMat(mesh);

	for (unsigned i = 0; i < vnum; i++)
	{
		m_vSmoothMat[i].EigenSolveSymmetric(veif.eigValue, veif.eigVec);

		if (abs(veif.eigValue[0]) > abs(veif.eigValue[1]))
		{
			std::swap(veif.eigValue[0], veif.eigValue[1]);
			std::swap(veif.eigVec[0], veif.eigVec[1]);
		}
		if (abs(veif.eigValue[0]) > abs(veif.eigValue[2]))
		{
			std::swap(veif.eigValue[0], veif.eigValue[2]);
			std::swap(veif.eigVec[0], veif.eigVec[2]);
		}
		if (abs(veif.eigValue[1]) > abs(veif.eigValue[2]))
		{
			std::swap(veif.eigValue[1], veif.eigValue[2]);
			std::swap(veif.eigVec[1], veif.eigVec[2]);
		}
		if (veif.eigValue[1] > veif.eigValue[2])
		{
			std::swap(veif.eigValue[1], veif.eigValue[2]);
			std::swap(veif.eigVec[1], veif.eigVec[2]);
		}

		std::swap(veif.eigVec[1], veif.eigVec[2]);
		m_vEigVert[i] = veif;
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VBMeshCurvature, VNALGMESH::VRMeshCurvature);