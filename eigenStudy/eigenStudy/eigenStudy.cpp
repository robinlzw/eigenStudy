#include <iostream>
#include <Eigen\Dense>		// ��Ҫ��eigen��Ŀ¼��ӵ�����Ŀ¼�С�

#include "vfvector3.h"		// �ṩ����ά�ռ�������VFVECTOR3
#include "vfmesh.h"			// �ṩ����ά����ļ������ݽṹ��
#include "vfquaternion.h"	// �ṩ����Ԫ����
#include "vflocation.h"		// �ṩ��ʵʩ�ֲ�/ȫ������ϵת������VFLocation


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



// ������Ŀ���������޸�simplemesh��ʾ����ά����������
void test1() 
{
	VSSimpleMeshF mesh1;




}





int main()
{
	//test0();

	
	VFVECTOR3 v1(1,1,1);
	VSSimpleMeshF TOOTH;
	
	getchar();
	return 0;
}