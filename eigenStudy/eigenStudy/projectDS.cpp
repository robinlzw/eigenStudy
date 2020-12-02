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


}


