#pragma once
#include <interface/iobjreference.h>
#include <obj/alg/ialgcutpathgen.h>
namespace NMALG_CUTPATHGEN
{
	class VCFindPointPairPath
	{
	public:
		VCFindPointPairPath();
		~VCFindPointPairPath();

		void Gen(VSConstBuffer<unsigned>& cbPointPair, const VSCutPathGenInput& input);

	private:
		std::vector<unsigned> m_vPointSet;
	};
}
