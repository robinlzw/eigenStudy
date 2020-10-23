#include <iostream>
#include <Eigen\Dense>		// 需要把eigen的目录添加到包含目录中。

#include "vfvector3.h"		// 提供了三维空间向量类VFVECTOR3
#include "vfmesh.h"			// 提供了三维网格的几种数据结构。
#include "vfquaternion.h"	// 提供了四元数类
#include "vflocation.h"		// 提供了实施局部/全局坐标系转换的类VFLocation


/*
	包含目录：
			工作项目\Base\vmath
			工作项目\Base\vstd

*/


using namespace Eigen;     // 改成这样亦可 using Eigen::MatrixXd; 
using namespace std;

#define PI 3.14159



/*
	VectorXd类
			normalized()	返回其归一化向量。
			normalize()		对象自身进行归一化操作。


*/


// 一点点把工作项目中用到的基础头文件加进来.




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



// 工作项目——可以修改simplemesh表示的三维网格数据吗。
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