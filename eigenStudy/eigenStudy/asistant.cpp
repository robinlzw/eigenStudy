#include "asistant.h"



using namespace std;
using namespace Eigen;

void v3disp(const VFVECTOR3& v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}




// 写一个接口，将矩阵数据保存到.dat文件中，方便python读取然后画图
void writeData2D(const VectorXd& x, const VectorXd& y, const char* filename)
{
	// 顺序挨个写入x和y向量中的数据，先写x再写y，因为两条向量是对应的，所以肯定前一半是x坐标，后一半是y坐标。
	double darr1[MAXLEN];
	double darr2[MAXLEN];
	unsigned int size = x.rows();
	string str1 = filename;
	string str2 = str1;
	
	auto iter = find(str1.begin(), str1.end(), '.');
	if (iter == str1.end())
	{
		cout << "错误，输出的二进制文件必须有后缀名。" << endl;
		return;
	}


	auto dis = distance(str1.begin(), iter);
	str1.insert(dis, "_x");
	str2.insert(dis, "_y");


	for (unsigned int i = 0; i < size; i++)
	{
		darr1[i] = x(i);
	}
	for (unsigned int i = 0; i < size; i++)
	{
		darr2[i] = y(i);
	}

	ofstream file1(str1, ios::out | ios::binary);
	ofstream file2(str2, ios::out | ios::binary);

	file1.write(reinterpret_cast<char*>(&darr1[0]), size * sizeof(double));
	file2.write(reinterpret_cast<char*>(&darr2[0]), size * sizeof(double));
	file1.close();
	file2.close();
}



void readData(VectorXd& x, const char* filename)
{
	ifstream file(filename, ios::in | ios::binary);
	file.seekg(0, file.end);					// 追溯到文件流的尾部
	unsigned int size = file.tellg();			// 获取文件流的长度。
	file.seekg(0, file.beg);					// 回到文件流的头部	

	// 这一块以后考虑用alloctor改写
	char* pc = (char*)malloc(size);
	file.read(pc, size);

	double* pd = reinterpret_cast<double*>(pc);
	for (unsigned int i = 0; i < size/sizeof(double); i++) 
	{
		x[i] = *pd;
		pd++;
	}

}



void writeData3D()
{

}



void writeOBJ()
{}



void readOBJ()
{

}

