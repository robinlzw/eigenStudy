#include <iostream>
#include <Eigen\Dense>		// 需要把eigen的目录添加到包含目录中。



using namespace Eigen;     // 改成这样亦可 using Eigen::MatrixXd; 
using namespace std;

#define PI 3.14159


// 自定义一个四元数类测试一下
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


		// 其他方法
		void disp()
		{
			cout << "(" << this->w << ", " << this->v[0] << ", " << this->v[1] << ", " << this->v[2] << ")" << endl;
		}

	};
}


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
	myQ::Quaternion q(cos(theta/2),sin(theta/2)*x_hat);		// 表示x轴的四元数
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