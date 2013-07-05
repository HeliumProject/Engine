#include "FrameworkPch.h"
#include "Framework/FlagSet.h"

HELIUM_IMPLEMENT_ASSET( Helium::FlagSetDefinition, Helium::Asset, 0 );

void Helium::FlagSetDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &FlagSetDefinition::m_Flags, "m_Flags" );
}

void Helium::FlagSetDefinition::FinalizeLoad()
{
	m_FlagCount = 0;

	uint64_t bitmask = 1;
	for ( DynamicArray<Name>::Iterator iter = m_Flags.Begin();
		iter != m_Flags.End(); ++iter )
	{
		if ( iter->IsEmpty() )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"A flag in flag set '%s' has an empty name",
				*GetPath().ToString());

			continue;
		}
		
		HashMap< Name, uint64_t >::Iterator mapIter = m_FlagLookup.Find(*iter);
		
		if ( mapIter != m_FlagLookup.End() )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"Flag set '%s' has a duplicate flag named '%s'",
				*GetPath().ToString(),
				*iter);

			continue;
		}

		if ( !bitmask )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"Flag set '%s' has more than 64 flags. Ignoring flag '%s'",
				*GetPath().ToString(),
				*iter);

			continue;
		}

		m_FlagLookup.Insert( mapIter, HashMap< Name, uint64_t >::ValueType( *iter, bitmask ) );
		bitmask = bitmask << 1;

		++m_FlagCount;
	}
}
