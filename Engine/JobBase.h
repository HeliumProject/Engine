
#pragma once

#include "Engine/Engine.h"

#include "Platform/Trace.h"
#include "Foundation/DynamicArray.h"
#include "Engine/JobContext.h"

namespace Helium
{
	class JobContext;

	/// WorldManager update job launcher.
	template< class ParametersType >
	class JobBase : Helium::NonCopyable
	{
	public:
		/// @name Construction/Destruction
		//@{
		inline JobBase() { }
		inline ~JobBase() { }
		//@}

		/// @name Parameters
		//@{
		inline ParametersType& GetParameters() { return m_parameters; }
		inline const ParametersType& GetParameters() const { return m_parameters; }
		inline void SetParameters( const ParametersType& rParameters ) { m_parameters = rParameters; }
		//@}

		/// @name Job Execution
		//@{
		inline void Run( JobContext* pContext );
		inline static void RunCallback( void* pJob, JobContext* pContext )
		{
			HELIUM_ASSERT( pJob );
			HELIUM_ASSERT( pContext );
			static_cast< JobBase< ParametersType >* >( pJob )->Run( pContext );
		}
		//@}

		typedef ParametersType Parameters;

	private:
		ParametersType m_parameters;
	};
}