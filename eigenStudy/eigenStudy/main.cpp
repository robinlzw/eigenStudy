#include <iostream>
#include <Eigen\Dense>		// 需要把eigen的目录添加到包含目录中。
#include <vector>
#include <string>


#include "myAsistant.h"		// 自己写的一些辅助函数
#include "vfvector3.h"		// 提供了三维空间向量类VFVECTOR3
#include "vfmesh.h"			// 提供了三维网格的几种数据结构。
#include "vfquaternion.h"	// 提供了四元数类
#include "vflocation.h"		// 提供了实施局部/全局坐标系转换的类VFLocation
#include "obj.h"			// 提供了一堆接口类。
#include "mem.h"			// 提供了VSConstBuffer类
#include "ticmn.h"			// 提供了处理VSConstBuffer等类型数据的函数。
#include "pttrn.h"			// 一些常用的模式
#include "strm.h"			// 提供了TVPersist, TVFilePersist等类模板
#include "projectDS.h"

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




// test0——eigen库的基本数据结构
void test0() 
{
	// 堆矩阵、向量——确定了尺寸，但未初始化,数据存在堆上
	//			最基本模板——Matrix<typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>
	//			堆矩阵——typedef Matrix<double, Dynamic, Dynamic> MatrixXd;
	//			堆向量——typedef Matrix<int, Dynamic, 1> VectorXi;
	MatrixXd m1(2, 2);
	MatrixXf mf1(1, 2);
	VectorXd v1(3);			// 注意是列向量
	cout << v1 << endl;


	// 堆Array
	ArrayXXd a1(2, 2), a2(2, 2);
	a1 << 1, 2, 3, 4;
	a2 << 1, 2, 3, 4;
	cout << "a1 = \n" << a1 << endl;
	cout << "a1*a2 = \n" << a1*a2 << endl;



	// 栈Array



	// 生成特殊矩阵的接口
	MatrixXd m2 = MatrixXd::Random(3, 3);              // 矩阵类static方法——Random()——返回随机矩阵
	MatrixXd m3 = MatrixXd::Constant(3, 3, 1.2);		// 常数矩阵
	MatrixXd m4 = MatrixXd::Ones(1,2);					// 全1矩阵



	// 数据存在栈上的矩阵类型
	Matrix3d mm1 = Matrix3d::Random();
	Vector3d vv1(1, 2, 3);
	cout << "m2 = \n" << m2 << endl << endl;
	cout << "mm1 = \n" << mm1 << endl << endl;
	//		堆矩阵和栈矩阵可以相互赋值。
	mm1 = m2;
	cout << "mm1 = \n" << mm1 << endl << endl;



	// 获取矩阵性质的接口
	cout << m1.rows() << endl;
	cout << m1.cols() << endl;
	cout << m1.size() << endl;


	// 列向量对象可以和矩阵对象相互构造
	Vector3d vv2(1,2,3);			 
	//		
	MatrixXd mm(v1);			
	Vector3d vv3(mm);
	cout << "vv2 = \n" << vv2 << endl << endl;
	cout << "vv3 = \n" << vv3 << endl << endl;
	cout << "mm = \n" << mm << endl << endl;
	cout << "mm.transpose() = \n" << mm.transpose() << endl << endl;


	//		列向量对象可以和矩阵对象相互赋值
	vv2 = m2.block<3,1>(0,0);
	cout << "vv2 = \n" << vv2 << endl << endl;
	mm = vv2;
	cout << "mm = \n" << mm << endl << endl;
}




// test1——矩阵性质、元素访问。
void test1() 
{
	MatrixXd m1(3, 4);
	VectorXd v1(5);

	// 输出流运算符赋值
	m1 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12;
	v1 << 1, 2;
	cout << "v1 = \n" << v1 << endl << endl;

	v1 << 3, 4, 5;			// 不会接着上面的赋值，而是从第一个元素开始赋值
	cout << "v1 = \n" << v1 << endl << endl;


	// 下标运算符[]只能获取向量元素，因为[]只支持一个参数。
	cout << "v1[0] == " << v1[0] << endl << endl;;


	// 括号运算符访问元素，注意索引从0开始
	cout << "m1(0, 1) ==" << m1(0, 1) << endl;
	cout << "v1(3) == " << v1(3) << endl;


	// 求矩阵的性质的类内接口
	cout << "元素数：m1.size() == " << m1.size() << endl;
	cout << "行数：m1.rows() == " << m1.rows() << endl;
	cout << "列数：m1.cols() == " << m1.cols() << endl;
	cout << "求和：sum():       " << m1.sum() << endl;
	cout << "？？？：prod():      " << m1.prod() << endl;
	cout << "均值：mean():      " << m1.mean() << endl;
	cout << "？？？：minCoeff():  " << m1.minCoeff() << endl;
	cout << "？？？：maxCoeff():  " << m1.maxCoeff() << endl;
	cout << "矩阵的迹：trace():     " << m1.trace() << endl << endl;



	// 基本的矩阵变换
	cout << "矩阵的转置：transpose() \n" << m1.transpose() << endl << endl;
	cout << m1 << endl;


}




// test2——矩阵基本变换、运算
void test2() 
{
	MatrixXd m1(3, 4), m3(4, 4);
	m1 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12;
	m3 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;

	cout << "m1 = \n" << endl;
	cout << m1 << endl << endl;

	// MatrixXd::block<>()方法——提取子矩阵。尖括号是子矩阵维度，圆括号表示子矩阵起点。
	cout << m1.block<2, 2>(1, 1) << endl << endl;
	cout << m1.block<1, 3>(1, 0) << endl << endl;

	// 对矩阵的一部分使用输出流运算符赋值
	m1.block<1, 4>(1, 0) << 88, 99, 111, 222;
	cout << "m1 = \n" << m1 << endl << endl;
	m1.block<1, 4>(1, 0) << 5, 6, 7, 8;



	// 更改矩阵尺寸——只适用于堆矩阵、堆向量
	m3.resize(3,3);
	cout << "m3.size() == "<< m3.size() << endl;
	cout << "m3.rows() == " << m3.rows() << endl;
	cout << "m3.cols() == " << m3.cols() << endl;
	cout << "m3 = \n" << m1 << endl << endl;



	// 矩阵按行、按列操作
	cout << m1.colwise().sum() << endl << endl;				// 按列求和，压成一个行向量
	cout << m1.rowwise().sum() << endl << endl;				// 按行求和，压成一个列向量。


	// 矩阵扩张——通过矩阵乘法实现
	MatrixXd  a(1, 3), b(3, 1);
	a << 1, 2, 3;
	b << 4, 5, 6;
	//		行向量扩张N行 == 左乘ones(N,1)
	auto aa = MatrixXd::Ones(3, 1)*a;
	//		列向量扩张为N列 == 右乘ones(1,N)
	auto bb = b*MatrixXd::Ones(1, 4);

	cout << aa << endl << endl;;
	cout << bb << endl << endl;;




	// 矩阵按元素相乘——需要转换到Array中进行：
	MatrixXd m2(3, 4);
	m2 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12;
	ArrayXXd a1(m1), a2(m2);
	ArrayXXd resultA = a1*a2;
	MatrixXd resultM(resultA);
	cout << "resultM == \n" << resultM << endl;



	// 向量点乘、叉乘
	Vector3d v1(1, 2, 3);
	Vector3d v2(3, 2, 1);
	cout << "v1.dot(v2) == \n" << v1.dot(v2) << endl;
	cout << "v1.cross(v2) == " << v1.cross(v2) << endl << endl;;
	




	// 向量归一化
	v1.normalize();					// normalize()会将向量本身归一化，normalized()只是返回归一化向量，不改变自身
	cout << "v1 = \n" << v1 << endl << endl;
	cout << "v2.normalized() = \n" << v2.normalized() << endl << endl;
	cout << "v2 = \n" << v2 << endl << endl;
}





// test3——线性代数科学计算的接口
void test3() 
{
	Matrix3d A;
	A << 1, 2, 3, 4, 5, 6, 7, 8, 9;


	// 求矩阵的特征值、特征向量。
	EigenSolver<Matrix3d> es(A);
	Matrix3d D = es.pseudoEigenvalueMatrix();			// 对角线元素是特征值
	Matrix3d V = es.pseudoEigenvectors();				// 每一个列向量都是特征向量。
	cout << "特征值矩阵D：" << endl << D << endl;
	cout << "特征向量矩阵V: " << endl << V << endl;
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