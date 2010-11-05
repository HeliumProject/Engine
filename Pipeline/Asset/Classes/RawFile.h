#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"


#include "Foundation/TUID.h"
#include "Foundation/Reflect/Registry.h"

namespace Helium
{
	namespace Asset
	{
		class PIPELINE_API RawFile : public AssetClass
		{
		public:

			RawFile()
			{
			}

			REFLECT_DECLARE_CLASS( RawFile, AssetClass );

			static void EnumerateClass( Reflect::Compositor<RawFile>& comp );
		};

		typedef Helium::SmartPtr< RawFile > RawFilePtr;
		typedef std::vector< RawFile > V_RawFile;
	}
}