#pragma once

#include <string>
#include <fstream>
#include <map>
#include <windows.h>

class VProfile
{
public:
    VProfile() 
    {
        QueryPerformanceFrequency(&frequency);
    }
    ~VProfile() {}

    void StartProfile()
    {
        QueryPerformanceCounter(&timeStart);
    }

    double FinishProfile()
    {
        QueryPerformanceCounter(&timeEnd);
        double quadpart = (double)frequency.QuadPart;//��ʱ��Ƶ��
        double elapsed = (timeEnd.QuadPart - timeStart.QuadPart) / quadpart;
        return elapsed;
    }

private:
    LARGE_INTEGER frequency;	//��ʱ��Ƶ��
    LARGE_INTEGER timeStart;	//��ʼʱ��
    LARGE_INTEGER timeEnd;		//����ʱ��
};



// VSFProfile�ࡪ������������ò����д����ִ�м�¼txt���ࣿ����
class VSFProfile
{
public:
    VSFProfile() {}
    ~VSFProfile() {}

    static void StartProfile(std::string pName, std::string filepath)
    {
        std::fstream fs(filepath, std::ios::out | std::ios::app);
        fs << pName << " start" << std::endl;
        fs.close();

        m_ProfileMap.insert(std::make_pair(pName, VProfile()));
        m_ProfileMap.find(pName)->second.StartProfile();
    }

    static void FinishProfile(std::string pName, std::string filepath)
    {
        double seconds = m_ProfileMap.find(pName)->second.FinishProfile();

        std::fstream fs(filepath, std::ios::out | std::ios::app);
        fs << pName << " finish : " << seconds << std::endl;
        fs.close();
    }

private:
    static std::map< std::string, VProfile > m_ProfileMap;
};

#define VD_F_PROFILE_START( profile ) //VSFProfile::StartProfile(#profile,"D:\\profile.txt");
#define VD_F_PROFILE_FINISH( profile ) //VSFProfile::FinishProfile(#profile,"D:\\profile.txt");