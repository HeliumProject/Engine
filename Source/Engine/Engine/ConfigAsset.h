#pragma once

#include "Engine/Asset.h"
#include "Engine/Engine.h"

#include "Reflect/Object.h"

namespace Helium
{
	/// Graphics configuration data.
	class HELIUM_ENGINE_API ConfigAsset : public Asset
	{
		HELIUM_DECLARE_ASSET( Helium::ConfigAsset, Asset );

	public:

		/// @name Construction/Destruction
		//@{
		ConfigAsset();
		virtual ~ConfigAsset();
		//@}

		static void PopulateMetaType( Reflect::MetaStruct& comp );

		Reflect::Object *GetConfigObject() { return m_ConfigObject; }

	private:

		Reflect::ObjectPtr m_ConfigObject;
	};
	typedef Helium::StrongPtr<ConfigAsset> ConfigAssetPtr;
}

