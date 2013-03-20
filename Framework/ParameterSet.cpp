
#include "FrameworkPch.h"
#include "Framework/ParameterSet.h"

Helium::ParameterSet::ParameterSet( size_t _block_size /*= 128*/ ) : m_Heap(_block_size)
{

}

Helium::ParameterSet::~ParameterSet()
{
    for (size_t index = 0; index < m_Parameters.GetSize(); ++index)
    {
        HELIUM_DELETE(m_Heap, m_Parameters[m_Parameters.GetSize() - index - 1]);
        m_Parameters.Clear();
    }
}
