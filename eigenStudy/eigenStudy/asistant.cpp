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
	double darr1[MAXLEN];
	double darr2[MAXLEN];
	unsigned int size = x.rows();
	string str1 = filename;
	string str2 = str1;
	
	auto iter = find(str1.begin(), str1.end(), '.');
	if (iter == str1.end())
	{
		cout << "��������Ķ������ļ������к�׺����" << endl;
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
	file.seekg(0, file.end);					// ׷�ݵ��ļ�����β��
	unsigned int size = file.tellg();			// ��ȡ�ļ����ĳ��ȡ�
	file.seekg(0, file.beg);					// �ص��ļ�����ͷ��	

	// ��һ���Ժ�����alloctor��д
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

