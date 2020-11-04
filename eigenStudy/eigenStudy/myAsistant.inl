#include "myAsistant.h"

// ".inl"文件的内容一般是内联函数的实现。

inline void v3disp(const VFVECTOR3& v) 
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

