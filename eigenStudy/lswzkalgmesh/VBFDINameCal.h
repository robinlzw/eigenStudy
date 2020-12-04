#pragma once
#include <algorithm>
#include <sys/alg/isysalgwxj.h>
#include <iomanip>
//#include "VBFDIProbabilityMat.h"

VD_PERSISTCLASS_BEGIN(VSFDICalInput)
    //////////////////////////////////////////////////////////////////////////
    //    18  17  16  15  .......  25  26  27  28
    // t0 -------->                                |
    // t1 -------->                                |
    // t2 -------->                                v
    // .
    // .
    // .
    VD_DEFMEMBER(VSConstBuffer<double>, probMat)

    //////////////////////////////////////////////////////////////////////////
    //     t0   t1   t2   t3   t4   t5 ...
    // t0  0.0        
    // t1       0.0         prob miss 0
    // t2            0.0
    // t3                 0.0
    // t4    prob miss 1       0.0
    // t5                           0.0
    // .
    // .
    // .
    VD_DEFMEMBER(VSConstBuffer<double>, gapMat)

    //////////////////////////////////////////////////////////////////////////
    //         18  17  16  15  .......  25  26  27  28  miss0  miss1  
    // 18                                                 v      v
    // 17                                                 v      v
    // 16                                                 v      v
    // 15                                                 v      v
    // .                                                  .      .
    // .                                                  .      .
    // .                                                  .      .
    // 25                                                 v      v
    // 26                                                 v      v
    // 27                                                 v      v
    // 28                                                 v      v
    // miss0   v   v   v   v   .......   v   v   v   v   
    // miss1   v   v   v   v   .......   v   v   v   v   
    VD_DEFMEMBER(VNWZKALG::VSTranDistribution, tranProb)
VD_PERSISTCLASS_END();

class VBFDINameCal
{
public:
    VBFDINameCal(const VSFDICalInput & probMat) : m_ProbMat(probMat){}

    ~VBFDINameCal() {}

    void Build(VNALGWXJ::VSFDIResult & out, const VSConstBuffer< VNALGWXJ::VEFDI > & initInfo)
    {
        unsigned len = m_ProbMat.probMat.len / 16;

        //std::fstream fsP("D:\\prob_tooth.txt", std::ios::out | std::ios::trunc);
        //for (unsigned i = 0; i < len; i++)
        //{
        //    for (unsigned j = 0; j < 16; j++)
        //    {
        //        fsP << m_ProbMat.probMat.pData[ i * 16 + j ] << " ";
        //    }
        //    fsP << std::endl;
        //}
        //
        //std::fstream fsG("D:\\prob_gap.txt", std::ios::out | std::ios::trunc);
        //for (unsigned i = 0; i < len; i++)
        //{
        //    for (unsigned j = 0; j < len; j++)
        //    {
        //        fsG << m_ProbMat.gapMat.pData[ i * len + j ] << " ";
        //    }
        //    fsG << std::endl;
        //}

        if (initInfo.len == len)
        {
            out.m_CouldCal = FDInameCalc2(initInfo);
            out.m_Result = VD_V2CB(m_vFDInameModify);
            //if (out.m_Result.len != initInfo.len)
            //    out.m_CouldCal = false;
        }
        else
        {
            out.m_CouldCal = false;
            out.m_Result = VD_V2CB(m_vFDInameModify);
        }
    }

private:
    unsigned SupernumerraryCheck(const VSConstBuffer< VNALGWXJ::VEFDI > & initInfo);
    void     FixedToothCheck(std::vector<VNVECTOR2UI> & vec, const VSConstBuffer< VNALGWXJ::VEFDI > & initInfo);
    int      Filter(const std::vector<VNVECTOR2UI> & fixedTooth, const std::vector<unsigned> & permuVec); //suc: 0, fail: other
    int      FilterEnds(const std::vector<VNVECTOR2UI> & fixedTooth, unsigned emptyNum);
    double   ProbCal(const std::vector<unsigned> & permuVec);
    bool     FDInameCalc(const VSConstBuffer< VNALGWXJ::VEFDI > & initInfo);
    void     FDIModify(unsigned supNum, unsigned toothNum);
    void     FullTooth(unsigned supNum);

    unsigned m_Map_FDItoIdx[ 18 ] = { VD_INVALID_INDEX , VD_INVALID_INDEX , 7 , 6 , 5 , 4 , 3 , 2 , 1 , 0 , 8 , 9 , 10 , 11 , 12 , 13 , 14 , 15 };
    unsigned m_Map_IdxtoFDI[ 16 ] = { VNALGWXJ::FDIL8 , VNALGWXJ::FDIL7 , VNALGWXJ::FDIL6 , VNALGWXJ::FDIL5 , VNALGWXJ::FDIL4 , VNALGWXJ::FDIL3 , VNALGWXJ::FDIL2 , VNALGWXJ::FDIL1 , VNALGWXJ::FDIR1 , VNALGWXJ::FDIR2 , VNALGWXJ::FDIR3 , VNALGWXJ::FDIR4 , VNALGWXJ::FDIR5 , VNALGWXJ::FDIR6 , VNALGWXJ::FDIR7 , VNALGWXJ::FDIR8 };

    bool     FDInameCalc2(const VSConstBuffer< VNALGWXJ::VEFDI > & initInfo)
    {
        std::vector< unsigned > changedInfo;
        std::vector< unsigned > changedIdxs;
        for (unsigned i = 0; i < initInfo.len; i++)
        {
            if (initInfo.pData[ i ] != VNALGWXJ::FDI_Supernumerary_Tooth) {
                changedIdxs.push_back(i);
                changedInfo.push_back(m_Map_FDItoIdx[ initInfo.pData[ i ] ]);
            }
        }
        unsigned poscount = changedIdxs.size();

        unsigned nolimitcount = calNonLimitCount(poscount, 16);
        std::vector< unsigned > nolimits(nolimitcount * poscount);
        calNonLimitData(&nolimits[ 0 ], 0, poscount, poscount, 16);

        std::vector< unsigned > possableIdxs;

        for (unsigned i = 0; i < nolimitcount; i++)
        {
            if (checkLineNo2Space(&nolimits[ i * poscount ], poscount)
                && checkLineFitSpec(&nolimits[ i * poscount ], poscount, changedInfo))
            {
                possableIdxs.push_back(i);
            }
        }

        if (possableIdxs.empty())
        {
            return false;
        }
        else
        {
            unsigned maxIdx = possableIdxs[ 0 ];
            double maxVal = calLineValue(&nolimits[ possableIdxs[ 0 ] * poscount ], poscount, changedIdxs);
            for (unsigned i = 1; i < possableIdxs.size(); i++)
            {
                double val = calLineValue(&nolimits[ possableIdxs[ i ] * poscount ], poscount, changedIdxs);
                if (val > maxVal)
                {
                    maxIdx = possableIdxs[ i ];
                }
            }

            unsigned * pRtLine = &nolimits[ maxIdx * poscount ];

            m_vFDInameModify.resize(initInfo.len);
            unsigned curIdx = 0;
            for (unsigned i = 0; i < initInfo.len; i++)
            {
                if (initInfo.pData[ i ] != VNALGWXJ::FDI_Supernumerary_Tooth)
                {
                    m_vFDInameModify[ i ] = VNALGWXJ::VEFDI(m_Map_IdxtoFDI[ pRtLine[ curIdx ] ]);
                    curIdx += 1;
                }
                else 
                {
                    m_vFDInameModify[ i ] = VNALGWXJ::FDI_Supernumerary_Tooth;
                }
            }

            return true;
        }
    }

    unsigned calNonLimitCount( unsigned posCount , unsigned itemCount ) {
        if (posCount > itemCount) 
        {
            return 0;
        }
        else if (posCount == 0)
        {
            return 1;
        }

        unsigned count = 0;
        for (unsigned i = 0; i < itemCount; i++)
        {
            count += calNonLimitCount(posCount - 1, itemCount - i - 1);
        }
        return count;
    }

    void calNonLimitData(unsigned * pStart, unsigned valStart, unsigned lineSize, unsigned posCount, unsigned itemCount)
    {
        if (posCount > itemCount)
        {
            return;
        }

        unsigned lineOffset = 0;
        for (unsigned i = 0; i < itemCount; i++)
        {
            unsigned count = calNonLimitCount(posCount - 1, itemCount - i - 1);
            unsigned * pOff = pStart + lineOffset * lineSize;
            for (unsigned j = 0; j < count; j++)
            {
                pOff[ j * lineSize ] = valStart + i;
            }

            if (posCount != 1)
            {
                calNonLimitData(pOff + 1, valStart + i + 1, lineSize, posCount - 1, itemCount - i - 1);
            }

            lineOffset += count;
        }
    }

    bool checkLineNo2Space(unsigned * pLine, unsigned lineSize)
    {
        for (unsigned i = 0; i < lineSize - 1; i++)
        {
            unsigned pre = pLine[ i ];
            unsigned nxt = pLine[ i + 1 ];

            if (nxt - pre > 2)
            {
                return false;
            }
        }
        return true;
    }

    bool checkLineFitSpec(unsigned * pLine, unsigned lineSize, const std::vector< unsigned > & initInfo)
    {
        VASSERT(lineSize == initInfo.size());

        for (unsigned i = 0; i < lineSize; i++)
        {
            if (initInfo[ i ] != VD_INVALID_INDEX && pLine[i] != initInfo[ i ])
            {
                return false;
            }
        }
        return true;
    }

    double calLineValue(unsigned * pLine, unsigned lineSize, const std::vector<unsigned> & changedIdxs)
    {
        unsigned toothCount = m_ProbMat.probMat.len / 16;

        VASSERT(lineSize == changedIdxs.size());
        double rtVal = 0.0;
        for( unsigned i = 0 ; i < lineSize ; i++)
        {
            rtVal += getToothFit(changedIdxs[ i ], pLine[ i ]);
        }
        rtVal *= 0.5;
        for (unsigned i = 0; i < lineSize - 1; i++)
        {
            rtVal += getGapFit(pLine[ i + 1 ] - pLine[ i ] - 1, changedIdxs[ i ], changedIdxs[ i + 1 ], toothCount);
        }
        for (unsigned i = 0; i < lineSize - 1; i++)
        {
            rtVal += getChainToothToGapFit(pLine[ i ], pLine[ i + 1 ] - pLine[ i ] - 1);
            rtVal += getChainGapToToothFit(pLine[ i + 1 ] - pLine[ i ] - 1, pLine[ i + 1 ]);
        }
        return rtVal;
    }

    double getToothFit(unsigned toothIdx, unsigned FDIPos)
    {
        VASSERT(FDIPos < 16);

        const double * start = &m_ProbMat.probMat.pData[ 0 ];
        const double * toothLine = start + toothIdx * 16;
        return toothLine[ FDIPos ];
    }

    double getGapFit(unsigned missCount, unsigned preToothIdx, unsigned aftToothIdx, unsigned totalToothCount)
    {
        VASSERT(missCount < 2);
        VASSERT(preToothIdx * totalToothCount + aftToothIdx < m_ProbMat.gapMat.len);
        VASSERT(aftToothIdx * totalToothCount + preToothIdx < m_ProbMat.gapMat.len);

        if (missCount == 0)
        {
            return m_ProbMat.gapMat.pData[ preToothIdx * totalToothCount + aftToothIdx ];
        }
        else if (missCount == 1)
        {
            return m_ProbMat.gapMat.pData[ aftToothIdx * totalToothCount + preToothIdx ];
        }
        else
        {
            return 0.0;
        }
    }

    double getChainToothToGapFit(unsigned FDIPos, unsigned missCount) {
        VASSERT(FDIPos * 2 + missCount < m_ProbMat.tranProb.TranProbCol.len);
        return m_ProbMat.tranProb.TranProbCol.pData[ FDIPos * 2 + missCount ];
    }

    double getChainGapToToothFit(unsigned missCount, unsigned FDIPos) {
        VASSERT(missCount * 16 + FDIPos < m_ProbMat.tranProb.TranProbRow.len);
        return m_ProbMat.tranProb.TranProbRow.pData[ missCount * 16 + FDIPos ];
    }

private:
    const VSFDICalInput & m_ProbMat;
    unsigned num, supNum, toothNum; //多生牙和实际牙齿的个数
    //std::vector<double>    ttt;

    std::vector<VNALGWXJ::VEFDI>     m_vFDInameModify;
    double prob;     //最大概率值
    std::vector<unsigned>            m_vPermu;   //对应最大概率的组合
    std::vector<VNALGWXJ::VEFDI>     m_vFDIname;
    std::vector<unsigned>            m_vPermuTemp;
    std::vector<unsigned>            m_vSupernumerrary;     //多生牙在初始设置中的位置
    std::vector<VNVECTOR2UI>         m_vFixedTooth;         //固定牙齿
    std::vector<VNALGWXJ::VEFDI>     m_vModifyTemp;
};

VD_BEGIN_PIPELINE( VPLFDICalculator, VSFDICalInput)
    VDDP_NOD2(FDINameCal, VBFDINameCal);
VD_END_PIPELINE()

class VMFDINameCal
{
public:
    VMFDINameCal(VPLFDICalculator & pl
        , IVSlot<VSConstBuffer< VNALGWXJ::VEFDI >> & slt
        , IVInputPort< VNALGWXJ::VSFDIResult > & ip)
    {
        pl.CreateFDINameCal(m_Cal, pl.GetEnv().Get<VSFDICalInput>());
        VLNK(m_Cal, slt);
        VLNK(ip, m_Cal);
    }
    ~VMFDINameCal() {}

private:
    VPLFDICalculator::FDINameCal m_Cal;
};

struct VSysTraitFDICalculator
{
    typedef VPLFDICalculator PIPELINE;
    template< typename TR > struct TRTrait;
    template<> struct TRTrait< VNALGWXJ::VRFDICalculator > { typedef VMFDINameCal mngr_type; };
};

using VSysFDICalculator = TVSystemProviderImp< VSysTraitFDICalculator, IVSYS< VNALGWXJ::VRFDICalculator > >;