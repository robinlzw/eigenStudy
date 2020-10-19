#include <iostream>
#include <Eigen\Dense>		// ��Ҫ��eigen��Ŀ¼��ӵ�����Ŀ¼�С�



using namespace Eigen;     // �ĳ�������� using Eigen::MatrixXd; 
using namespace std;

#define PI 3.14159

/*
	VectorXd��
			normalized()	�������һ��������
			normalize()		����������й�һ��������
*/





static inline VFQuaternion FromAxisAngle(const Vector3d &axis, float angle)
{
	return VFQuaternion(cos(angle / 2), axis*sin(angle / 2));
}


// �Զ���һ����Ԫ�������һ��
class VFQuaternion
{
public:

// ��Ա����
	union {
		struct {
			double    s;			 
			Vector3d v;		 
		};
		struct {	// ע��ԭ��Ŀ����float����
			double    w;
			double    x;
			double    y;
			double    z;
		};
		struct { double elem[4]; };  
	};

	static VFQuaternion ZERO;
	static VFQuaternion IDENTITY;


//	���캯��
	VFQuaternion() {}

	VFQuaternion(double w0, double x0, double y0, double z0) :w(w0), v(x0, y0, z0) {}

	VFQuaternion(double w0, Vector3d v0) :w(w0), v(v0) {}

	VFQuaternion(const VFQuaternion& q) :s(q.s), v(q.v) {}


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

	// 2.���������
	VFQuaternion &operator =(const VFQuaternion &q)
	{
		s = q.s; v = q.v; return *this;
	}




	// ��������
	void disp()
	{
		cout << "(" << this->w << ", " << this->v[0] << ", " << this->v[1] << ", " << this->v[2] << ")" << endl;
	}




	static VFQuaternion FromRotateAxis(const Vector3d & orign, const Vector3d & dest)
	{
		Vector3d vo, vd;
		vo = orign.normalized();
		vd = dest.normalized();

		Vector3d vAxis = vo.cross(vd);
		double     dot = vo.dot(vd);


		if (vAxis.isZero())
		{
			if (dot > 0)
				return IDENTITY;
			else
			{
				Vector3d vtemp(vo * vo);

				if (vtemp.y >= vtemp.x && vtemp.z >= vtemp.x)
					vtemp = Vector3d(1,0,0);
				else if (vtemp.x >= vtemp.y && vtemp.z >= vtemp.y)
					vtemp = Vector3d(0,1,0);
				else
					vtemp = Vector3d(0,0,1);

				return VFQuaternion::FromAxisAngle(vo.Cross(vtemp), VF_PI);
			}
		}

		return VFQuaternion::FromAxisAngle(vAxis.normalize(), VFMath::ACos(dot).valueRadians());
	}



};

VFQuaternion VFQuaternion::ZERO(0.0, 0.0, 0.0, 0.0);
VFQuaternion VFQuaternion::IDENTITY(1.0, 0.0, 0.0, 0.0);




//	ѧϰ��Ŀ�е�location��Ķ��壬��ƽ�Ʋ�������ת��������װ��һ������ʵ�֡� 
class VFLocation 
{
protected:
	Vector3d    position;					// ��ά��������ʾƽ�Ʋ�����
	VFQuaternion orientation;				// ��Ԫ������ʾ��ת����
	static const VFLocation ORIGIN;			// ԭ�㣬Ĭ�Ϲ��캯�����죬������Ԫ�ض���0��


public:
	VFLocation() {}

	VFLocation(const Vector3d& pos, const VFQuaternion& q):position(pos), orientation(q) {}

	~VFLocation() {}


	// �ֲ��������ά���� �� ȫ���������ά����
	Vector3d TransLocalVertexToGlobal(const Vector3d& v) const
	{
		// ����ת��ƽ�ơ�
		return position + orientation * v;	
	}



};



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
	VFQuaternion q(cos(theta/2),sin(theta/2)*x_hat);		// ��ʾx�����Ԫ��
	v2 = q*v1;
	cout << "v2 == \n" << v2 << endl;
	cout << "q == " << endl;
	q.disp();

	// test 
	cout << endl << endl;
	Vector3d temp = Vector3d(1,1,1);
	//cout << temp.normalized() << endl;
	temp.normalize();
	cout << temp << endl;
	//VFQuaternion qtemp = VFQuaternion::From



}


int main()
{
	//test0();
	test1();
	
	
	
	getchar();
	return 0;
}