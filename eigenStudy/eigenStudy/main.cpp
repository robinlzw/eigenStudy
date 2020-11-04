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
#include "projectDS.h"

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




// test0����eigen��Ļ������ݽṹ
void test0() 
{
	// �Ѿ�����������ȷ���˳ߴ磬��δ��ʼ��,���ݴ��ڶ���
	//			�����ģ�塪��Matrix<typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>
	//			�Ѿ��󡪡�typedef Matrix<double, Dynamic, Dynamic> MatrixXd;
	//			����������typedef Matrix<int, Dynamic, 1> VectorXi;
	MatrixXd m1(2, 2);
	MatrixXf mf1(1, 2);
	VectorXd v1(3);			// ע����������
	cout << v1 << endl;


	// ��Array
	ArrayXXd a1(2, 2), a2(2, 2);
	a1 << 1, 2, 3, 4;
	a2 << 1, 2, 3, 4;
	cout << "a1 = \n" << a1 << endl;
	cout << "a1*a2 = \n" << a1*a2 << endl;



	// ջArray



	// �����������Ľӿ�
	MatrixXd m2 = MatrixXd::Random(3, 3);              // ������static��������Random()���������������
	MatrixXd m3 = MatrixXd::Constant(3, 3, 1.2);		// ��������
	MatrixXd m4 = MatrixXd::Ones(1,2);					// ȫ1����



	// ���ݴ���ջ�ϵľ�������
	Matrix3d mm1 = Matrix3d::Random();
	Vector3d vv1(1, 2, 3);
	cout << "m2 = \n" << m2 << endl << endl;
	cout << "mm1 = \n" << mm1 << endl << endl;
	//		�Ѿ����ջ��������໥��ֵ��
	mm1 = m2;
	cout << "mm1 = \n" << mm1 << endl << endl;



	// ��ȡ�������ʵĽӿ�
	cout << m1.rows() << endl;
	cout << m1.cols() << endl;
	cout << m1.size() << endl;


	// ������������Ժ;�������໥����
	Vector3d vv2(1,2,3);			 
	//		
	MatrixXd mm(v1);			
	Vector3d vv3(mm);
	cout << "vv2 = \n" << vv2 << endl << endl;
	cout << "vv3 = \n" << vv3 << endl << endl;
	cout << "mm = \n" << mm << endl << endl;
	cout << "mm.transpose() = \n" << mm.transpose() << endl << endl;


	//		������������Ժ;�������໥��ֵ
	vv2 = m2.block<3,1>(0,0);
	cout << "vv2 = \n" << vv2 << endl << endl;
	mm = vv2;
	cout << "mm = \n" << mm << endl << endl;
}




// test1�����������ʡ�Ԫ�ط��ʡ�
void test1() 
{
	MatrixXd m1(3, 4);
	VectorXd v1(5);

	// ������������ֵ
	m1 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12;
	v1 << 1, 2;
	cout << "v1 = \n" << v1 << endl << endl;

	v1 << 3, 4, 5;			// �����������ĸ�ֵ�����Ǵӵ�һ��Ԫ�ؿ�ʼ��ֵ
	cout << "v1 = \n" << v1 << endl << endl;


	// �±������[]ֻ�ܻ�ȡ����Ԫ�أ���Ϊ[]ֻ֧��һ��������
	cout << "v1[0] == " << v1[0] << endl << endl;;


	// �������������Ԫ�أ�ע��������0��ʼ
	cout << "m1(0, 1) ==" << m1(0, 1) << endl;
	cout << "v1(3) == " << v1(3) << endl;


	// ���������ʵ����ڽӿ�
	cout << "Ԫ������m1.size() == " << m1.size() << endl;
	cout << "������m1.rows() == " << m1.rows() << endl;
	cout << "������m1.cols() == " << m1.cols() << endl;
	cout << "��ͣ�sum():       " << m1.sum() << endl;
	cout << "��������prod():      " << m1.prod() << endl;
	cout << "��ֵ��mean():      " << m1.mean() << endl;
	cout << "��������minCoeff():  " << m1.minCoeff() << endl;
	cout << "��������maxCoeff():  " << m1.maxCoeff() << endl;
	cout << "����ļ���trace():     " << m1.trace() << endl << endl;



	// �����ľ���任
	cout << "�����ת�ã�transpose() \n" << m1.transpose() << endl << endl;
	cout << m1 << endl;


}




// test2������������任������
void test2() 
{
	MatrixXd m1(3, 4), m3(4, 4);
	m1 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12;
	m3 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;

	cout << "m1 = \n" << endl;
	cout << m1 << endl << endl;

	// MatrixXd::block<>()����������ȡ�Ӿ��󡣼��������Ӿ���ά�ȣ�Բ���ű�ʾ�Ӿ�����㡣
	cout << m1.block<2, 2>(1, 1) << endl << endl;
	cout << m1.block<1, 3>(1, 0) << endl << endl;

	// �Ծ����һ����ʹ��������������ֵ
	m1.block<1, 4>(1, 0) << 88, 99, 111, 222;
	cout << "m1 = \n" << m1 << endl << endl;
	m1.block<1, 4>(1, 0) << 5, 6, 7, 8;



	// ���ľ���ߴ硪��ֻ�����ڶѾ��󡢶�����
	m3.resize(3,3);
	cout << "m3.size() == "<< m3.size() << endl;
	cout << "m3.rows() == " << m3.rows() << endl;
	cout << "m3.cols() == " << m3.cols() << endl;
	cout << "m3 = \n" << m1 << endl << endl;



	// �����С����в���
	cout << m1.colwise().sum() << endl << endl;				// ������ͣ�ѹ��һ��������
	cout << m1.rowwise().sum() << endl << endl;				// ������ͣ�ѹ��һ����������


	// �������š���ͨ������˷�ʵ��
	MatrixXd  a(1, 3), b(3, 1);
	a << 1, 2, 3;
	b << 4, 5, 6;
	//		����������N�� == ���ones(N,1)
	auto aa = MatrixXd::Ones(3, 1)*a;
	//		����������ΪN�� == �ҳ�ones(1,N)
	auto bb = b*MatrixXd::Ones(1, 4);

	cout << aa << endl << endl;;
	cout << bb << endl << endl;;




	// ����Ԫ����ˡ�����Ҫת����Array�н��У�
	MatrixXd m2(3, 4);
	m2 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12;
	ArrayXXd a1(m1), a2(m2);
	ArrayXXd resultA = a1*a2;
	MatrixXd resultM(resultA);
	cout << "resultM == \n" << resultM << endl;



	// ������ˡ����
	Vector3d v1(1, 2, 3);
	Vector3d v2(3, 2, 1);
	cout << "v1.dot(v2) == \n" << v1.dot(v2) << endl;
	cout << "v1.cross(v2) == " << v1.cross(v2) << endl << endl;;
	




	// ������һ��
	v1.normalize();					// normalize()�Ὣ���������һ����normalized()ֻ�Ƿ��ع�һ�����������ı�����
	cout << "v1 = \n" << v1 << endl << endl;
	cout << "v2.normalized() = \n" << v2.normalized() << endl << endl;
	cout << "v2 = \n" << v2 << endl << endl;
}





// test3�������Դ�����ѧ����Ľӿ�
void test3() 
{
	Matrix3d A;
	A << 1, 2, 3, 4, 5, 6, 7, 8, 9;


	// ����������ֵ������������
	EigenSolver<Matrix3d> es(A);
	Matrix3d D = es.pseudoEigenvalueMatrix();			// �Խ���Ԫ��������ֵ
	Matrix3d V = es.pseudoEigenvectors();				// ÿһ����������������������
	cout << "����ֵ����D��" << endl << D << endl;
	cout << "������������V: " << endl << V << endl;
	cout << "Finally, V * D * V^(-1) = " << endl << V * D * V.inverse() << endl;

	cout << A*V.block<3, 1>(0, 0) << endl << endl;
	cout << D(0,0)*V.block<3, 1>(0, 0) << endl << endl;
}



int main()
{
	PDS::test4();
	getchar();
	return 0;
}