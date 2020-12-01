#include "stdafx.h"
#include "vctoothpatchbtm.h"
#include <obj/lsalgtools/ilsalgtools.h>
#include <obj/lsalgtools/triangle.h>

void PatchToothBottom(unsigned& nVertCount, std::vector<VNVECTOR3UI>& vAddrSurfs,
	std::vector<unsigned>& vCtrlPoints, const VSConstBuffer<VFVECTOR3>& cbBndry)
{
	std::vector<VFVECTOR2> verts(cbBndry.len);
	std::vector< VNVECTOR2I > edges(cbBndry.len);

	float flAngleShift = VF_DBL_PI / (float)(cbBndry.len);
	float flAngle = 0.0f;
	for (unsigned i = 0; i < cbBndry.len; i++)
	{
		verts[i].x = std::cos(flAngle);
		verts[i].y = std::sin(flAngle);
		flAngle += flAngleShift;

		edges[i].x = i + 1;
		edges[i].y = (i + 1) % cbBndry.len + 1;
	}
	triangulateio triIn, triOut;
	triIn.numberofpoints = cbBndry.len;
	triIn.pointlist = (float *)&verts[0];
	triIn.numberofpointattributes = 0;
	triIn.pointattributelist = NULL;
	triIn.pointmarkerlist = NULL;

	triIn.numberofsegments = cbBndry.len;
	triIn.segmentlist = (int *)&edges[0];
	triIn.segmentmarkerlist = NULL;

	triIn.numberoftriangles = 0;
	triIn.numberofcorners = 0;
	triIn.numberoftriangleattributes = 0;

	triIn.numberofholes = 0;
	triIn.holelist = NULL;

	triIn.numberofregions = 0;
	triIn.regionlist = NULL;

	std::vector< VFVECTOR2 > outverts(cbBndry.len);
	std::vector< VNVECTOR2I > outsegments(cbBndry.len);
	std::vector< VNVECTOR3I > outtris(cbBndry.len * 2);
	memset(&triOut, 0, sizeof(triangulateio));
	//triOut.numberofpoints = cbBndry.len;
	//triOut.pointlist = (float *)&outverts[0];
	//triOut.pointattributelist = NULL;
	//triOut.pointmarkerlist = NULL;

	//triOut.numberoftriangles = outtris.size();
	//triOut.trianglelist = (int *)&outtris[0];
	//triOut.numberoftriangleattributes = 0;
	//triOut.triangleattributelist = NULL;

	//triOut.numberofsegments = cbBndry.len;
	//triOut.segmentlist = (int *)&outsegments[0];
	//triOut.segmentmarkerlist = NULL;
	char szBuf[256] = { 0 };
	sprintf_s(szBuf, "pa%fq30Y", 5.0f * (verts[0] - verts[1]).SqrMagnitude());
	triangulate(szBuf, &triIn, &triOut, NULL);
	vAddrSurfs.resize((unsigned)triOut.numberoftriangles);
	nVertCount = triOut.numberofpoints;
	//if (dir.Dot(VFVECTOR3::AXIS_Y) < 0.f)
	{
		for (unsigned i = 0; i < (unsigned)triOut.numberoftriangles; i++)
		{
			vAddrSurfs[i].x = (unsigned)triOut.trianglelist[3 * i] - 1;
			vAddrSurfs[i].y = (unsigned)triOut.trianglelist[3 * i + 2] - 1;
			vAddrSurfs[i].z = (unsigned)triOut.trianglelist[3 * i + 1] - 1;
			//vAddrSurfs[i].x = (unsigned)outtris[i].x - 1;
			//vAddrSurfs[i].y = (unsigned)outtris[i].y - 1;
			//vAddrSurfs[i].z = (unsigned)outtris[i].z - 1;
		}
	}
	outverts.resize(triOut.numberofpoints);
	VFVECTOR2 centralVert = VFVECTOR2::ZERO;
	for (unsigned i = 0; i < (unsigned)triOut.numberofpoints; i++)
	{
		outverts[i].x = (float)triOut.pointlist[2 * i];
		outverts[i].y = (float)triOut.pointlist[2 * i + 1];		
		centralVert += outverts[i];
	}
	centralVert /= (float)triOut.numberofpoints;
	double minRange = BTM_CTRL_RADIUS; //(outverts[0] - centralVert).Magnitude();
	//nCenterIndex = 0;
	double dbDiff = DBL_MAX;
	unsigned nMinIdx = 0;
	vCtrlPoints.resize((unsigned)triOut.numberofpoints, VD_INVALID_INDEX);
	for (unsigned i = 0; i < (unsigned)triOut.numberofpoints; i++)
	{
		float flVal = (outverts[i] - centralVert).Magnitude();
		if (flVal < minRange)
		{
			vCtrlPoints[i] = E_CTRLPOINT;
		}
		if (dbDiff > flVal)
		{
			dbDiff = flVal;
			nMinIdx = i;
		}
	}
	vCtrlPoints[nMinIdx] = E_CTRLPOINT_CENTER;
	//else
	//{
	//	for (unsigned i = 0; i < (unsigned)triOut.numberoftriangles; i++)
	//	{
	//		rslt[i].x = (unsigned)outtris[i].x - 1;
	//		rslt[i].y = (unsigned)outtris[i].z - 1;
	//		rslt[i].z = (unsigned)outtris[i].y - 1;
	//	}
	//}
}

void ExtendToothBottomCtrlPoints(std::vector<VFVECTOR3>& outVerts, 
	NMALG_TEETHPATCHING::VSToothAxis& toothAxis,
	const VSConstBuffer<unsigned>& cbCtrlPoints, 
	const VSConstBuffer<unsigned>& cbGumLine, const VSPerfectMesh& pm)
{
	//build a plane
	float flDiff = FLT_MAX;
	const VFVECTOR3& dir = toothAxis.m_TipRay.GetDirection();
	const VFVECTOR3& root = toothAxis.m_TipRay.GetOrigin();
	for (size_t i = 0; i < cbGumLine.len; i++)
	{
		float flVal = (pm.pVertices[cbGumLine.pData[i]] - root).Dot(dir);
		if (flVal < flDiff)
		{
			flDiff = flVal;
		}
	}
	VFVECTOR3 mPoint = toothAxis.m_TipRay.GetPoint(flDiff - 1.0f);// -toothAxis.m_RootBackDis);
	VFPlane mPlane(toothAxis.m_TipRay.GetDirection() ,mPoint);

	//projection and save
	VFVECTOR3 dotInPlane = -mPlane.d * mPlane.normal;
	for (unsigned i = 0; i < cbCtrlPoints.len; i++)
	{
		VFVECTOR3 dif = dotInPlane - pm.pVertices[cbCtrlPoints.pData[i]];
		dif = dif.Dot(mPlane.normal) * mPlane.normal;
		outVerts[i] = pm.pVertices[cbCtrlPoints.pData[i]] + dif;
	}
}
