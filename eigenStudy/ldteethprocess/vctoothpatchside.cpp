#include "stdafx.h"
#include <pkg/ldck4cce.h>
#include <pkg/ldwzkupdater.h>
#include <obj/lsalgtools/triangle.h>
#include "vctoothpatchside.h"
#include "vctoothmakematrixs.h"

void PatchToothSide(unsigned& nVertCount, std::vector<VNVECTOR3UI>& vSurfaces, unsigned& nCenterIndex,
	const VSConstBuffer<unsigned>& cbBndry, const unsigned addVertCount)
{
	unsigned nTotalCount = cbBndry.len + addVertCount;
	std::vector<VFVECTOR2> verts(nTotalCount);
	std::vector< VNVECTOR2I > edges(nTotalCount);

	double flAngleShift = VD_PI / (double)(cbBndry.len-1);
	double flAngle = 0.0f;
	for (unsigned i = 0; i < cbBndry.len; i++)
	{
		verts[i].x = std::cos(flAngle);
		verts[i].y = std::sin(flAngle);
		flAngle += flAngleShift;

		edges[i].x = i + 1;
		edges[i].y = (i + 1) + 1;
	}
	float mSpace = 2.f / (addVertCount + 1);
	for (unsigned j = 0; j < addVertCount; j++)
	{
		unsigned k = j + cbBndry.len;
		verts[k].x = (j + 1) * mSpace - 1.f;
		verts[k].y = 0;
		
		edges[k].x = k + 1;
		edges[k].y = (k + 1) % (nTotalCount) + 1;
	}
	triangulateio triIn, triOut;
	triIn.numberofpoints = nTotalCount;
	triIn.pointlist = (float *)&verts[0];
	triIn.numberofpointattributes = 0;
	triIn.pointattributelist = NULL;
	triIn.pointmarkerlist = NULL;

	triIn.numberofsegments = nTotalCount;
	triIn.segmentlist = (int *)&edges[0];
	triIn.segmentmarkerlist = NULL;

	triIn.numberoftriangles = 0;
	triIn.numberofcorners = 0;
	triIn.numberoftriangleattributes = 0;

	triIn.numberofholes = 0;
	triIn.holelist = NULL;

	triIn.numberofregions = 0;
	triIn.regionlist = NULL;

	std::vector< VFVECTOR2 > outverts(nTotalCount);
	std::vector< VNVECTOR2I > outsegments(nTotalCount);
	std::vector< VNVECTOR3I > outtris(nTotalCount * 2);
	memset(&triOut, 0, sizeof(triangulateio));
	//triOut.numberofpoints = nTotalCount;
	//triOut.pointlist = (float *)&outverts[0];
	//triOut.pointattributelist = NULL;
	//triOut.pointmarkerlist = NULL;

	//triOut.numberoftriangles = outtris.size();
	//triOut.trianglelist = (int *)&outtris[0];
	//triOut.numberoftriangleattributes = 0;
	//triOut.triangleattributelist = NULL;

	//triOut.numberofsegments = nTotalCount;
	//triOut.segmentlist = (int *)&outsegments[0];
	//triOut.segmentmarkerlist = NULL;
	char szBuf[256] = { 0 };
	double flAvgEdgeLen = 0.0f;
	for (unsigned i =0; i <verts.size(); i++)
	{
		flAvgEdgeLen += (verts[i] - verts[(i + 1) % verts.size()]).Magnitude();
	}
	flAvgEdgeLen /= (double)verts.size();
	double dbMaxArea = flAvgEdgeLen * flAvgEdgeLen*10.0;
	sprintf_s(szBuf, "pa%fq30Y", dbMaxArea);
	triangulate(szBuf, &triIn, &triOut, NULL);
	vSurfaces.resize(triOut.numberoftriangles);
	nVertCount = triOut.numberofpoints;
	for (unsigned i = 0; i < (unsigned)triOut.numberoftriangles; i++)
	{
		vSurfaces[i].x = (unsigned)triOut.trianglelist[3 * i] - 1;
		vSurfaces[i].y = (unsigned)triOut.trianglelist[3 * i + 2] - 1;
		vSurfaces[i].z = (unsigned)triOut.trianglelist[3 * i + 1] - 1;
		/*vSurfaces[i].x = (unsigned)outtris[i].x - 1;
		vSurfaces[i].y = (unsigned)outtris[i].y - 1;
		vSurfaces[i].z = (unsigned)outtris[i].z - 1;*/
	}
	outverts.resize(triOut.numberofpoints);
	VFVECTOR2 centralVert = VFVECTOR2::ZERO;
	for (unsigned i = 0; i < (unsigned)triOut.numberofpoints; i++)
	{
		outverts[i].x = (float)triOut.pointlist[2 * i];
		outverts[i].y = (float)triOut.pointlist[2 * i + 1];
		centralVert += outverts[i];
	}
	centralVert  /= triOut.numberofpoints;
	double minRange = (outverts[0] - centralVert).Magnitude();
	nCenterIndex = 0;
	for (unsigned i = 1; i < (unsigned)triOut.numberofpoints; i++)
	{
		if (minRange > ((outverts[i] - centralVert).Magnitude()))
		{
			minRange = (outverts[i] - centralVert).Magnitude();
			nCenterIndex = i;
		}
	}
#if 0
	{
		char szFileName[256] = { 0 };
		static int nCount = 0;
		sprintf_s(szFileName, 256, "D:/testdata/halfcycle_%d.obj", nCount++);
		std::ofstream objFile(szFileName);
		//objFile << "center: " << nCenterIndex << std::endl;
		//objFile << "bndry len: " << cbBndry.len << " add count: " << addVertCount << std::endl;
		//objFile << "vert count: " << verts.size() << " out verts count: " << outverts.size()  << std::endl;

		for (size_t i = 0; i < outverts.size(); i++)
		{
			objFile << "v " << outverts[i].x << " " << outverts[i].y << " 0.0" << std::endl;
		}
		for (size_t i = 0; i < vSurfaces.size(); i++)
		{
			objFile << "f " << vSurfaces[i].x+1 << " " << vSurfaces[i].y +1<< " " << vSurfaces[i].z+1 << std::endl;
		}
		objFile.close();
		;
	}
#endif
}

void CalcToothSideVertices(std::vector<VFVECTOR3>& vVertices, const VSConstBuffer<VFVECTOR3>& cbBndryVerts,
	const VSConstBuffer<VNVECTOR3UI>& cbSurfaces, const unsigned nVertCount)
{
#if 1
	TVExtSource< VNALGEQ::VRSprsResolverD > extSprsResolver;
	TVExtSource< VNALGEQ::VRTriple2MatrixD > extTriple2Matrix;
	TVR2B< VNALGEQ::VRSprsResolverD > sprsResolver(*extSprsResolver);
	TVR2B< VNALGEQ::VRTriple2MatrixD > triple2Matrix(*extTriple2Matrix);

	VSSprsMatrixColMajor< double >	A;
	VSDnsMatData< double, true >	B;
	VSDnsMatData< double, true >	X;
	VSSprsMatData< double >	ATriple;

	double* vRight = new double[nVertCount * 3];
	memset(vRight, 0, sizeof(double) * nVertCount * 3);
	
	std::vector< VSTriplet<double> > vLMatrix;
	::MakeMatrixBndry(vLMatrix, cbBndryVerts, cbSurfaces, nVertCount);

	unsigned nShift1 = nVertCount;
	unsigned nShift2 = nVertCount + nShift1;
	for (size_t i = 0; i < cbBndryVerts.len; i++)
	{
		vRight[i] = cbBndryVerts.pData[i].x;
		vRight[nShift1 + i] = cbBndryVerts.pData[i].y;
		vRight[nShift2 + i] = cbBndryVerts.pData[i].z;
	}

	ATriple.row = ATriple.col = nVertCount;
	ATriple.val = VD_V2CB(vLMatrix);

	B.row = nVertCount;
	B.col = 3;
	B.pData = vRight;
	A = triple2Matrix.Run(ATriple).Get<VSSprsMatrixColMajor< double > >();
	X = sprsResolver.Run(A, B).Get<VSDnsMatData< double, true > >();

	vVertices.resize(nVertCount);
	for (size_t i = 0; i < nVertCount; i++)
	{
		vVertices[i].x = X.pData[i];
		vVertices[i].y = X.pData[i + nShift1];
		vVertices[i].z = X.pData[i + nShift2];
	}
#endif
}