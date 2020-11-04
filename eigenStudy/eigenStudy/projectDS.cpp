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




//	����buffer��ģ��
/*
��һ����Ϊ��ָ����࣬��װ�������ݵ�ָ�롣
��Ա����
unsigned    len;		Ԫ����
const T *   pData;		���ݵ�ָ�룬ע���ǵײ�const��������ͨ��ָ���޸����ݡ�
*/
void test2()
{
	int arri[] = { 1,2,3,4,5 };
	VSConstBuffer<int> buffer(5, arri);		// ���ι��졣



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

}



// ����TVPersist��ģ��
/*
��Ա����(private)
std::vector< char > m_buff;			���������л������ݣ�
struct InputPort
struct CoorectPort

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


	VFVECTOR3 v2 = vp.Get();							// �������л�֮ǰ�����ݡ�
	VSConstBuffer<char> buffer_per = vp.GetBuffer();		// �������л������ݣ���m_buff��Ա������

	v3disp(v2);


	// ��һ�����л����������ʲô���ġ�
	const char* pc = buffer_per.pData;
	cout << "���л�֮��������ֽ�����" << buffer_per.len << endl;
	cout << "���л�֮ǰ�������ֽ�����" << sizeof(v1) << endl;
	for (int i = 0; i < buffer_per.len; i++)
	{
		cout << *pc << endl;
		pc++;
	}

}



// ����VFQuaternion��Ԫ����
void test4() 
{
	VFQuaternion q1(1, 2, 3, 4);

	// ������ת����������ת�Ƕ�������Ԫ������
	VFQuaternion q2 = VFQuaternion::FromAxisAngle(VFVECTOR3::AXIS_Z, pi/2);
	v3disp(q2*VFVECTOR3(1, 0, 0));


}

}