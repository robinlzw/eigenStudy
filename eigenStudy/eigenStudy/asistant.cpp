#include "asistant.h"



using namespace std;
using namespace Eigen;

void v3disp(const VFVECTOR3& v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}




// дһ���ӿڣ����������ݱ��浽.dat�ļ��У�����python��ȡȻ��ͼ
void writeData2D(const VectorXd& x, const VectorXd& y, const char* filename)
{
	// ˳�򰤸�д��x��y�����е����ݣ���дx��дy����Ϊ���������Ƕ�Ӧ�ģ����Կ϶�ǰһ����x���꣬��һ����y���ꡣ
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
	file.seekg(0, file.end);					// ׷�ݵ��ļ�����β��
	unsigned int size = file.tellg();			// ��ȡ�ļ����ĳ��ȡ�
	file.seekg(0, file.beg);					// �ص��ļ�����ͷ��	

	// ��һ���Ժ�����alloctor��д
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

