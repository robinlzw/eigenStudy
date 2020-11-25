#pragma once

#include "obj.h"
#include "../vmath/vnvector.h"

#ifdef WIN32
#include <windows.h>
#endif

struct VSRenderContext
{
#ifdef WIN32
    HDC     m_hDC;
    HGLRC   m_hRC;
#endif
};

struct VSDOMNodeID
{
    VSConstBuffer< wchar_t >    m_ID;
};

struct VSDOMNodeProperty
{
    VSConstBuffer< wchar_t >    m_Name;
    VSConstBuffer< wchar_t >    m_Value;
};

struct VSDOMNode
{
    VSConstBuffer< wchar_t >            m_Label;
    VSConstBuffer< wchar_t >            m_Content;
    VSDOMNodeID                         m_ID;
    VSConstBuffer< VSDOMNodeProperty >  m_Propertys;
    VSConstBuffer< VSDOMNode >          m_Children;
};

struct VSDOMTree
{
    VSDOMNode   m_root;
};

// 三键鼠标
struct VSCommonTypeMouse
{
    enum VNMouseTypeOfs
    {
        Ofs_LBtn ,
        Ofs_RBtn ,
        Ofs_MBtn ,
    };

    enum VNMouseType
    {
        LBtn = 1 << Ofs_LBtn ,
        RBtn = 1 << Ofs_RBtn ,
        MBtn = 1 << Ofs_MBtn ,
    };

    VNVECTOR2I  m_Pos;
    int         m_BtnState;
};

// 标准长键盘
struct VSCommonTypeKeyBoard
{
    enum VNLetterTypeOfs
    {
        Ofs_A   ,
        Ofs_B   ,
        Ofs_C   ,
        Ofs_D   ,
        Ofs_E   ,
        Ofs_F   ,
        Ofs_G   ,
        Ofs_H   ,
        Ofs_I   ,
        Ofs_J   ,
        Ofs_K   ,
        Ofs_L   ,
        Ofs_M   ,
        Ofs_N   ,
        Ofs_O   ,
        Ofs_P   ,
        Ofs_Q   ,
        Ofs_R   ,
        Ofs_S   ,
        Ofs_T   ,
        Ofs_U   ,
        Ofs_V   ,
        Ofs_W   ,
        Ofs_X   ,
        Ofs_Y   ,
        Ofs_Z   ,
    };

    enum VNLetterType
    {
        K_A = 1 << Ofs_A   ,
        K_B = 1 << Ofs_B   ,
        K_C = 1 << Ofs_C   ,
        K_D = 1 << Ofs_D   ,
        K_E = 1 << Ofs_E   ,
        K_F = 1 << Ofs_F   ,
        K_G = 1 << Ofs_G   ,
        K_H = 1 << Ofs_H   ,
        K_I = 1 << Ofs_I   ,
        K_J = 1 << Ofs_J   ,
        K_K = 1 << Ofs_K   ,
        K_L = 1 << Ofs_L   ,
        K_M = 1 << Ofs_M   ,
        K_N = 1 << Ofs_N   ,
        K_O = 1 << Ofs_O   ,
        K_P = 1 << Ofs_P   ,
        K_Q = 1 << Ofs_Q   ,
        K_R = 1 << Ofs_R   ,
        K_S = 1 << Ofs_S   ,
        K_T = 1 << Ofs_T   ,
        K_U = 1 << Ofs_U   ,
        K_V = 1 << Ofs_V   ,
        K_W = 1 << Ofs_W   ,
        K_X = 1 << Ofs_X   ,
        K_Y = 1 << Ofs_Y   ,
        K_Z = 1 << Ofs_Z   ,
    };

    enum VNNumFuncnumTypeOfs
    {
        Ofs_1   ,
        Ofs_2   ,
        Ofs_3   ,
        Ofs_4   ,
        Ofs_5   ,
        Ofs_6   ,
        Ofs_7   ,
        Ofs_8   ,
        Ofs_9   ,
        Ofs_0   ,
        Ofs_F1  ,
        Ofs_F2  ,
        Ofs_F3  ,
        Ofs_F4  ,
        Ofs_F5  ,
        Ofs_F6  ,
        Ofs_F7  ,
        Ofs_F8  ,
        Ofs_F9  ,
        Ofs_F10 ,
        Ofs_F11 ,
        Ofs_F12 ,
    };

    enum VNNumFuncnumType
    {
        K_1   = 1 << Ofs_1   ,
        K_2   = 1 << Ofs_2   ,
        K_3   = 1 << Ofs_3   ,
        K_4   = 1 << Ofs_4   ,
        K_5   = 1 << Ofs_5   ,
        K_6   = 1 << Ofs_6   ,
        K_7   = 1 << Ofs_7   ,
        K_8   = 1 << Ofs_8   ,
        K_9   = 1 << Ofs_9   ,
        K_0   = 1 << Ofs_0   ,
        K_F1  = 1 << Ofs_F1  ,
        K_F2  = 1 << Ofs_F2  ,
        K_F3  = 1 << Ofs_F3  ,
        K_F4  = 1 << Ofs_F4  ,
        K_F5  = 1 << Ofs_F5  ,
        K_F6  = 1 << Ofs_F6  ,
        K_F7  = 1 << Ofs_F7  ,
        K_F8  = 1 << Ofs_F8  ,
        K_F9  = 1 << Ofs_F9  ,
        K_F10 = 1 << Ofs_F10 ,
        K_F11 = 1 << Ofs_F11 ,
        K_F12 = 1 << Ofs_F12 ,
    };

    int m_LetterBtnState;
    int m_NumAndFuncNumBtnState;
    int m_LeftZoneCtrlBtnState;  // unfinish enum
    int m_RightZoneCtrlBtnState; // unfinish enum
};

struct VSWindow
{
    VNVECTOR2I      m_WndSize;
    VSRenderContext m_RC;
};