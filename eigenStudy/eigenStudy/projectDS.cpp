#include "projectDS.h"
// ������Ŀ�е��Զ������ݽṹ



using namespace Eigen;     // �ĳ�������� using Eigen::MatrixXd; 
using namespace std;
const double pi = 3.14159;

#define PI 3.14159



namespace PDS 
{


// ������Ŀ
//		�����޸�simplemesh��ʾ����ά����������
void test1()
{





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



// ������ײ�����
void test9() 
{

}


// �������߼���ࣺ
void test10() 
{


}


}


