#include "myAsistant.h"

// ".inl"�ļ�������һ��������������ʵ�֡�

inline void v3disp(const VFVECTOR3& v) 
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}



// дһ���ӿڣ����������ݱ��浽.dat�ļ��У�����python��ȡȻ��ͼ
inline void writeData2D(const VectorXd& x, const VectorXd& y, const char* filename)
{ 
	// ˳�򰤸�д��x��y�����е����ݣ���дx��дy����Ϊ���������Ƕ�Ӧ�ģ����Կ϶�ǰһ����x���꣬��һ����y���ꡣ
	ofstream file(filename, ios::out | ios::binary);

	// ������ò��vector������ṹ��������������Ԫ�ص�ַ����ͼ������Ԫ�ض�д���ļ���
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
