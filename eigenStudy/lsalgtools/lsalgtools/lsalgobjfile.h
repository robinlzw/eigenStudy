#pragma once
#include "obj/alg/stdnotdef.h"
#include "vmath/vmath.h"
#include "vstd/tisys.h"


// 声明了一堆和输出的.obj文件相关的函数




VD_PERSISTCLASS_BEGIN(VSToothID)
	VD_DEFMEMBER(VSConstBuffer<char>, id)
VD_PERSISTCLASS_END();

VD_PERSISTCLASS_BEGIN(VSToothState)
	VD_DEFMEMBER(VSToothID, id)
	VD_DEFMEMBER(VFLocation, state)
VD_PERSISTCLASS_END();

VD_PERSISTCLASS_BEGIN(VSTooth)
	VD_DEFMEMBER(VSToothState, state)
	VD_DEFMEMBER(VSSimpleMeshF, simpMesh)
VD_PERSISTCLASS_END();

VD_PERSISTCLASS_BEGIN(VSJaw)
	VD_DEFMEMBER(VSConstBuffer<VSSimpleMeshF>, upperJaw)			// 这里的牙齿坐标为全局坐标
	VD_DEFMEMBER(VSConstBuffer<VSSimpleMeshF>, lowerJaw)
VD_PERSISTCLASS_END();

VD_PERSISTCLASS_BEGIN(VSPointInfo)
	VD_DEFMEMBER(unsigned, nToothIdx)
	VD_DEFMEMBER(unsigned, nVertexIdx)
VD_PERSISTCLASS_END();

typedef std::pair<VSPointInfo, VSPointInfo> VSPointPair;

VD_PERSISTCLASS_BEGIN(VSJawOcclusionInfo)
	VD_DEFMEMBER(VFLocation, location)		// 可移动的牙颌移动信息
	VD_DEFMEMBER(VSConstBuffer< VSPointPair >, relation)	// 上下颌接触点对<上颌顶点信息，下颌顶点信息>
VD_PERSISTCLASS_END();






// OBJ文件的IO接口
//		Read
extern void OBJReadJawFile(VSJaw& jaw, const char* pszFileName);
extern void OBJReadSimpMesh(VSSimpleMeshF& tooth, const char* pszFileName);
extern void OBJReadVertices(std::vector<VFVECTOR3>& vertices, const char* pszFileName);
extern void OBJReadFile(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs, const char* pszFileName);
extern void OBJReadStlFile(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs, 
								const std::string & fileName, const bool blIsAscii = false);


//		Write
extern void OBJWriteJaws(VSJaw& jaw, const char* pszFileName);
extern void OBJWriteCrowns(VSJaw& jaw, const VSJawOcclusionInfo& occludingInfo, const char* pszFileName);
extern void OBJWriteVertices(const VSConstBuffer<VFVECTOR3>& cbVertices, const char* pszFileName);
extern void OBJWriteSimpleMesh(const char* pszFileName, const VSSimpleMeshF& mesh);
extern void OBJWritePerfectMesh(const char* pszFileName, const VSPerfectMesh& mesh);
extern void OBJWriteToothMesh(const VSConstBuffer<VSTooth> & teeth, const std::string& strDstFileName);
extern void OBJAppendSimpleMesh(std::ofstream& dstFile, const unsigned nOffset, const VSSimpleMeshF& mesh);
extern void OBJWriteStlFile(const VSSimpleMeshF& simpMesh, const std::string & fileName, const bool blIsAscii = false);

//////////////////////////////////////////////////////////////////////////
