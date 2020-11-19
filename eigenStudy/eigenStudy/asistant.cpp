#include "asistant.h"



using namespace std;
using namespace Eigen;

void v3disp(const VFVECTOR3& v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}




// 写一个接口，将矩阵数据保存到.dat文件中，方便python读取然后画图
void writeData2D(const VectorXd& x, const VectorXd& y, const char* filename)
{
	// 顺序挨个写入x和y向量中的数据，先写x再写y，因为两条向量是对应的，所以肯定前一半是x坐标，后一半是y坐标。
	double darr1[MAXLEN];
	double darr2[MAXLEN];
	unsigned int size = x.rows();
	string str1 = filename;
	string str2 = str1;
	
	auto iter = find(str1.begin(), str1.end(), '.');
	if (iter == str1.end())
	{
		cout << "错误，输出的二进制文件必须有后缀名。" << endl;
		return;
	}


	auto dis = distance(str1.begin(), iter);
	str1.insert(dis, "_x");
	str2.insert(dis, "_y");


	for (unsigned int i = 0; i < size; i++)
	{
		darr1[i] = x(i);
	}
	for (unsigned int i = 0; i < size; i++)
	{
		darr2[i] = y(i);
	}

	ofstream file1(str1, ios::out | ios::binary);
	ofstream file2(str2, ios::out | ios::binary);

	file1.write(reinterpret_cast<char*>(&darr1[0]), size * sizeof(double));
	file2.write(reinterpret_cast<char*>(&darr2[0]), size * sizeof(double));
	file1.close();
	file2.close();
}



void readData(VectorXd& x, const char* filename)
{
	ifstream file(filename, ios::in | ios::binary);
	file.seekg(0, file.end);					// 追溯到文件流的尾部
	unsigned int size = file.tellg();			// 获取文件流的长度。
	file.seekg(0, file.beg);					// 回到文件流的头部	

	// 这一块以后考虑用alloctor改写
	char* pc = (char*)malloc(size);
	file.read(pc, size);

	double* pd = reinterpret_cast<double*>(pc);
	for (unsigned int i = 0; i < size/sizeof(double); i++) 
	{
		x[i] = *pd;
		pd++;
	}

}



void writeData3D()
{

}



void writeOBJ()
{}



void readOBJ()
{

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