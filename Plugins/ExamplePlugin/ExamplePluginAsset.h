
#include "Reflect/MetaStruct.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"

namespace ExamplePlugin
{
	struct EXAMPLE_PLUGIN_API ExamplePluginStruct : public Helium::Reflect::Struct
	{
		HELIUM_DECLARE_BASE_STRUCT(ExamplePlugin::ExamplePluginStruct);
	
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		inline bool operator==( const ExamplePluginStruct& _rhs ) const;
		inline bool operator!=( const ExamplePluginStruct& _rhs ) const;

		ExamplePluginStruct();

		float m_Value1;
		Helium::Vector3 m_Value2;
	};

	struct EXAMPLE_PLUGIN_API ExamplePluginAsset : public Helium::Asset
	{
	public:
		HELIUM_DECLARE_ASSET(ExamplePlugin::ExamplePluginAsset, Helium::Asset);
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		ExamplePluginAsset();

		Helium::DynamicArray<ExamplePluginStruct> m_StructValues;
		float m_Float;
		bool m_Bool;
	};
	typedef Helium::StrongPtr<ExamplePluginAsset> ExamplePluginAssetPtr;

}