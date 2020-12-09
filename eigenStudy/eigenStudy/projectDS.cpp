#include "projectDS.h"
// ������Ŀ�е��Զ������ݽṹ



using namespace Eigen;     // �ĳ�������� using Eigen::MatrixXd; 
using namespace std;
const double pi = 3.14159;

#define PI 3.14159



namespace PDS 
{


// ����VSSimpleMeshF�ࣺ
/*
	��һ����Ϊ��ָ����࣬��VSConstBuffer���ơ�
	ָ�������ʵ����NM_PMMESH::VSMesh����
	20201203��
			Ŀǰ�о�simpleMesh��Ϊһ��pointer-like�ಢû����Ƶ��㹻���ƣ�û��ʵ�����ü�����Ҳû�ж������delete����������������
			���õ��ڳ����д�����ά����ʵ��ķ�����ʹ��OBJReadSimpMesh()�ӿڣ����ɵ��ǶѶ�������simpleMesh�����Զ�delete��ʹ�������з��ա�Ŀǰ�����������������ļ��ж�ȡ�������ݡ�
*/
void test1()
{
	VSSimpleMeshF mesh1, mesh2, tooth;

	generateMeshWrong(mesh1);
	generateMeshRight(mesh2);

	v3disp(mesh1.pVertices[0]);			// ջ���������Wrong()��������֮���������ڽ�����Ȼ���ͷţ�mesh1ָ����Ұָ�롣
	v3disp(mesh2.pVertices[0]);			// �����������Right()��������֮����Ȼ���ڡ�


	delete[] mesh1.pVertices;
	delete[] mesh2.pVertices;

	OBJReadSimpMesh(tooth, "tooth.obj");			// ��ȡOBJ�ļ�������simpleMesh����ָ�������������ʵ���ڶ��ϡ�


	cout << "finished." << endl;
}
	


// ������������ջ����������������ָ��ĺ���������������ʾ��
bool generateMeshWrong(VSSimpleMeshF& mesh) 
{

	NM_PMMESH::VSMesh vmesh;
	vmesh.vVertice.push_back(VFVECTOR3(1,2,3));

	PMGetSimpMesh(mesh, vmesh);

	return true;
}



// �����������ɶ��ϵ���������������ָ��ĺ���������ȷʾ��
bool generateMeshRight(VSSimpleMeshF& mesh) 
{
	NM_PMMESH::VSMesh* pmesh = new NM_PMMESH::VSMesh;

	pmesh->vVertice.push_back(VFVECTOR3(1, 2, 3));

	PMGetSimpMesh(mesh, *pmesh);
	return true;

}






//	����VSConstBuffer��ģ��
/*
	��һ����Ϊ��ָ����࣬��װ�������ݵ�ָ�롣
		��Ա����
			unsigned    len;		Ԫ����
			const T *   pData;		���ݵ�ָ�룬ע���ǵײ�const��������ͨ��ָ���޸����ݡ�

	����ĳ�����ݶ����Ӧ��VSConstBuffer����Ĺ����У����������ݵĿ�����
	VSConstBuffer��һ����Ϊ��ָ����ࡣ
	��VSConstBuffer����Ŀ�����ǳ������
*/
void test2()
{
	int arri[] = { 1,2,3,4,5 };
	VSConstBuffer<int> buffer(5, arri);		// ���ι��졣


	// ʹ���±����������VCBuffer�е�Ԫ�أ�
	for (int i = 0; i < buffer.len; i++) 
	{
		cout << buffer.pData[i] << "   ";
	}
	cout << endl;



	// VD_V2CB()����STL����ת��Ϊbuffer����vector to const buffer
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


	
	// ����ĳ�����ݶ����Ӧ��VBuffer����ʱ�����������ݵĿ�����
	//		VBuffer��һ����Ϊ��ָ����ࡣ
	int numi = 999;
	VSConstBuffer<int> ibuffer(1, &numi);
	cout << "*pData == " << *ibuffer.pData << endl;

	numi = 888;
	cout << "*pData == " << *ibuffer.pData << endl;





	


}



// ����TVPersist��ģ��
/*
��Ա����(private)
	std::vector< char > m_buff;			
		���л�������ݣ������л������ݱ����ֽڵ�д�뵽��vector�����С�

 
����ӿ�
	Get()
		���ط�װ���ݵ�ָ�룬ΪT*����
	Build()
		����T���͵����ݣ����л�Ϊ�Լ��ĳ�Ա����m_buff
	GetBuffer()
		����Ա����m_buff������װ��bufferȻ�󷵻ء�
*/
void test3()
{
	TVPersist<VFVECTOR3> vp;
	VFVECTOR3 v1(1, 2, 3);

	vp.Build(v1);		// �������������л������뵽�����л������m_buff��Ա����֮�С�


	VFVECTOR3 v2 = vp.Get();								// �������л�֮ǰ�����ݡ�
	VSConstBuffer<char> buffer_per = vp.GetBuffer();		// �������л������ݣ���m_buff��Ա������

	v3disp(v2);


	// ��һ�����л����������ʲô���ġ�
	const char* pc = buffer_per.pData;
	cout << "���л�֮�����ɵ�TVPersist������ֽ�����" << sizeof(vp) / sizeof(char) << endl;
	cout << "���л�֮ǰ�������ֽ�����" << sizeof(v1) << endl;
	for (int i = 0; i < buffer_per.len; i++)
	{
		cout << *pc << endl;
		pc++;
	}


	// ���л������з����˿�����
	float numf = 9.999999;
	TVPersist<float> fper;
	
	cout << "���л�ǰ��sizeof(iper) == " << sizeof(fper) << endl;


	fper.Build(numf);


	numf = 8.88888;
	float result = fper.Get();
	cout << "���л���sizeof(iper) == " << sizeof(fper) << endl;
	cout << "result == " << result << endl;	
	cout << "numf == " << numf << endl;

}



// ����VFQuaternion��Ԫ����
void test4() 
{
	VFQuaternion q1(1, 2, 3, 4);

	// 1. ������ת����������ת�Ƕ�, ������Ԫ������
	VFQuaternion q2 = VFQuaternion::FromAxisAngle(VFVECTOR3::AXIS_Z, pi/2);
	v3disp(q2*VFVECTOR3(1, 0, 0));


	// 2. ����Ŀ�������᷽��������������Ԫ������
	VFVECTOR3	x_new, y_new, z_new;
	x_new = VFVECTOR3(1,2,3);
	y_new = VFVECTOR3(0,-3,2);
	z_new = x_new.Cross(y_new);
	x_new.Normalize();
	y_new.Normalize();
	z_new.Normalize();

	VFQuaternion q = VFQuaternion::FromAxes(x_new, y_new, z_new);
	v3disp(q*VFVECTOR3::AXIS_X);


	// 3. ����ԭʼ������Ŀ��������������Ԫ������
	q = VFQuaternion::FromRotateAxis(VFVECTOR3::AXIS_X, VFVECTOR3::AXIS_Y);
	v3disp(q*VFVECTOR3::AXIS_X);
	v3disp(q*VFVECTOR3::AXIS_Y);
	v3disp(q*VFVECTOR3::AXIS_Z);


}



// ����VFLocation��
void test5()
{
	// ʹ��VFLocation����һ������������תȻ��ƽ�ơ�

	//VFLocation loc();


}



// ����TVBuilderArgTuple��ģ��
void test6()
{
	TVBuilderArgTuple<string> tuple;		// һ������string�������ݵ�argtuple����
	
	// 1. ���ɴ��������ݣ����л���д�뵽�ļ���
	string str = "hello world hahahahaha";
	TVPersist<string> pstr;
	pstr.Build(str);
	fstream file1("persist_data.dat", ios::out|ios::binary);
	file1 << pstr;
	file1.close();


	// 2. argtuple������ȡ�ļ��е����л�����
	ifstream file2("persist_data.dat", ios::in | ios::binary);
	tuple.Resume(file2);
	file2.close();


	// 3. ���뺯���Ӵ���argtuple�е�����		����������ӡ�ַ���
	tuple.Travel([](const string& str0)		// �����ӵ��βα�����const�ģ�����Ӧ�þ���argtuple�����л����ݵ�ԭ����
	{
		// argtupleʹ�ú����Ӷ��ڲ�����ִ�еĲ�����ֻ��������
		cout << str0.length() << endl;
		cout << str0 << endl;

	});




	cout << "finished " << endl;


}



// ����VFMatrix3��
void test7() 
{
	


}



// ����TVR2B��
void test8() 
{
	//TVR2B<VFVECTOR3>	v1();
	//
	//VFVECTOR3 vec1(1,2,3);
	//VFQuaternion q1(1, 2, 3, 4);

	//v1.Build(vec1, q1);


}





// ������ײ�����VBCollisionSence
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




// �������߼���ࣺ
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


	// �������������ĵ㡣
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


	// ȷ���������򡪡�����Ӧ���ǹ�Դָ�򱻲�������
	VFVECTOR3 dir = center1 - center;
	dir.Normalize();


	// �������߶����buffer��
	vector<VFRay> vec_ray;
	VSConstBuffer<VFRay> buffer_ray;
	for (int i = 0; i < tooth.nVertCount; i++)
	{
		VFRay rayTemp(tooth.pVertices[i], dir);
		vec_ray.push_back(rayTemp);
	}
	buffer_ray = VD_V2CB(vec_ray);




	// �������߲����̵��������
	rayIn.simpMesh = tooth1;
	rayIn.rays = buffer_ray;

	// ִ�����߲�����
	pRay->Build(rayOut, rayIn);


	// �������߲���������
	/*
	VSConstBuffer<VSConstBuffer<float>>, cbRayLen				���������������񽻵㣺ray.origin + ray.direction * len
	VSConstBuffer<VSConstBuffer<unsigned>>, cbSurfIdx			�����������������������ཻ�������ϵĶ�����������cbRayLenһһ��Ӧ
	VSConstBuffer<VSConstBuffer<float>>, cbOpRayLen				���߷����������񽻵㣺ray.origin + ray.direction * len
	VSConstBuffer<VSConstBuffer<unsigned>>, cbOpSurfIdx			���߷������������ཻ������Ƭ����cbOpRayLenһһ��Ӧ
	*/
	const VSConstBuffer<float>* pcf = NULL;
	const VSConstBuffer<unsigned>* pcu = NULL;
	cout << "tooth.nVertCount == " << tooth.nVertCount << endl;
	cout << "rayOut.cbRayLen.len == " << rayOut.cbRayLen.len << endl;

	pcf = &rayOut.cbRayLen.pData[0];					// ��һ�������õ��ľ������ݡ���float��Vbuffer

														//		�������ߴ���Ŀ���������������㡪��λ��ǰ���桢����档
	cout << "�������ߺ�Ŀ�����񽻵���� == " << pcf->len << endl;
	cout << "��þ���ֱ�Ϊ��" << pcf->pData[0] << ", " << pcf->pData[1] << endl;


	//		��������û�д���Ŀ������
	pcf = &rayOut.cbOpRayLen.pData[0];
	cout << "�������ߺ�Ŀ�����񽻵���� == " << pcf->len << endl;


	//		�������ߺ�Ŀ�����񽻵����ڵ�����Ƭ��
	pcu = &rayOut.cbSurfIdx.pData[0];				// ��һ�������õ�������Ƭ���ݡ�
	cout << "tooth1.nTriangleCount == " << tooth1.nTriangleCount << endl;
	cout << "�������ߺ�Ŀ�����񽻵���� == " << pcu->len << endl;
	cout << "������Ŀ�������ϵ����ڵ�����Ƭ�����ֱ�Ϊ��" << pcu->pData[0] << ", " << pcu->pData[1] << endl;
	VNVector3<unsigned> vn;
	vn = tooth1.pTriangles[pcu->pData[0]];
	vndisp<unsigned>(vn);


}



// �����и�·��
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
	
	// ��ȡ�ļ��е����л����ݶ��󡪡�VSCutPathGenInput��������л�����
	std::ifstream datF("E:/cutPath/cutpathparaminputLower_1.dat", std::ios::binary);
	datF >> perstGenInput;
	datF.close();
	NMALG_CUTPATHGEN::VSCutPathInfo pathInfo;


	NMALG_CUTPATHGEN::VSCutPathGenInput input = perstGenInput.Get();

	// ��ȡ��������з�װ������
	VSPerfectMesh perfMesh;
	GETPERFECTMESH(perfMesh, input.gumMesh, extSys, rb);


	// ��input.teethMesh�е�������һȡ���ݣ�Ȼ����㡣
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




	// 1. ȷ��ǰ���㡣
	NMALG_CUTPATHGEN::VCFindPointPairPath findPointPairPath;
	VSConstBuffer<unsigned> cbFrontVertices;			// ǰ���������������

	VD_F_PROFILE_START(VSCutPathGenerator::findPointPairPath.Gen);
	findPointPairPath.Gen(cbFrontVertices, input);
	VD_F_PROFILE_FINISH(VSCutPathGenerator::findPointPairPath.Gen);


	// 2. ���laplace���̡�
	NMALG_CUTPATHGEN::VCCalcLevelSet calcLevelSet;
	VSConstBuffer<double> cbLevelSet;				// ��ŵڶ����������

	VD_F_PROFILE_START(VSCutPathGenerator::calcLevelSet.Calc);
	calcLevelSet.Calc(cbLevelSet, perfMesh, cbFrontVertices, input.cbBottomLine);
	VD_F_PROFILE_FINISH(VSCutPathGenerator::calcLevelSet.Calc);

	std::vector<float> vLevelSet(cbLevelSet.len);		// �洢�����������
	for (size_t i = 0; i < cbLevelSet.len; i++)
	{
		vLevelSet[i] = cbLevelSet.pData[i];
	}



	// 3. ͨ��laplace���̵Ľ��ҳ����������и�·���Ķ��㣬���ܲ������ս������Ϊ���������Ҫ�Ե������о���
	NMALG_CUTPATHGEN::VCFindCutPath  m_findCutPath;				// ������ĳ�Ա����
	std::vector<unsigned> vCutPathVIndex;
	VD_F_PROFILE_START(VSCutPathGenerator::m_findCutPath.Gen);
	m_findCutPath.Gen(pathInfo, vCutPathVIndex, perfMesh, VD_V2CB(vLevelSet));

	VD_F_PROFILE_FINISH(VSCutPathGenerator::m_findCutPath.Gen);
	if (pathInfo.blValid != CUTPATH_GEN_OK)			// ������
		return;


	// 4. �����������и�·���еĵ���������
	std::vector<unsigned> vOptimizeVertIndex;
	std::vector<VFVECTOR3> vCutVertices;
	std::vector<VFVECTOR3> vCutNormals;

	//			ԭ����Ϊ�������ڲ���private����, ����д��lambda.
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



// ��һ���и�·���������������з�װ����Щ����
void test12() 
{
	TVPersist<NMALG_CUTPATHGEN::VSCutPathGenInput> perstGenInput;

	// ��ȡ�ļ��е����л����ݶ��󡪡�VSCutPathGenInput��������л�����
	std::ifstream datF("E:/cutPath/cutpathparaminputLower_1.dat", std::ios::binary);
	datF >> perstGenInput;
	datF.close();
	NMALG_CUTPATHGEN::VSCutPathInfo pathInfo;
	
	



	NMALG_CUTPATHGEN::VSCutPathGenInput input = perstGenInput.Get();

	VSConstBuffer<VSSimpleMeshF>* pbuffer = nullptr;
	VSConstBuffer<VSConstBuffer<unsigned> >* pbb = nullptr;
	const VSSimpleMeshF* pmesh = nullptr;


	OBJWriteSimpleMesh("E:/cutPath/VSCutPathGenInput.gumMesh.obj", input.gumMesh);



	// ��Ա����cbGumGumLine��һ��˫��buffer����VSConstBuffer<VSConstBuffer<unsigned>>, ÿ�����������ߵ㼯�� 
	pbuffer = &input.teethMesh;
	pbb = &input.cbGumGumLine;

	stringstream ss;
	string str = "E:/cutPath/VSCutPathGenInput.teethMesh_";


	for (int i = 0; i < pbuffer->len; i++)			// ��ǰ���ڴ����i�����ݡ�
	{
		ss << str;
		ss << i; 
		ss << ".obj";
		pmesh = &(pbuffer->pData[i]);
		cout << pmesh->nVertCount << endl;
		OBJWriteSimpleMesh(ss.str().c_str(), *pmesh);
		ss.clear();
		ss.str("");

		str = "E:/cutPath/������_";
		ss << str;
		ss << i;
		ss << ".obj";
		vector<VFVECTOR3> vers;
		vers.resize(pbb->pData[i].len);
		for (int j = 0; j < pbb->pData[i].len; j++)		// vector�����з���ÿһ�������߶��㡣 
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



// ���Բ�ؾ��빦���ࣺ
void test13() 
{

	VBGeodicMesh geo;				// ������ؾ���Ĺ��������

	VNALGMESH::VSMeshGeodic result;			// ��Ų�������Ķ���ֻ��һ���ֶΣ�lstDist����һ����װ��float��buffer		
	VSPerfectMesh  perfMesh;				// �������񡪡�һ���ӽ�ԭ�͵������񣬶���ȫ�ڱ�Ե��
	VSConstBuffer<unsigned>	pointIdx;		// ��ʼ���������

	VSSimpleMeshF mesh;
	OBJReadSimpMesh(mesh, "E:/roundSurface.obj");
	GETPERFECTMESH(perfMesh, mesh , extSys, rb);			// ����simplemesh�õ�perfectMesh�Ľӿڡ�

															
	// ��������Ϊ�����е�һ���㡣
	vector<unsigned> verIdx;
	verIdx.push_back(0);
	pointIdx = VD_V2CB(verIdx);


	// VBGeodicMesh���������ִ�в�ؾ���Ĳ�������Build()����
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
	cout << "��������е�float�ĸ�����" << buffer.len << endl;

	const float pi = 3.14159;
	float radius = 10;
	VFVECTOR3 temp1(radius, 0, 0);
	VFVECTOR3 temp2(radius*cos(2 * pi / 30.0), radius*sin(2 * pi / 30.0), 0);
	cout << "���ۼ���������������ľ���Ϊ��" << (temp2 - temp1).Magnitude() << endl;


	// ���������ؾ����趨����ʼ�㲻ֹһ����
	verIdx.push_back(1);
	verIdx.push_back(2);
	pointIdx = VD_V2CB(verIdx);
	result = VNALGMESH::VSMeshGeodic();		// �����������һ�¡�
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


