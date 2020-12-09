#include "projectDS.h"
// 工作项目中的自定义数据结构



using namespace Eigen;     // 改成这样亦可 using Eigen::MatrixXd; 
using namespace std;
const double pi = 3.14159;

#define PI 3.14159



namespace PDS 
{


// 测试VSSimpleMeshF类：
/*
	是一个行为像指针的类，和VSConstBuffer类似。
	指向的数据实体是NM_PMMESH::VSMesh对象。
	20201203：
			目前感觉simpleMesh作为一个pointer-like类并没有设计得足够完善，没有实现引用计数，也没有定义包含delete操作的析构函数。
			常用的在程序中创建三维网格实体的方法是使用OBJReadSimpMesh()接口，生成的是堆对象。由于simpleMesh不会自动delete，使用起来有风险。目前考虑用其他方法从文件中读取网格数据。
*/
void test1()
{
	VSSimpleMeshF mesh1, mesh2, tooth;

	generateMeshWrong(mesh1);
	generateMeshRight(mesh2);

	v3disp(mesh1.pVertices[0]);			// 栈网格对象在Wrong()函数结束之后生命周期结束，然后被释放，mesh1指针变成野指针。
	v3disp(mesh2.pVertices[0]);			// 堆网格对象在Right()函数结束之后依然存在。


	delete[] mesh1.pVertices;
	delete[] mesh2.pVertices;

	OBJReadSimpMesh(tooth, "tooth.obj");			// 读取OBJ文件，返回simpleMesh网格指针对象，网格数据实体在堆上。


	cout << "finished." << endl;
}
	


// 函数体内生成栈上网格对象，输出网格指针的函数————错误示范
bool generateMeshWrong(VSSimpleMeshF& mesh) 
{

	NM_PMMESH::VSMesh vmesh;
	vmesh.vVertice.push_back(VFVECTOR3(1,2,3));

	PMGetSimpMesh(mesh, vmesh);

	return true;
}



// 函数体内生成堆上的网格对象，输出网格指针的函数——正确示范
bool generateMeshRight(VSSimpleMeshF& mesh) 
{
	NM_PMMESH::VSMesh* pmesh = new NM_PMMESH::VSMesh;

	pmesh->vVertice.push_back(VFVECTOR3(1, 2, 3));

	PMGetSimpMesh(mesh, *pmesh);
	return true;

}






//	测试VSConstBuffer类模板
/*
	是一个行为像指针的类，封装的是数据的指针。
		成员数据
			unsigned    len;		元素数
			const T *   pData;		数据的指针，注意是底层const，不可以通过指针修改数据。

	生成某个数据对象对应的VSConstBuffer对象的过程中，不发生数据的拷贝。
	VSConstBuffer是一个行为像指针的类。
	对VSConstBuffer对象的拷贝是浅拷贝。
*/
void test2()
{
	int arri[] = { 1,2,3,4,5 };
	VSConstBuffer<int> buffer(5, arri);		// 带参构造。


	// 使用下标运算符访问VCBuffer中的元素：
	for (int i = 0; i < buffer.len; i++) 
	{
		cout << buffer.pData[i] << "   ";
	}
	cout << endl;



	// VD_V2CB()——STL向量转换为buffer对象，vector to const buffer
	/*
		VSConstBuffer< T > VD_V2CB( const std::vector< T > & v )
	*/
	vector<string> vec_str;
	vec_str.push_back("hahahaha");
	vec_str.push_back("wawawa");

	auto buffer_str = VD_V2CB(vec_str);
	cout << "len == " << buffer_str.len << endl;

	const string* pstr = buffer_str.pData;
	cout << *pstr++ << endl;
	cout << *pstr << endl;


	
	// 生成某个数据对象对应的VBuffer对象时，不发生数据的拷贝。
	//		VBuffer是一个行为像指针的类。
	int numi = 999;
	VSConstBuffer<int> ibuffer(1, &numi);
	cout << "*pData == " << *ibuffer.pData << endl;

	numi = 888;
	cout << "*pData == " << *ibuffer.pData << endl;





	


}



// 测试TVPersist类模板
/*
成员数据(private)
	std::vector< char > m_buff;			
		序列化后的数据，被序列化的数据被逐字节地写入到本vector对象中。

 
对外接口
	Get()
		返回封装数据的指针，为T*类型
	Build()
		接受T类型的数据，序列化为自己的成员数据m_buff
	GetBuffer()
		将成员数据m_buff向量封装成buffer然后返回。
*/
void test3()
{
	TVPersist<VFVECTOR3> vp;
	VFVECTOR3 v1(1, 2, 3);

	vp.Build(v1);		// 将参数数据序列化，存入到本序列化对象的m_buff成员向量之中。


	VFVECTOR3 v2 = vp.Get();								// 返回序列化之前的数据。
	VSConstBuffer<char> buffer_per = vp.GetBuffer();		// 返回序列化的数据，即m_buff成员变量。

	v3disp(v2);


	// 看一下序列化后的数据是什么样的。
	const char* pc = buffer_per.pData;
	cout << "序列化之后生成的TVPersist对象的字节数：" << sizeof(vp) / sizeof(char) << endl;
	cout << "序列化之前的数据字节数：" << sizeof(v1) << endl;
	for (int i = 0; i < buffer_per.len; i++)
	{
		cout << *pc << endl;
		pc++;
	}


	// 序列化过程中发生了拷贝。
	float numf = 9.999999;
	TVPersist<float> fper;
	
	cout << "序列化前：sizeof(iper) == " << sizeof(fper) << endl;


	fper.Build(numf);


	numf = 8.88888;
	float result = fper.Get();
	cout << "序列化后：sizeof(iper) == " << sizeof(fper) << endl;
	cout << "result == " << result << endl;	
	cout << "numf == " << numf << endl;

}



// 测试VFQuaternion四元数类
void test4() 
{
	VFQuaternion q1(1, 2, 3, 4);

	// 1. 输入旋转轴向量、旋转角度, 生成四元数对象
	VFQuaternion q2 = VFQuaternion::FromAxisAngle(VFVECTOR3::AXIS_Z, pi/2);
	v3disp(q2*VFVECTOR3(1, 0, 0));


	// 2. 输入目标坐标轴方向向量，生成四元数对象
	VFVECTOR3	x_new, y_new, z_new;
	x_new = VFVECTOR3(1,2,3);
	y_new = VFVECTOR3(0,-3,2);
	z_new = x_new.Cross(y_new);
	x_new.Normalize();
	y_new.Normalize();
	z_new.Normalize();

	VFQuaternion q = VFQuaternion::FromAxes(x_new, y_new, z_new);
	v3disp(q*VFVECTOR3::AXIS_X);


	// 3. 输入原始向量和目标向量，生成四元数对象
	q = VFQuaternion::FromRotateAxis(VFVECTOR3::AXIS_X, VFVECTOR3::AXIS_Y);
	v3disp(q*VFVECTOR3::AXIS_X);
	v3disp(q*VFVECTOR3::AXIS_Y);
	v3disp(q*VFVECTOR3::AXIS_Z);


}



// 测试VFLocation类
void test5()
{
	// 使用VFLocation对象将一个牙齿网格旋转然后平移。

	//VFLocation loc();


}



// 测试TVBuilderArgTuple类模板
void test6()
{
	TVBuilderArgTuple<string> tuple;		// 一个处理string类型数据的argtuple容器
	
	// 1. 生成待处理数据，序列化后写入到文件中
	string str = "hello world hahahahaha";
	TVPersist<string> pstr;
	pstr.Build(str);
	fstream file1("persist_data.dat", ios::out|ios::binary);
	file1 << pstr;
	file1.close();


	// 2. argtuple容器读取文件中的序列化数据
	ifstream file2("persist_data.dat", ios::in | ios::binary);
	tuple.Resume(file2);
	file2.close();


	// 3. 传入函数子处理argtuple中的数据		————打印字符串
	tuple.Travel([](const string& str0)		// 函数子的形参必须是const的，类型应该就是argtuple中序列化数据的原类型
	{
		// argtuple使用函数子对内部数据执行的操作是只读操作。
		cout << str0.length() << endl;
		cout << str0 << endl;

	});




	cout << "finished " << endl;


}



// 测试VFMatrix3类
void test7() 
{
	


}



// 测试TVR2B类
void test8() 
{
	//TVR2B<VFVECTOR3>	v1();
	//
	//VFVECTOR3 vec1(1,2,3);
	//VFQuaternion q1(1, 2, 3, 4);

	//v1.Build(vec1, q1);


}





// 测试碰撞检测类VBCollisionSence
void test9() 
{
	//VGlEnvCreator<4, 5> m_GLCreator;
	//std::shared_ptr< TVExtSysUser< VNCollisionTest::VRCollisionExtCreator > > m_SysUserCollisionTestCreator;
	//VSESS< VNCollisionTest::VRCollisionExt >  m_EssSysCollisionTest;
	//m_SysUserCollisionTestCreator.reset(new TVExtSysUser< VNCollisionTest::VRCollisionExtCreator >(*m_GLCreator.m_pGl));
	//m_SysUserCollisionTestCreator->Elapse([&m_EssSysCollisionTest](const VSESS< VNCollisionTest::VRCollisionExt > & ess) {
	//	m_EssSysCollisionTest = ess;
	//});

}




// 测试射线检测类：
void test10() 
{
	NMALG_OBJECTCONTAINERPROXY::VCProxy::Initialize();
	NMALG_OBJECTCONTAINERPROXY::VCProxy objproxy;

	NMALG_MESHRAYINTERSECTION::VSMeshRayIntersectInput rayIn;
	NMALG_MESHRAYINTERSECTION::VSMeshRayIntersectOutput	rayOut;
	NMALG_MESHRAYINTERSECTION::IVMeshRayIntersection* pRay = (NMALG_MESHRAYINTERSECTION::IVMeshRayIntersection*)objproxy.CreateObj(OBJ_MESHRAYINTERSECTION_GEN_E);;


	VSSimpleMeshF	tooth, tooth1;
	OBJReadSimpMesh(tooth, "tooth.obj");
	OBJReadSimpMesh(tooth1, "tooth_01.obj");

	typedef NM_PMMESH::VSMesh VSMesh;
	VSMesh vmesh(tooth);
	VSMesh vmesh1(tooth1);


	// 求两个网格中心点。
	VFVECTOR3 center(0, 0, 0), center1(0, 0, 0);
	for (int i = 0; i<tooth.nVertCount; i++)
	{
		center += tooth.pVertices[i];
	}
	center /= tooth.nTriangleCount;

	for (int i = 0; i<tooth1.nVertCount; i++)
	{
		center1 += tooth1.pVertices[i];
	}
	center1 /= tooth1.nTriangleCount;


	// 确定测量方向————应该是光源指向被测量物体
	VFVECTOR3 dir = center1 - center;
	dir.Normalize();


	// 生成射线对象的buffer：
	vector<VFRay> vec_ray;
	VSConstBuffer<VFRay> buffer_ray;
	for (int i = 0; i < tooth.nVertCount; i++)
	{
		VFRay rayTemp(tooth.pVertices[i], dir);
		vec_ray.push_back(rayTemp);
	}
	buffer_ray = VD_V2CB(vec_ray);




	// 构造射线测距过程的输入对象。
	rayIn.simpMesh = tooth1;
	rayIn.rays = buffer_ray;

	// 执行射线测距过程
	pRay->Build(rayOut, rayIn);


	// 解析射线测距输出对象：
	/*
	VSConstBuffer<VSConstBuffer<float>>, cbRayLen				射线正方向与网格交点：ray.origin + ray.direction * len
	VSConstBuffer<VSConstBuffer<unsigned>>, cbSurfIdx			？？？射线正方向与网格相交的网格上的顶点索引，与cbRayLen一一对应
	VSConstBuffer<VSConstBuffer<float>>, cbOpRayLen				射线反方向与网格交点：ray.origin + ray.direction * len
	VSConstBuffer<VSConstBuffer<unsigned>>, cbOpSurfIdx			射线反方向与网格相交的三角片，与cbOpRayLen一一对应
	*/
	const VSConstBuffer<float>* pcf = NULL;
	const VSConstBuffer<unsigned>* pcu = NULL;
	cout << "tooth.nVertCount == " << tooth.nVertCount << endl;
	cout << "rayOut.cbRayLen.len == " << rayOut.cbRayLen.len << endl;

	pcf = &rayOut.cbRayLen.pData[0];					// 第一个点测距后得到的距离数据。是float的Vbuffer

														//		正向射线穿过目标网格，有两个交点——位于前表面、后表面。
	cout << "正向射线和目标网格交点个数 == " << pcf->len << endl;
	cout << "测得距离分别为：" << pcf->pData[0] << ", " << pcf->pData[1] << endl;


	//		反向射线没有穿过目标网格：
	pcf = &rayOut.cbOpRayLen.pData[0];
	cout << "反向射线和目标网格交点个数 == " << pcf->len << endl;


	//		正向射线和目标网格交点所在的三角片：
	pcu = &rayOut.cbSurfIdx.pData[0];				// 第一个点测距后得到的三角片数据。
	cout << "tooth1.nTriangleCount == " << tooth1.nTriangleCount << endl;
	cout << "正向射线和目标网格交点个数 == " << pcu->len << endl;
	cout << "交点在目标网格上的所在的三角片索引分别为：" << pcu->pData[0] << ", " << pcu->pData[1] << endl;
	VNVector3<unsigned> vn;
	vn = tooth1.pTriangles[pcu->pData[0]];
	vndisp<unsigned>(vn);


}



// 测试切割路径
enum ErrorCode
{
	CUTPATH_GEN_OK = 0,
	CUTPATH_GEN_FAIL_EXTRACTED = 1,
	CUTPATH_GEN_FAIL_CUTNORM = 2,
	CUTPATH_GEN_FAIL_FIND = 3
};




void test11() 
{
	TVPersist<NMALG_CUTPATHGEN::VSCutPathGenInput> perstGenInput;
	
	// 读取文件中的序列化数据对象——VSCutPathGenInput对象的序列化数据
	std::ifstream datF("E:/cutPath/cutpathparaminputLower_1.dat", std::ios::binary);
	datF >> perstGenInput;
	datF.close();
	NMALG_CUTPATHGEN::VSCutPathInfo pathInfo;


	NMALG_CUTPATHGEN::VSCutPathGenInput input = perstGenInput.Get();

	// 获取输入对象中封装的网格
	VSPerfectMesh perfMesh;
	GETPERFECTMESH(perfMesh, input.gumMesh, extSys, rb);


	// 对input.teethMesh中的网格逐一取数据，然后计算。
	for (size_t ti = 0; ti < input.teethMesh.len - 1; ti++)
	{
		const auto& curTeethVerts = input.teethMesh.pData[ti].pVertices;
		const auto& curTeethVertCnt = input.teethMesh.pData[ti].nVertCount;
		const auto& nxtTeethVerts = input.teethMesh.pData[ti + 1].pVertices;
		const auto& nxtTeethVertCnt = input.teethMesh.pData[ti + 1].nVertCount;

		VFVECTOR3 curTeethCen = VFVECTOR3::ZERO;
		VFVECTOR3 nxtTeethCen = VFVECTOR3::ZERO;
		for (size_t i = 0; i < curTeethVertCnt; i++)
			curTeethCen += curTeethVerts[i];
		curTeethCen /= (float)curTeethVertCnt;
		for (size_t i = 0; i < nxtTeethVertCnt; i++)
			nxtTeethCen += nxtTeethVerts[i];
		nxtTeethCen /= (float)nxtTeethVertCnt;

		VFVECTOR3 cur2nxt = nxtTeethCen - curTeethCen;
		cur2nxt.Normalize();
		float curMaxDist = VF_MIN;
		for (size_t i = 0; i < curTeethVertCnt; i++)
		{
			const float dist = cur2nxt.Dot(curTeethVerts[i] - curTeethCen);
			if (curMaxDist < dist)
				curMaxDist = dist;
		}
		VFVECTOR3 nxt2cur = curTeethCen - nxtTeethCen;
		nxt2cur.Normalize();
		float nxtMaxDist = VF_MIN;
		for (size_t i = 0; i < nxtTeethVertCnt; i++)
		{
			const float dist = nxt2cur.Dot(nxtTeethVerts[i] - nxtTeethCen);
			if (nxtMaxDist < dist)
				nxtMaxDist = dist;
		}

		if ((nxtTeethCen - curTeethCen).Magnitude() - curMaxDist - nxtMaxDist > 3.5f)
		{
			// pathInfo.blValid = ErrorCode::CUTPATH_GEN_FAIL_EXTRACTED;
			return;
		}
	}




	// 1. 确定前景点。
	NMALG_CUTPATHGEN::VCFindPointPairPath findPointPairPath;
	VSConstBuffer<unsigned> cbFrontVertices;			// 前景点的索引？？？

	VD_F_PROFILE_START(VSCutPathGenerator::findPointPairPath.Gen);
	findPointPairPath.Gen(cbFrontVertices, input);
	VD_F_PROFILE_FINISH(VSCutPathGenerator::findPointPairPath.Gen);


	// 2. 求解laplace方程。
	NMALG_CUTPATHGEN::VCCalcLevelSet calcLevelSet;
	VSConstBuffer<double> cbLevelSet;				// 存放第二步的输出。

	VD_F_PROFILE_START(VSCutPathGenerator::calcLevelSet.Calc);
	calcLevelSet.Calc(cbLevelSet, perfMesh, cbFrontVertices, input.cbBottomLine);
	VD_F_PROFILE_FINISH(VSCutPathGenerator::calcLevelSet.Calc);

	std::vector<float> vLevelSet(cbLevelSet.len);		// 存储结果的向量。
	for (size_t i = 0; i < cbLevelSet.len; i++)
	{
		vLevelSet[i] = cbLevelSet.pData[i];
	}



	// 3. 通过laplace方程的解找出腭网格中切割路径的顶点，可能并非最终结果，因为后面可能需要对点数进行精简。
	NMALG_CUTPATHGEN::VCFindCutPath  m_findCutPath;				// 功能类的成员数据
	std::vector<unsigned> vCutPathVIndex;
	VD_F_PROFILE_START(VSCutPathGenerator::m_findCutPath.Gen);
	m_findCutPath.Gen(pathInfo, vCutPathVIndex, perfMesh, VD_V2CB(vLevelSet));

	VD_F_PROFILE_FINISH(VSCutPathGenerator::m_findCutPath.Gen);
	if (pathInfo.blValid != CUTPATH_GEN_OK)			// 错误处理
		return;


	// 4. ？？？精简切割路径中的点数？？？
	std::vector<unsigned> vOptimizeVertIndex;
	std::vector<VFVECTOR3> vCutVertices;
	std::vector<VFVECTOR3> vCutNormals;

	//			原函数为功能类内部的private方法, 这里写成lambda.
	auto OptimizeCutPath = [](std::vector<VFVECTOR3>& outPath, std::vector<unsigned>& vOptimizeVertIndex,
		const VSConstBuffer<unsigned>& cbVertIndex, const VSConstBuffer<VFVECTOR3>& cbCutPath)		
	{
		if (0 == cbCutPath.len)
			return;

		const unsigned cnVertCount = 200;
		int nVertCount = cbCutPath.len;
		if (nVertCount > cnVertCount)
		{
			std::vector<float> vEdgeLen(nVertCount + 1);

			vEdgeLen[0] = 0.0f;

			for (int nVIdx = 1; nVIdx <= nVertCount; nVIdx++)
			{
				float flVal = (cbCutPath.pData[nVIdx - 1] - cbCutPath.pData[nVIdx % nVertCount]).SqrMagnitude();
				vEdgeLen[nVIdx] = vEdgeLen[nVIdx - 1] + flVal;
			}
			float flStep = vEdgeLen[nVertCount] / (float)cnVertCount;

			outPath.resize(cnVertCount);
			vOptimizeVertIndex.resize(cnVertCount);
			outPath[0] = cbCutPath.pData[0];
			unsigned nOrgVIdx = 1;
			for (int nCutVCount = 1; nCutVCount < cnVertCount; nCutVCount++)
			{
				float flVal = flStep * nCutVCount;
				for (; nOrgVIdx < nVertCount; nOrgVIdx++)
				{
					if (flVal - vEdgeLen[nOrgVIdx] < 0.0f)
					{
						outPath[nCutVCount] = cbCutPath.pData[nOrgVIdx];
						vOptimizeVertIndex[nCutVCount] = cbVertIndex.pData[nOrgVIdx];
						if (std::abs(vEdgeLen[nOrgVIdx] - flVal) > std::abs(vEdgeLen[nOrgVIdx - 1] - flVal))
						{
							outPath[nCutVCount] = cbCutPath.pData[nOrgVIdx - 1];
							vOptimizeVertIndex[nCutVCount] = cbVertIndex.pData[nOrgVIdx - 1];
						}
						break;
					}
				}
			}
		}
		else
		{
			outPath.resize(nVertCount);
			vOptimizeVertIndex.resize(nVertCount);
			std::memcpy(&outPath[0], cbCutPath.pData, sizeof(VFVECTOR3) * nVertCount);
			std::memcpy(&vOptimizeVertIndex[0], cbVertIndex.pData, sizeof(unsigned) * nVertCount);
		}
	};



	VD_F_PROFILE_START(VSCutPathGenerator::OptimizeCutPath);
	OptimizeCutPath(vCutVertices, vOptimizeVertIndex, VD_V2CB(vCutPathVIndex), pathInfo.cbCutVertices);
	VD_F_PROFILE_FINISH(VSCutPathGenerator::OptimizeCutPath);



}



// 看一下切割路径程序输入数据中封装了哪些网格
void test12() 
{
	TVPersist<NMALG_CUTPATHGEN::VSCutPathGenInput> perstGenInput;

	// 读取文件中的序列化数据对象——VSCutPathGenInput对象的序列化数据
	std::ifstream datF("E:/cutPath/cutpathparaminputLower_1.dat", std::ios::binary);
	datF >> perstGenInput;
	datF.close();
	NMALG_CUTPATHGEN::VSCutPathInfo pathInfo;
	
	



	NMALG_CUTPATHGEN::VSCutPathGenInput input = perstGenInput.Get();

	VSConstBuffer<VSSimpleMeshF>* pbuffer = nullptr;
	VSConstBuffer<VSConstBuffer<unsigned> >* pbb = nullptr;
	const VSSimpleMeshF* pmesh = nullptr;


	OBJWriteSimpleMesh("E:/cutPath/VSCutPathGenInput.gumMesh.obj", input.gumMesh);



	// 成员数据cbGumGumLine是一个双重buffer——VSConstBuffer<VSConstBuffer<unsigned>>, 每颗牙的牙龈线点集。 
	pbuffer = &input.teethMesh;
	pbb = &input.cbGumGumLine;

	stringstream ss;
	string str = "E:/cutPath/VSCutPathGenInput.teethMesh_";


	for (int i = 0; i < pbuffer->len; i++)			// 当前正在处理第i颗牙齿。
	{
		ss << str;
		ss << i; 
		ss << ".obj";
		pmesh = &(pbuffer->pData[i]);
		cout << pmesh->nVertCount << endl;
		OBJWriteSimpleMesh(ss.str().c_str(), *pmesh);
		ss.clear();
		ss.str("");

		str = "E:/cutPath/牙龈线_";
		ss << str;
		ss << i;
		ss << ".obj";
		vector<VFVECTOR3> vers;
		vers.resize(pbb->pData[i].len);
		for (int j = 0; j < pbb->pData[i].len; j++)		// vector容器中放入每一个牙龈线顶点。 
		{
			unsigned index = (pbb->pData[i]).pData[j];
			cout << "index == " << index << endl;
			vers[j] = pmesh->pVertices[index];
		}

		OBJWriteVertices( VD_V2CB(vers), ss.str().c_str());
		vers.clear();

		ss.clear();
		ss.str("");
	}


	pbuffer = &input.waxMesh;
	str = "E:/cutPath/VSCutPathGenInput.waxMesh_";
	for (int i = 0; i < pbuffer->len; i++)
	{
		ss << str;
		ss << i;
		ss << ".obj";
		OBJWriteSimpleMesh(ss.str().c_str(), pbuffer->pData[i]);
		ss.clear();
		ss.str("");
	}


	pbuffer = &input.attMesh;
	str = "E:/cutPath/VSCutPathGenInput.attMesh_";
	for (int i = 0; i < pbuffer->len; i++)
	{
		ss << str;
		ss << i;
		ss << ".obj";
		OBJWriteSimpleMesh(ss.str().c_str(), pbuffer->pData[i]);
		ss.clear();
		ss.str("");
	}






}



// 测试测地距离功能类：
void test13() 
{

	VBGeodicMesh geo;				// 测量测地距离的功能类对象。

	VNALGMESH::VSMeshGeodic result;			// 存放测量结果的对象，只有一个字段：lstDist，是一个封装了float的buffer		
	VSPerfectMesh  perfMesh;				// 输入网格——一个接近原型的面网格，顶点全在边缘。
	VSConstBuffer<unsigned>	pointIdx;		// 起始点的索引。

	VSSimpleMeshF mesh;
	OBJReadSimpMesh(mesh, "E:/roundSurface.obj");
	GETPERFECTMESH(perfMesh, mesh , extSys, rb);			// 输入simplemesh得到perfectMesh的接口。

															
	// 射测量起点为网格中第一个点。
	vector<unsigned> verIdx;
	verIdx.push_back(0);
	pointIdx = VD_V2CB(verIdx);


	// VBGeodicMesh功能类对象执行测地距离的测量——Build()方法
	/*
		void Build( VNALGMESH::VSMeshGeodic & gm , 
					const VSGraphMesh & msh , 
					const VSConstBuffer< unsigned > & cd 
					) ;
	*/
	geo.Build(result, perfMesh, pointIdx);

	VSConstBuffer<float>& buffer = result.lstDist;

	for (int i = 0; i < buffer.len; i++)
	{
		cout << buffer.pData[i] << ",  ";
		if (0 == (i + 1) % 5)
		{
			cout << endl;
		}
	}
	cout << endl;
	cout << "测量结果中的float的个数：" << buffer.len << endl;

	const float pi = 3.14159;
	float radius = 10;
	VFVECTOR3 temp1(radius, 0, 0);
	VFVECTOR3 temp2(radius*cos(2 * pi / 30.0), radius*sin(2 * pi / 30.0), 0);
	cout << "理论计算相邻两个顶点的距离为：" << (temp2 - temp1).Magnitude() << endl;


	// 如果测量测地距离设定的起始点不止一个：
	verIdx.push_back(1);
	verIdx.push_back(2);
	pointIdx = VD_V2CB(verIdx);
	result = VNALGMESH::VSMeshGeodic();		// 输出容器重置一下。
	geo.Build(result, perfMesh, pointIdx);


	buffer = result.lstDist;
	for (int i = 0; i < buffer.len; i++)
	{
		cout << buffer.pData[i] << ",  ";
		if ( 0 == (i+1)% 5) 
		{
			cout << endl;
		}
	}
	cout << endl;


	cout << "finished." << endl;
}



}


