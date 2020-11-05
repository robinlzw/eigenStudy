#include "myAsistant.h"

// ".inl"文件的内容一般是内联函数的实现。

inline void v3disp(const VFVECTOR3& v) 
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}



// 写一个接口，将矩阵数据保存到.dat文件中，方便python读取然后画图
inline void writeData2D(const VectorXd& x, const VectorXd& y, const char* filename)
{ 
	// 顺序挨个写入x和y向量中的数据，先写x再写y，因为两条向量是对应的，所以肯定前一半是x坐标，后一半是y坐标。
	ofstream file(filename, ios::out | ios::binary);

	// ？？？貌似vector是链表结构，不能输入其首元素地址来试图将所有元素都写入文件。
	file.write(reinterpret_cast<char*>(), x.size()*sizeof(double));


}

inline void writeData3D()
{

}

inline void writeOBJ()
{}

inline void readOBJ()
{

}
