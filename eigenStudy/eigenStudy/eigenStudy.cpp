#include <iostream>
#include <Eigen\Dense>		// ��Ҫ��eigen��Ŀ¼��ӵ�����Ŀ¼�С�



using namespace Eigen;     // �ĳ�������� using Eigen::MatrixXd; 
using namespace std;

#define PI 3.14159


// �Զ���һ����Ԫ�������һ��
namespace myQ 
{
	class Quaternion
	{
	protected:
		double w;
		Vector3d v;

	public:
		Quaternion() {}

		Quaternion(double w0, double x0, double y0, double z0) :w(w0), v(x0, y0, z0) {}

		Quaternion(double w0, Vector3d v0) :w(w0), v(v0) {}



		// ���������
		//		��Ԫ��������ά����������ʾ������v0������Ԫ��ʩ����ת������
		//		��Ԫ������ת�ǵĹ�ϵ����q = cos(theta/2) + sin(theta/2)*A; ����A����ת��ĵ�λ������
		Vector3d operator * (const Vector3d & v0) const
		{
			// nVidia SDK implementation
			Vector3d a, b;
			a = this->v.cross(v0);
			b = this->v.cross(a);
			a *= (2.0f * this->w);
			b *= 2.0f;

			return (v0 + a + b);
		}


		// ��������
		void disp()
		{
			cout << "(" << this->w << ", " << this->v[0] << ", " << this->v[1] << ", " << this->v[2] << ")" << endl;
		}

	};
}


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





// ������ά��������ת
void test1() 
{
	// ��������ϵxoyƽ�������(1,1,0)��x����������ת90�ȣ�Ӧ�ñ��xozƽ�������(1,0,1) 

	// 1. �任���󷽷�
	VectorXd v1(3), v2(3);
	v1 << 1, 1, 0;

	MatrixXd Rx(3,3);		// ��X����������ת90�ȵı任����
	Rx << 1, 0, 0, 0, 0, -1, 0, 1, 0;
	cout << "v1 == \n" << v1 << endl;
	cout << "Rx == \n" << Rx << endl;

	v2 = Rx*v1;
	cout << "v2 == \n" << v2 << endl << endl << endl;;


	// 2. �Զ�����Ԫ������
	double theta = PI/2;
	Vector3d x_hat(1,0,0);
	//			ע��cos()sin()����ֻ�ϻ��ȡ�
	myQ::Quaternion q(cos(theta/2),sin(theta/2)*x_hat);		// ��ʾx�����Ԫ��
	v2 = q*v1;
	cout << "v2 == \n" << v2 << endl;



}


int main()
{
	//test0();
	test1();
	
	
	
	getchar();
	return 0;
}