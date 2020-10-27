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



// 工作项目
//		可以修改simplemesh表示的三维网格数据吗。
void test1() 
{





}




//	测试buffer类模板
/*
	是一个行为像指针的类，封装的是数据的指针。
	成员数据
			unsigned    len;		元素数
			const T *   pData;		数据的指针，注意是底层const，不可以通过指针修改数据。
*/
void test2()
{
	int arri[] = {1,2,3,4,5};
	VSConstBuffer<int> buffer(5,arri);		// 带参构造。



	// VD_V2CB()——STL向量转换为buffer对象，vector to const buffer
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



// 测试TVPersist类模板
/*
	成员数据(private)
			std::vector< char > m_buff;			？？？序列化的数据？
			struct InputPort
			struct CoorectPort

	对外接口
			Get()
					返回封装数据的指针，为T*类型
			Build()
					接受T类型的数据，序列化为自己的成员数据m_buff
			GetBuffer()
					将成员数据m_buff向量封装成buffer然后返回。
*/
void test3() 
{
	TVPersist<VFVECTOR3> vp;
	VFVECTOR3 v1(1,2,3);

	vp.Build(v1);		// 将参数数据序列化，存入到本序列化对象的m_buff成员向量之中。
	

	VFVECTOR3 v2 = vp.Get();							// 返回序列化之前的数据。
	VSConstBuffer<char> buffer_per = vp.GetBuffer();		// 返回序列化的数据，即m_buff成员变量。
	
	v3disp(v2);


	// 看一下序列化后的数据是什么样的。
	const char* pc = buffer_per.pData;
	cout << "序列化之后的数据字节数：" << buffer_per.len << endl;
	cout << "序列化之前的数据字节数：" << sizeof(v1) << endl;
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