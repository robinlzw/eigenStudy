#include "stdafx.h"
#include <io.h>
#include <string>
#include <iostream>
#include "lsalgtools/ilsalgtools.h"

void ENV_SetCurrentDirAsExe(void)
{
	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(wcsrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串  	
	SetCurrentDirectory(szFilePath);
}

void DBGTrace(char *format, ...)
{
	char buffer[1000];
	va_list argptr;
	va_start(argptr, format);
	vsprintf_s(buffer, 1000, format, argptr);
	va_end(argptr);
	OutputDebugStringA(buffer);
}

unsigned FILE_GetLength(std::ifstream& fileStream)
{
	if (false == fileStream.is_open())
	{
		return 0xFFFFFFFF;
	}
	std::streampos   pos = fileStream.tellg();     //   save   current   position   
	fileStream.seekg(0, std::ios::end);
	unsigned fileLen = (unsigned)fileStream.tellg();
	fileStream.seekg(pos);     //   restore   saved   position  
	return fileLen;
}


void FILE_GetDirFiles(std::vector<std::string>& vFileName, const std::string& path)
{
	long hFile = 0;
	struct _finddata_t fileInfo;
	std::string pathName, exdName;


	if ((hFile = _findfirst(pathName.assign(path).
		append("\\*").c_str(), &fileInfo)) == -1) {
		return;
	}
	do {
		if (fileInfo.attrib&_A_SUBDIR) {
			std::string fname = std::string(fileInfo.name);
			if (fname != ".." && fname != ".") {
				FILE_GetDirFiles(vFileName, path + "\\" + fname);
			}
		}
		else {
			vFileName.push_back(path + "\\" + fileInfo.name);
		}
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
}

void FILE_GetPathLastName(std::string& strName, const std::string& strPath)
{	
	std::string::size_type idx = strPath.find_last_of("/\\");
	if (idx == strPath.length() - 1)
	{
		std::string strName1 = strPath.substr(0, idx);
		idx = strName1.find_last_of("/\\");
		if (std::string::npos != idx)
		{
			strName = strName1.substr(idx + 1);
		}
		else
			strName = strName1;
	}
	else if (std::string::npos != idx)
		strName = strPath.substr(idx + 1);
}