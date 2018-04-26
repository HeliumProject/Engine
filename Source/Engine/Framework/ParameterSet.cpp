
#include "Precompile.h"
#include "Framework/ParameterSet.h"

using namespace Helium;

//////////////////////////////////////////////////////////////////////////
// ParameterSet

HELIUM_DEFINE_ABSTRACT( Helium::ParameterSet )

ParameterSet::ParameterSet()
	: m_NextParams( NULL )
{

}

void ParameterSet::EnumerateParameters( DynamicArray<Parameter> &parameters ) const
{
	const Reflect::MetaStruct *structure = GetMetaClass();
	for (DynamicArray< Reflect::Field >::ConstIterator iter = structure->m_Fields.Begin();
		iter != structure->m_Fields.End(); ++iter)
	{
		Parameter *p = parameters.New();
		p->m_Name = Name( iter->m_Name ); // Kind of sucks that we do string lookup here! Could we do something better like have names in the field object?
		p->m_Pointer = Reflect::Pointer( &*iter, const_cast< ParameterSet * >( this ), const_cast< ParameterSet *>( this ) );
		p->m_Translator = iter->m_Translator;
	}

	if ( m_NextParams )
	{
		m_NextParams->EnumerateParameters(parameters);
	}
}

void ParameterSet::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

#if 0
//////////////////////////////////////////////////////////////////////////
// ParameterSetDefinition
HELIUM_IMPLEMENT_ASSET( Helium::ParameterSetDefinition, Framework, 0 )

void Helium::ParameterSetDefinition::PopulateMetaType( Reflect::Structure& comp )
{
	comp.AddField( &ParameterSetDefinition::m_ParameterSets, "m_ParameterSets" );
}
#endif

//////////////////////////////////////////////////////////////////////////
// ParameterSet_InitLocated

HELIUM_DEFINE_CLASS( Helium::ParameterSet_InitLocated )

void ParameterSet_InitLocated::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ParameterSet_InitLocated::m_Position, "m_Position" );
	comp.AddField( &ParameterSet_InitLocated::m_Rotation, "m_Rotation" );
}

//////////////////////////////////////////////////////////////////////////
// ParameterSet_InitPhysical
HELIUM_DEFINE_CLASS( Helium::ParameterSet_InitPhysical )

void ParameterSet_InitPhysical::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ParameterSet_InitPhysical::m_Velocity, "m_Velocity" );
}

//////////////////////////////////////////////////////////////////////////
// Old Crap

#if 0
const ParameterSet ParameterSet::EmptyParameterSet;
const Name ParameterSet::ParameterNamePosition("Position"); // Simd::Vector3
const Name ParameterSet::ParameterNameRotation("Rotation"); // Simd::Quat
const Name ParameterSet::ParameterNameVelocity("Velocity"); // Simd::Quat

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
#endif
