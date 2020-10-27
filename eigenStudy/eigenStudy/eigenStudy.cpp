#include <iostream>
#include <Eigen\Dense>		// ��Ҫ��eigen��Ŀ¼��ӵ�����Ŀ¼�С�
#include <vector>
#include <string>


#include "myAsistant.h"		// �Լ�д��һЩ��������
#include "vfvector3.h"		// �ṩ����ά�ռ�������VFVECTOR3
#include "vfmesh.h"			// �ṩ����ά����ļ������ݽṹ��
#include "vfquaternion.h"	// �ṩ����Ԫ����
#include "vflocation.h"		// �ṩ��ʵʩ�ֲ�/ȫ������ϵת������VFLocation
#include "obj.h"			// �ṩ��һ�ѽӿ��ࡣ
#include "mem.h"			// �ṩ��VSConstBuffer��
#include "ticmn.h"			// �ṩ�˴���VSConstBuffer���������ݵĺ�����
#include "pttrn.h"			// һЩ���õ�ģʽ
#include "strm.h"			// �ṩ��TVPersist, TVFilePersist����ģ��


/*
	����Ŀ¼��
			������Ŀ\Base\vmath
			������Ŀ\Base\vstd

*/


using namespace Eigen;     // �ĳ�������� using Eigen::MatrixXd; 
using namespace std;

#define PI 3.14159



/*
	VectorXd��
			normalized()	�������һ��������
			normalize()		����������й�һ��������


*/


// һ���ѹ�����Ŀ���õ��Ļ���ͷ�ļ��ӽ���.




// ��һ��eigen����
void test0() 
{
	MatrixXd m = MatrixXd::Random(3, 3);              // ������static��������Random()���������������
	VectorXd v(3);										// ����vΪ3*1��double������
	Vector3d v1;
	
	// �������static��������Constant()�������س�������
	m = (m + MatrixXd::Constant(3, 3, 1.2)) * 50;      //MatrixXd::Constant(3,3,1.2)��ʾ����3*3��double�;��󣬸þ�������Ԫ�ؾ�Ϊ1.2
	cout << "m == \n" << endl << m << endl;


	// ���������������������
	v << 1, 2, 3;							// ������ֵ
	cout << "m * v == \n" << endl << m * v << endl;


	v1 << 1, 2, 3;        
	cout << "v1 == \n" << v1 << endl;
	cout << "v1[0] == " << v1[0] << endl;			// Ԫ���±����

}



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
	int arri[] = {1,2,3,4,5};
	VSConstBuffer<int> buffer(5,arri);		// ���ι��졣



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
	VFVECTOR3 v1(1,2,3);

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



int main()
{
	test3();


	getchar();
	return 0;
}