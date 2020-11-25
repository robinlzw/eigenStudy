#include "stdafx.h"
#include "lsalgtools/lsalgobjfile.h"
#include "VSTLFSaver.h"
#include "VSTLFReader.h"




void OBJWriteToothMesh(const VSConstBuffer<VSTooth> & teeth, const std::string& strDstFileName)
{
	std::ofstream dstFile(strDstFileName);
	unsigned nToothCount = teeth.len;
	std::vector<unsigned> vShift(nToothCount + 1, 0);
	for (unsigned i = 1; i < nToothCount; ++i)
	{
		unsigned nSize = teeth.pData[i].simpMesh.nVertCount;

		for (unsigned j = 0; j < nSize; j++)
		{
			char szBuf[256] = { 0 };
			VFVECTOR3 vert = teeth.pData[i].state.state.TransLocalVertexToGlobal(teeth.pData[i].simpMesh.pVertices[j]);
			sprintf_s(szBuf, 256, "v %f %f %f", vert.x, vert.y, vert.z);
			dstFile << szBuf << "\n";
		}
		vShift[i + 1] = vShift[i] + nSize;
	}

	for (unsigned i = 1; i < nToothCount; i++)
	{
		unsigned nSize = teeth.pData[i].simpMesh.nTriangleCount;
		for (unsigned j = 0; j < nSize; ++j)
		{
			char szBuf[256] = { 0 };
			const VNVECTOR3UI& tri = teeth.pData[i].simpMesh.pTriangles[j];
			sprintf_s(szBuf, 256, "f %d %d %d", tri.x + 1 + vShift[i], tri.y + 1 + vShift[i], tri.z + 1 + vShift[i]);
			dstFile << szBuf << "\n";
		}
	}
}

void OBJAppendSimpleMesh(std::ofstream& dstFile, const unsigned nOffset, const VSSimpleMeshF& mesh)
{
	unsigned nSize = mesh.nVertCount;

	for (unsigned j = 0; j < nSize; j++)
	{
		char szBuf[256] = { 0 };
		VFVECTOR3 vert = mesh.pVertices[j];
		sprintf_s(szBuf, 256, "v %f %f %f", vert.x, vert.y, vert.z);
		dstFile << szBuf << "\n";
	}

	nSize = mesh.nTriangleCount;
	unsigned nShift = nOffset + 1;
	for (unsigned j = 0; j < nSize; ++j)
	{
		char szBuf[256] = { 0 };
		const VNVECTOR3UI& tri = mesh.pTriangles[j];
		sprintf_s(szBuf, 256, "f %d %d %d", tri.x + nShift, tri.y + nShift, tri.z + nShift);
		dstFile << szBuf << "\n";
	}
}

void OBJWriteSimpleMesh(const char* pszFileName, const VSSimpleMeshF& mesh)
{
	std::ofstream dstFile(pszFileName);

	unsigned nSize = mesh.nVertCount;

	for (unsigned j = 0; j < nSize; j++)
	{
		char szBuf[256] = { 0 };
		VFVECTOR3 vert = mesh.pVertices[j];
		sprintf_s(szBuf, 256, "v %f %f %f", vert.x, vert.y, vert.z);
		dstFile << szBuf << "\n";
	}

	nSize = mesh.nTriangleCount;
	for (unsigned j = 0; j < nSize; ++j)
	{
		char szBuf[256] = { 0 };
		const VNVECTOR3UI& tri = mesh.pTriangles[j];
		sprintf_s(szBuf, 256, "f %d %d %d", tri.x + 1, tri.y + 1, tri.z + 1);
		dstFile << szBuf << "\n";
	}
}

void OBJWritePerfectMesh(const char* pszFileName, const VSPerfectMesh& mesh)
{
	std::ofstream dstFile(pszFileName);

	unsigned nSize = mesh.GetVertexCount();

	for (unsigned j = 0; j < nSize; j++)
	{
		char szBuf[256] = { 0 };
		VFVECTOR3 vert = mesh.pVertices[j];
		sprintf_s(szBuf, 256, "v %f %f %f", vert.x, vert.y, vert.z);
		dstFile << szBuf << "\n";
	}

	nSize = mesh.GetSurfCount();
	for (unsigned j = 0; j < nSize; ++j)
	{
		char szBuf[256] = { 0 };
		const VNVECTOR3UI& tri = mesh.pSurfaces[j];
		sprintf_s(szBuf, 256, "f %d %d %d", tri.x + 1, tri.y + 1, tri.z + 1);
		dstFile << szBuf << "\n";
	}
}
/////////////////////////////////////////////////////////////////////////
unsigned ReadNextValidData(char* & pszBuf, unsigned& nCount, char* validData, const unsigned nMaxSize)
{
	unsigned nIndx = 0;

	while ((pszBuf[0] == ' ') ||
		(pszBuf[0] == '\n') ||
		(pszBuf[0] == '\t') ||
		(pszBuf[0] == '\r'))
	{
		pszBuf++;
		nCount++;
	}

	while ((pszBuf[0] != ' ') &&
		(pszBuf[0] != '\n') &&
		(pszBuf[0] != '\t') &&
		(pszBuf[0] != '\r') &&
		(pszBuf[0] != '\null') &&
		(pszBuf[0] != 0) &&
		(nIndx < nMaxSize))
	{
		validData[nIndx++] = pszBuf[0];
		pszBuf++;
		nCount++;
	}
	validData[nIndx] = 0;
	return nIndx;
}

void OBJReadJawFile(VSJaw& jaw, const char* pszFileName)
{
	char* pTmp = NULL;
	std::ifstream ifs(pszFileName);//cube bunny Eight
	std::streampos   pos = ifs.tellg();     //   save   current   position   
	ifs.seekg(0, std::ios::end);
	unsigned fileLen = (unsigned)ifs.tellg();
	ifs.seekg(pos);     //   restore   saved   position   
	char* pFileBuf = new char[fileLen + 1];
	std::memset(pFileBuf, 0, fileLen + 1);
	ifs.read(pFileBuf, fileLen);
	char tmpBuffer[1024];
	unsigned nMaxSize = 1024;
	pTmp = pFileBuf;
	unsigned nReadLen = 0;
	unsigned nRet = 0;

	std::memset(&jaw, 0, sizeof(VSJaw));
	std::vector<std::vector<VFVECTOR3> > vvUpperVerts;
	std::vector<std::vector<VFVECTOR3> > vvLowerVerts;
	std::vector<std::vector<VFVECTOR3> >* pvvVerts = &vvUpperVerts;

	std::vector<std::vector<VNVECTOR3UI> > vvUpperSurfs;
	std::vector<std::vector<VNVECTOR3UI> > vvLowerSurfs;
	std::vector<std::vector<VNVECTOR3UI> >* pvvSurfs = &vvUpperSurfs;

	VSConstBuffer<VSSimpleMeshF>* pJaw = &jaw.upperJaw;
	bool blIsLower = true;
	do
	{
		nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
		if (0 == nRet)
			break;

		while (nReadLen < fileLen)
		{
			// 顶点信息			
			if (tmpBuffer[0] == 'g')
			{
				nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
				if (0 == nRet)
					break;
				if (strcmp(tmpBuffer, "upper") == 0)
				{
					// 上颌数据
					blIsLower = false;
					pJaw = &jaw.upperJaw;
					pvvVerts = &vvUpperVerts;
					pvvSurfs = &vvUpperSurfs;
				}
				else if (strcmp(tmpBuffer, "lower") == 0)
				{
					// 下颌数据
					blIsLower = true;
					pJaw = &jaw.lowerJaw;
					pvvVerts = &vvLowerVerts;
					pvvSurfs = &vvLowerSurfs;
				}

				nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
				if (0 == nRet)
					break;
			}

			if (tmpBuffer[0] == 'o')
			{
				nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
				if (0 == nRet)
					break;
				unsigned num = atoi(tmpBuffer);
				pvvVerts->push_back(std::vector<VFVECTOR3>());
				pvvSurfs->push_back(std::vector<VNVECTOR3UI>());
				pJaw->len++;
				unsigned nVertsCount = 0;
				std::vector<VFVECTOR3>& vertsRef = (*pvvVerts)[num];
				std::vector<VNVECTOR3UI>& surfsRef = (*pvvSurfs)[num];
				while (nReadLen < fileLen)
				{
					nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
					if (0 == nRet)
						break;
					if ('v' == tmpBuffer[0])
					{
						VFVECTOR3 vert;
						nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
						if (0 == nRet)
							break;
						vert.x = (float)atof(tmpBuffer);
						nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
						if (0 == nRet)
							break;
						vert.y = (float)atof(tmpBuffer);
						nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
						if (0 == nRet)
							break;

						vert.z = (float)atof(tmpBuffer);
						if (true == blIsLower)
							vert.z -= 0.1f;
						vertsRef.push_back(vert);
						nVertsCount++;
					}
					else if ('f' == tmpBuffer[0])
					{
						VNVECTOR3UI surf;
						nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
						if (0 == nRet)
							break;
						surf.x = atoi(tmpBuffer) + nVertsCount;
						nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
						if (0 == nRet)
							break;
						surf.y = atoi(tmpBuffer) + nVertsCount;

						nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
						if (0 == nRet)
							break;
						surf.z = atoi(tmpBuffer) + nVertsCount;
						surfsRef.push_back(surf);
					}
					else
						break;
				}
			}
			if (0 == tmpBuffer[0])
				break;
		}
	} while (0);

	delete[] pFileBuf;

	for (unsigned j = 0; j < 2; j++)
	{
		std::vector<std::vector<VFVECTOR3> >* pvvVertsRef = &vvUpperVerts;
		std::vector<std::vector<VNVECTOR3UI> >* pvvSurfsRef = &vvUpperSurfs;
		VSConstBuffer<VSSimpleMeshF>* pJawRef = &jaw.upperJaw;
		if (j > 0)
		{
			pvvVertsRef = &vvLowerVerts;
			pvvSurfsRef = &vvLowerSurfs;
			pJawRef = &jaw.lowerJaw;
		}
		unsigned nSize = pvvVertsRef->size();
		VSSimpleMeshF* pMesh = new VSSimpleMeshF[nSize];
		pJawRef->pData = pMesh;
		for (unsigned i = 0; i < nSize; ++i)
		{
			pMesh[i].nVertCount = (*pvvVertsRef)[i].size();
			pMesh[i].nTriangleCount = (*pvvSurfsRef)[i].size();

			VFVECTOR3* pVerts = new VFVECTOR3[pMesh[i].nVertCount];
			VNVECTOR3UI* pSurfs = new VNVECTOR3UI[pMesh[i].nTriangleCount];
			std::memcpy(pVerts, &(*pvvVertsRef)[i][0], sizeof(VFVECTOR3) * pMesh[i].nVertCount);
			std::memcpy(pSurfs, &(*pvvSurfsRef)[i][0], sizeof(VNVECTOR3UI) * pMesh[i].nTriangleCount);
			pMesh[i].pVertices = pVerts;
			pMesh[i].pTriangles = pSurfs;
		}
	}
}

void OBJReadSimpMesh(VSSimpleMeshF& tooth, const char* pszFileName)
{	
	std::memset(&tooth, 0, sizeof(VSSimpleMeshF));
	std::vector<VFVECTOR3> vVerts;
	std::vector<VNVECTOR3UI> vSurfs;

	OBJReadFile(vVerts, vSurfs, pszFileName);
	tooth.nVertCount = vVerts.size();
	tooth.nTriangleCount = vSurfs.size();
	VFVECTOR3* pVerts = new VFVECTOR3[tooth.nVertCount];
	VNVECTOR3UI* pSurfs = new VNVECTOR3UI[tooth.nTriangleCount];
	std::memcpy(pVerts, &vVerts[0], sizeof(VFVECTOR3) * tooth.nVertCount);
	std::memcpy(pSurfs, &vSurfs[0], sizeof(VNVECTOR3UI) * tooth.nTriangleCount);
	tooth.pVertices = pVerts;
	tooth.pTriangles = pSurfs;
}

void OBJReadFile(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs, const char* pszFileName)
{
	char* pTmp = NULL;
	std::ifstream ifs(pszFileName);//cube bunny Eight
	if (false == ifs.is_open())
	{
		return;
	}
	std::streampos   pos = ifs.tellg();     //   save   current   position   
	ifs.seekg(0, std::ios::end);
	unsigned fileLen = (unsigned)ifs.tellg();
	if (0 == fileLen)
	{
		return;
	}
	ifs.seekg(pos);     //   restore   saved   position   
	char* pFileBuf = new char[fileLen + 1];
	std::memset(pFileBuf, 0, fileLen + 1);
	ifs.read(pFileBuf, fileLen);
	char tmpBuffer[1024];
	unsigned nMaxSize = 1024;
	pTmp = pFileBuf;
	unsigned nReadLen = 0;
	unsigned nRet = 0;
		
	while (nReadLen < fileLen)
	{
		nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
		if (0 == nRet)
			break;
		// 顶点信息		
		if (std::strcmp(tmpBuffer, "v") == 0)
		{
			VFVECTOR3 vert;
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			vert.x = (float)atof(tmpBuffer);
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			vert.y = (float)atof(tmpBuffer);
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			vert.z = (float)atof(tmpBuffer);
			vVerts.push_back(vert);
		}
		else if (std::strcmp(tmpBuffer, "f") == 0)
		{
			VNVECTOR3UI surf;
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			surf.x = atoi(tmpBuffer) - 1;
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			surf.y = atoi(tmpBuffer) - 1;

			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			surf.z = atoi(tmpBuffer) - 1;
			vSurfs.push_back(surf);
		}
	}
	delete[] pFileBuf;
}

void OBJReadVertices(std::vector<VFVECTOR3>& vertices, const char* pszFileName)
{
	char* pTmp = NULL;
	std::ifstream ifs(pszFileName);//cube bunny Eight
	if (false == ifs.is_open())
	{
		return;
	}
	std::streampos   pos = ifs.tellg();     //   save   current   position   
	ifs.seekg(0, std::ios::end);
	unsigned fileLen = (unsigned)ifs.tellg();
	if (0 == fileLen)
	{
		return;
	}
	ifs.seekg(pos);     //   restore   saved   position   
	char* pFileBuf = new char[fileLen + 1];
	std::memset(pFileBuf, 0, fileLen + 1);
	ifs.read(pFileBuf, fileLen);
	char tmpBuffer[1024];
	unsigned nMaxSize = 1024;
	pTmp = pFileBuf;
	unsigned nReadLen = 0;
	unsigned nRet = 0;

	vertices.clear();

	while (nReadLen < fileLen)
	{
		nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
		if (0 == nRet)
			break;
		// 顶点信息		
		if (std::strcmp(tmpBuffer, "v") == 0)
		{
			VFVECTOR3 vert;
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			vert.x = (float)atof(tmpBuffer);
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			vert.y = (float)atof(tmpBuffer);
			nRet = ReadNextValidData(pTmp, nReadLen, tmpBuffer, nMaxSize);
			if (0 == nRet)
				break;
			vert.z = (float)atof(tmpBuffer);
			vertices.push_back(vert);
		}
		else
			break;
	}
	delete[] pFileBuf;
}

void WriteCrowns(const VSConstBuffer<VSSimpleMeshF>& jawRef,
	std::ofstream& dstFile)
{
	unsigned nToothCount = jawRef.len;

	for (unsigned toothIdx = 0; toothIdx < nToothCount; ++toothIdx)
	{
		unsigned nVertCount = jawRef.pData[toothIdx].nVertCount;
		dstFile << "o " << toothIdx << std::endl;
		for (unsigned vertIdx = 0; vertIdx < nVertCount; vertIdx++)
		{
			const VFVECTOR3& vert = jawRef.pData[toothIdx].pVertices[vertIdx];
			dstFile << "v " << vert.x << " " << vert.y << " " << vert.z << std::endl;
		}
		unsigned nSize = jawRef.pData[toothIdx].nTriangleCount;
		for (unsigned surfIdx = 0; surfIdx < nSize; surfIdx++)
		{
			const VNVECTOR3UI& surf = jawRef.pData[toothIdx].pTriangles[surfIdx];
			dstFile << "f " << ((int)surf.x - nVertCount) << " " <<
				((int)surf.y - nVertCount) << " " << ((int)surf.z - nVertCount) << std::endl;
		}
	}
}

void WriteCrowns(const VSConstBuffer<VSSimpleMeshF>& jawRef,
	const VSJawOcclusionInfo& occludingInfo,
	std::ofstream& dstFile)
{
	unsigned nToothCount = jawRef.len;

	for (unsigned toothIdx = 0; toothIdx < nToothCount; ++toothIdx)
	{
		unsigned nVertCount = jawRef.pData[toothIdx].nVertCount;
		dstFile << "o " << toothIdx << std::endl;
		for (unsigned vertIdx = 0; vertIdx < nVertCount; vertIdx++)
		{
			VFVECTOR3 vert = occludingInfo.location.TransLocalVertexToGlobal(jawRef.pData[toothIdx].pVertices[vertIdx]);
			dstFile << "v " << vert.x << " " << vert.y << " " << vert.z << std::endl;
		}
		unsigned nSize = jawRef.pData[toothIdx].nTriangleCount;
		for (unsigned surfIdx = 0; surfIdx < nSize; surfIdx++)
		{
			const VNVECTOR3UI& surf = jawRef.pData[toothIdx].pTriangles[surfIdx];
			dstFile << "f " << ((int)surf.x - nVertCount) << " " <<
				((int)surf.y - nVertCount) << " " << ((int)surf.z - nVertCount) << std::endl;
		}
	}
}

void WriteCrowns(const VSConstBuffer<VSSimpleMeshF>& jawRef,
	const VFLocation& location,
	std::ofstream& dstFile)
{
	unsigned nToothCount = jawRef.len;

	for (unsigned toothIdx = 0; toothIdx < nToothCount; ++toothIdx)
	{
		unsigned nVertCount = jawRef.pData[toothIdx].nVertCount;
		dstFile << "o " << toothIdx << std::endl;
		for (unsigned vertIdx = 0; vertIdx < nVertCount; vertIdx++)
		{
			VFVECTOR3 vert = location.TransLocalVertexToGlobal(jawRef.pData[toothIdx].pVertices[vertIdx]);
			dstFile << "v " << vert.x << " " << vert.y << " " << vert.z << std::endl;
		}
		unsigned nSize = jawRef.pData[toothIdx].nTriangleCount;
		for (unsigned surfIdx = 0; surfIdx < nSize; surfIdx++)
		{
			const VNVECTOR3UI& surf = jawRef.pData[toothIdx].pTriangles[surfIdx];
			dstFile << "f " << ((int)surf.x - nVertCount) << " " <<
				((int)surf.y - nVertCount) << " " << ((int)surf.z - nVertCount) << std::endl;
		}
	}
}


void OBJWriteCrowns(VSJaw& jaw,
	const VSJawOcclusionInfo& occludingInfo,
	const char* pszFileName)
{
	std::ofstream dstFile(pszFileName);

	VSConstBuffer<VSSimpleMeshF>* jawRef = &jaw.upperJaw;
	dstFile << "g upper" << std::endl;
	WriteCrowns(*jawRef, dstFile);

	jawRef = &jaw.lowerJaw;
	dstFile << "g lower" << std::endl;
	WriteCrowns(*jawRef, occludingInfo, dstFile);	
}

void WriteJaw(const VSConstBuffer<VSSimpleMeshF>& jawRef,
	std::ofstream& dstFile, const float flShift = 0.0f)
{
	if ((flShift < 0.000001f) && (flShift > -0.000001f))
	{
		WriteCrowns(jawRef, dstFile);
		return;
	}

	unsigned nToothCount = jawRef.len;

	for (unsigned toothIdx = 0; toothIdx < nToothCount; ++toothIdx)
	{
		unsigned nVertCount = jawRef.pData[toothIdx].nVertCount;
		dstFile << "o " << toothIdx << std::endl;
		for (unsigned vertIdx = 0; vertIdx < nVertCount; vertIdx++)
		{
			const VFVECTOR3& vert = jawRef.pData[toothIdx].pVertices[vertIdx];
			dstFile << "v " << vert.x << " " << vert.y << " " << (vert.z + flShift) << std::endl;
		}
		unsigned nSize = jawRef.pData[toothIdx].nTriangleCount;
		for (unsigned surfIdx = 0; surfIdx < nSize; surfIdx++)
		{
			const VNVECTOR3UI& surf = jawRef.pData[toothIdx].pTriangles[surfIdx];
			dstFile << "f " << ((int)surf.x - nVertCount) << " " <<
				((int)surf.y - nVertCount) << " " << ((int)surf.z - nVertCount) << std::endl;
		}
	}
}

void OBJWriteJaws(VSJaw& jaw,
	const char* pszFileName)
{
	std::ofstream dstFile(pszFileName);

	VSConstBuffer<VSSimpleMeshF>* jawRef = &jaw.upperJaw;
	dstFile << "g upper" << std::endl;

	WriteJaw(*jawRef, dstFile);

	jawRef = &jaw.lowerJaw;
	dstFile << "g lower" << std::endl;
	WriteJaw(*jawRef, dstFile, 8.0f);
}

void OBJWriteVertices(const VSConstBuffer<VFVECTOR3>& cbVertices, const char* pszFileName)
{
	std::ofstream dstFile(pszFileName);
	for (unsigned i = 0; i < cbVertices.len; i++)
	{
		dstFile << "v " << cbVertices.pData[i].x << " " << cbVertices.pData[i].y << " " << cbVertices.pData[i].z << std::endl;
	}
	dstFile.close();
}

void OBJWriteStlFile(const VSSimpleMeshF& simpMesh, const std::string & fileName, const bool blIsAscii)
{
	VSTLFSaver saver(fileName, blIsAscii);
	saver.Process(simpMesh);
}

void OBJReadStlFile(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs, 
	const std::string & fileName, const bool blIsAscii)
{
	VSTLFReader reader(fileName, blIsAscii);
	reader.Process(vVerts, vSurfs);
}

