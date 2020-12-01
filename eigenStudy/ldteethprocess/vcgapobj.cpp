#include "stdafx.h"
#include "vcgapobj.h"
#include "vctoothpatchside.h"
#include "vctoothpatchbtm.h"
#include "vctoothmakematrixs.h"
#include "vctoothfindstrawhatside.h"

VCGapObj::VCGapObj(const unsigned nToothLabel1, const unsigned nToothLabel2, IVTracer& tracer)
	:m_tracer(tracer)
{
	if (nToothLabel1 < nToothLabel2)
	{
		m_pair.first = nToothLabel1;
		m_pair.second = nToothLabel2;
	}
	else
	{
		m_pair.first = nToothLabel2;
		m_pair.second = nToothLabel1;
	}
}

VCGapObj::~VCGapObj()
{

}

