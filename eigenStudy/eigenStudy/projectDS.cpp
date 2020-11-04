#include "projectDS.h"
// 工作项目中的自定义数据结构



using namespace Eigen;     // 改成这样亦可 using Eigen::MatrixXd; 
using namespace std;
const double pi = 3.14159;

#define PI 3.14159

namespace PDS 
{


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
	int arri[] = { 1,2,3,4,5 };
	VSConstBuffer<int> buffer(5, arri);		// 带参构造。



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
	VFVECTOR3 v1(1, 2, 3);

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



// 测试VFQuaternion四元数类
void test4() 
{
	VFQuaternion q1(1, 2, 3, 4);

	// 输入旋转轴向量、旋转角度生成四元数对象
	VFQuaternion q2 = VFQuaternion::FromAxisAngle(VFVECTOR3::AXIS_Z, pi/2);
	v3disp(q2*VFVECTOR3(1, 0, 0));


}

}