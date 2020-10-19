#include <iostream>
#include <Eigen\Dense>		// 需要把eigen的目录添加到包含目录中。



using namespace Eigen;     // 改成这样亦可 using Eigen::MatrixXd; 
using namespace std;

#define PI 3.14159

/*
	VectorXd类
			normalized()	返回其归一化向量。
			normalize()		对象自身进行归一化操作。
*/





static inline VFQuaternion FromAxisAngle(const Vector3d &axis, float angle)
{
	return VFQuaternion(cos(angle / 2), axis*sin(angle / 2));
}


// 自定义一个四元数类测试一下
class VFQuaternion
{
public:

// 成员数据
	union {
		struct {
			double    s;			 
			Vector3d v;		 
		};
		struct {	// 注意原项目中是float类型
			double    w;
			double    x;
			double    y;
			double    z;
		};
		struct { double elem[4]; };  
	};

	static VFQuaternion ZERO;
	static VFQuaternion IDENTITY;


//	构造函数
	VFQuaternion() {}

	VFQuaternion(double w0, double x0, double y0, double z0) :w(w0), v(x0, y0, z0) {}

	VFQuaternion(double w0, Vector3d v0) :w(w0), v(v0) {}

	VFQuaternion(const VFQuaternion& q) :s(q.s), v(q.v) {}


	// 运算符重载
	//		四元数乘以三维向量——表示该向量v0被该四元素施加旋转操作。
	//		四元数和旋转角的关系——q = cos(theta/2) + sin(theta/2)*A; 其中A是旋转轴的单位向量。
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

	// 2.运算符重载
	VFQuaternion &operator =(const VFQuaternion &q)
	{
		s = q.s; v = q.v; return *this;
	}




	// 其他方法
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




//	学习项目中的location类的定义，将平移操作和旋转操作都封装到一个类内实现。 
class VFLocation 
{
protected:
	Vector3d    position;					// 三维向量，表示平移操作。
	VFQuaternion orientation;				// 四元数，表示旋转操作
	static const VFLocation ORIGIN;			// 原点，默认构造函数构造，即所有元素都是0；


public:
	VFLocation() {}

	VFLocation(const Vector3d& pos, const VFQuaternion& q):position(pos), orientation(q) {}

	~VFLocation() {}


	// 局部坐标的三维向量 → 全局坐标的三维向量
	Vector3d TransLocalVertexToGlobal(const Vector3d& v) const
	{
		// 先旋转再平移。
		return position + orientation * v;	
	}



};



// 第一个eigen程序
void test0() 
{
	MatrixXd m = MatrixXd::Random(3, 3);              // 矩阵类static方法——Random()——返回随机矩阵
	VectorXd v(3);										// 定义v为3*1的double型向量
	Vector3d v1;
	
	// 矩阵类的static方法——Constant()——返回常数矩阵。
	m = (m + MatrixXd::Constant(3, 3, 1.2)) * 50;      //MatrixXd::Constant(3,3,1.2)表示生成3*3的double型矩阵，该矩阵所有元素均为1.2
	cout << "m == \n" << endl << m << endl;


	// 向量类重载了流输出符。
	v << 1, 2, 3;							// 向量赋值
	cout << "m * v == \n" << endl << m * v << endl;


	v1 << 1, 2, 3;        
	cout << "v1 == \n" << v1 << endl;
	cout << "v1[0] == " << v1[0] << endl;			// 元素下标访问

}





// 测试三维向量的旋转
void test1() 
{
	// 左手坐标系xoy平面的向量(1,1,0)绕x轴正方向旋转90度，应该变成xoz平面的向量(1,0,1) 

	// 1. 变换矩阵方法
	VectorXd v1(3), v2(3);
	v1 << 1, 1, 0;

	MatrixXd Rx(3,3);		// 绕X轴正方向旋转90度的变换矩阵：
	Rx << 1, 0, 0, 0, 0, -1, 0, 1, 0;
	cout << "v1 == \n" << v1 << endl;
	cout << "Rx == \n" << Rx << endl;

	v2 = Rx*v1;
	cout << "v2 == \n" << v2 << endl << endl << endl;;


	// 2. 自定义四元数方法
	double theta = PI/2;
	Vector3d x_hat(1,0,0);
	//			注意cos()sin()函数只认弧度。
	VFQuaternion q(cos(theta/2),sin(theta/2)*x_hat);		// 表示x轴的四元数
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