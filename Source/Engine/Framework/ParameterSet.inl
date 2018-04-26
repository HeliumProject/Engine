
namespace Helium
{

	template <class T>
	T *ParameterSet::FindParameterSet()
	{
		// Find the structure we're looking for
		const Reflect::MetaStruct *searchType = Reflect::GetMetaClass<T>();
		HELIUM_ASSERT(searchType);

		// Iterate through parameter set chain
		ParameterSet *parameterSet = this;
		while ( parameterSet )
		{
			// See if it is of type T
			const Reflect::MetaStruct *structure = parameterSet->GetMetaClass();
			HELIUM_ASSERT( structure );
			if ( structure->IsType( searchType ) )
			{
				// It is! Return it
				return static_cast<T *>( parameterSet );
			}

			parameterSet = m_NextParams;
		}

		// Give up, we did not find T in the chain
		return NULL;
	}
}
