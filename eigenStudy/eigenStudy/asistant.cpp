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
	double darr[MAXLEN];
	unsigned int size = x.rows();

	for (int i = 0; i < size; i++)
	{
		darr[i] = x(i);
	}

	for (int i = 0; i < size; i++)
	{
		darr[size + i] = y(i);
	}

	ofstream file(filename, ios::out | ios::binary);

	file.write(reinterpret_cast<char*>(&darr[0]), 2 * size * sizeof(double));
	file.close();
}



void readData2D(VectorXd& x, VectorXd& y, const char* filename)
{
	ifstream file(filename, ios::in | ios::binary);
	file.seekg(0, file.end);					// 追溯到文件流的尾部
	unsigned int size = file.tellg();			// 获取文件流的长度。
	file.seekg(0, file.beg);					// 回到文件流的头部	

	// 这一块以后考虑用alloctor改写
	char* pc = (char*)malloc(size);
	file.read(pc, size);

	double* pd = reinterpret_cast<double*>(pc);
	for (int i = 0; i<size/sizeof(double)/2; i++) 
	{
		x[i] = *pd;
		pd++;
	}

	for (int i = 0; i<size / sizeof(double) / 2; i++)
	{
		y[i] = *pd;
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

