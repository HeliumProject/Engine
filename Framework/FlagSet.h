#pragma once

#include "Framework/Framework.h"

#include "Foundation/Numeric.h"
#include "Engine/Asset.h"

namespace Helium
{
	class HELIUM_FRAMEWORK_API FlagSetDefinition : public Helium::Asset
	{
		HELIUM_DECLARE_ASSET( FlagSetDefinition, Asset );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

	public:
		virtual void FinalizeLoad();

		size_t GetFlagCount() const 
		{
			return m_FlagCount;
		}

		template <class T>
		bool GetFlag( const Name &name, T &flag )
		{
			HashMap<Name, uint64_t>::Iterator iter = m_FlagLookup.Find( name );
			
			if ( iter == m_FlagLookup.End() )
			{
				return false;
			}

			// Hitting this assert without a warning ahead of time means we did not check SupportsFlagSetDefinition
			HELIUM_ASSERT( iter->Second() <= NumericLimits<T>::Maximum );
			flag = static_cast<T>(iter->Second());

			return true;
		}

		template <class T>
		bool GetBitset( const DynamicArray<Name> &names, T &bitset )
		{
			bitset = 0;
			bool success = true;
			for (DynamicArray< Name >::ConstIterator flagIter = names.Begin(); flagIter != names.End(); ++flagIter)
			{
				T flag;
				if (GetFlag(*flagIter, flag))
				{
					bitset |= flag;
				}
				else
				{
					HELIUM_TRACE(
						TraceLevels::Warning,
						"FlagSetDefinition::GetBitset - Could not find flag '%s' in flag set '%s'\n",
						**flagIter,
						*GetPath().ToString());

					success = false;
				}
			}

			return success;
		}

	private:
		DynamicArray<Name> m_Flags;
		HashMap<Name, uint64_t> m_FlagLookup;
		uint32_t m_FlagCount;
	};
	typedef Helium::StrongPtr< FlagSetDefinition > FlagSetDefinitionPtr;

	template <class T>
	class FlagSetT
	{
		enum
		{
			NUM_BITS_IN_BYTE = 8
		};

		bool SupportsFlagSetDefinition( const FlagSetDefinition &flagSet ) const
		{
			return flagSet.GetFlagCount() <= ( sizeof(T) * NUM_BITS_IN_BYTE );
		}

		bool HasFlag( const FlagSetDefinition *flagSet, const Name &name ) const
		{
			T bitfield;
			bool flagExists = flagSet->GetFlag( name, bitfield );

			return ( flagExists && (m_Flags & bitfield) );
		}

		T m_Flags;
	};

	typedef FlagSetT<uint32_t> FlagSet;
}

#include "Framework/FlagSet.inl"