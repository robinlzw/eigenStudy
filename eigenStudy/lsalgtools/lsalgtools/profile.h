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
        double quadpart = (double)frequency.QuadPart;//计时器频率
        double elapsed = (timeEnd.QuadPart - timeStart.QuadPart) / quadpart;
        return elapsed;
    }

private:
    LARGE_INTEGER frequency;	//计时器频率
    LARGE_INTEGER timeStart;	//开始时间
    LARGE_INTEGER timeEnd;		//结束时间
};



// VSFProfile类————？？？貌似是写流程执行记录txt的类？？？
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