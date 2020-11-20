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




//	测试VSConstBuffer类模板
/*
	是一个行为像指针的类，封装的是数据的指针。
		成员数据
			unsigned    len;		元素数
			const T *   pData;		数据的指针，注意是底层const，不可以通过指针修改数据。

	生成某个数据对象对应的VSConstBuffer对象的过程中，不发生数据的拷贝。
	VSConstBuffer是一个行为像指针的类。
	对VSConstBuffer对象的拷贝是浅拷贝。
*/
void test2()
{
	int arri[] = { 1,2,3,4,5 };
	VSConstBuffer<int> buffer(5, arri);		// 带参构造。


	// 使用下标运算符访问VCBuffer中的元素：
	for (int i = 0; i < buffer.len; i++) 
	{
		cout << buffer.pData[i] << "   ";
	}
	cout << endl;



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


	
	// 生成某个数据对象对应的VBuffer对象时，不发生数据的拷贝。
	//		VBuffer是一个行为像指针的类。
	int numi = 999;
	VSConstBuffer<int> ibuffer(1, &numi);
	cout << "*pData == " << *ibuffer.pData << endl;

	numi = 888;
	cout << "*pData == " << *ibuffer.pData << endl;




	


}



// 测试TVPersist类模板
/*
成员数据(private)
	std::vector< char > m_buff;			
		序列化后的数据，被序列化的数据被逐字节地写入到本vector对象中。

 
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


	VFVECTOR3 v2 = vp.Get();								// 返回序列化之前的数据。
	VSConstBuffer<char> buffer_per = vp.GetBuffer();		// 返回序列化的数据，即m_buff成员变量。

	v3disp(v2);


	// 看一下序列化后的数据是什么样的。
	const char* pc = buffer_per.pData;
	cout << "序列化之后生成的TVPersist对象的字节数：" << sizeof(vp) / sizeof(char) << endl;
	cout << "序列化之前的数据字节数：" << sizeof(v1) << endl;
	for (int i = 0; i < buffer_per.len; i++)
	{
		cout << *pc << endl;
		pc++;
	}


	// 序列化过程中发生了拷贝。
	float numf = 9.999999;
	TVPersist<float> fper;
	
	cout << "序列化前：sizeof(iper) == " << sizeof(fper) << endl;


	fper.Build(numf);


	numf = 8.88888;
	float result = fper.Get();
	cout << "序列化后：sizeof(iper) == " << sizeof(fper) << endl;
	cout << "result == " << result << endl;	
	cout << "numf == " << numf << endl;

}



// 测试VFQuaternion四元数类
void test4() 
{
	VFQuaternion q1(1, 2, 3, 4);

	// 1. 输入旋转轴向量、旋转角度, 生成四元数对象
	VFQuaternion q2 = VFQuaternion::FromAxisAngle(VFVECTOR3::AXIS_Z, pi/2);
	v3disp(q2*VFVECTOR3(1, 0, 0));


	// 2. 输入目标坐标轴方向向量，生成四元数对象
	VFVECTOR3	x_new, y_new, z_new;
	x_new = VFVECTOR3(1,2,3);
	y_new = VFVECTOR3(0,-3,2);
	z_new = x_new.Cross(y_new);
	x_new.Normalize();
	y_new.Normalize();
	z_new.Normalize();

	VFQuaternion q = VFQuaternion::FromAxes(x_new, y_new, z_new);
	v3disp(q*VFVECTOR3::AXIS_X);


	// 3. 输入原始向量和目标向量，生成四元数对象
	q = VFQuaternion::FromRotateAxis(VFVECTOR3::AXIS_X, VFVECTOR3::AXIS_Y);
	v3disp(q*VFVECTOR3::AXIS_X);
	v3disp(q*VFVECTOR3::AXIS_Y);
	v3disp(q*VFVECTOR3::AXIS_Z);


}



// 测试VFLocation类
void test5()
{
	// 使用VFLocation对象将一个牙齿网格旋转然后平移。

	//VFLocation loc();


}



// 测试TVBuilderArgTuple类模板
void test6()
{
	TVBuilderArgTuple<string> tuple;		// 一个处理string类型数据的argtuple容器
	
	// 1. 生成待处理数据，序列化后写入到文件中
	string str = "hello world hahahahaha";
	TVPersist<string> pstr;
	pstr.Build(str);
	fstream file1("persist_data.dat", ios::out|ios::binary);
	file1 << pstr;
	file1.close();


	// 2. argtuple容器读取文件中的序列化数据
	ifstream file2("persist_data.dat", ios::in | ios::binary);
	tuple.Resume(file2);
	file2.close();


	// 3. 传入函数子处理argtuple中的数据		————打印字符串
	tuple.Travel([](const string& str0)		// 函数子的形参必须是const的，类型应该就是argtuple中序列化数据的原类型
	{
		// argtuple使用函数子对内部数据执行的操作是只读操作。
		cout << str0.length() << endl;
		cout << str0 << endl;

	});




	cout << "finished " << endl;


}



// 测试VFMatrix3类
void test7() 
{
	


}



// 测试TVR2B类
void test8() 
{
	//TVR2B<VFVECTOR3>	v1();
	//
	//VFVECTOR3 vec1(1,2,3);
	//VFQuaternion q1(1, 2, 3, 4);

	//v1.Build(vec1, q1);


}



// 测试碰撞检测类
void test9() 
{

}


// 测试射线检测类：
void test10() 
{


}


}


