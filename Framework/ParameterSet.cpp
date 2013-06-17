
#include "FrameworkPch.h"
#include "Framework/ParameterSet.h"

using namespace Helium;

const ParameterSet ParameterSet::EmptyParameterSet;
const Name ParameterSet::ParameterNamePosition("Position"); // Simd::Vector3
const Name ParameterSet::ParameterNameRotation("Rotation"); // Simd::Quat

ParameterSet::ParameterSet( size_t _block_size /*= 128*/ )
#if HELIUM_HEAP
	: m_ParameterAllocator(_block_size)
#endif
{

}

ParameterSet::~ParameterSet()
{
	for (size_t index = 0; index < m_Parameters.GetSize(); ++index)
	{
		HELIUM_DELETE(m_ParameterAllocator, m_Parameters[m_Parameters.GetSize() - index - 1]);
		m_Parameters.Clear();
	}
}
