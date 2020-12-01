#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "lsalgteethprocess.h"

class VCGapObj
{
public:
	VCGapObj(const unsigned nToothLabel1, const unsigned nToothLabel2, IVTracer& tracer);
	~VCGapObj();

public:
	VSGapPointPairs m_pointPairs;
	std::pair<unsigned, unsigned> m_pair;

private:
	IVTracer &	m_tracer;
};
