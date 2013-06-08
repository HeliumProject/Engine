
#include "Reflect/Structure.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"

namespace EmptyGame
{
	struct EMPTY_GAME_API ExampleStruct : public Helium::Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(EmptyGame::ExampleStruct);
	
		static void PopulateStructure( Helium::Reflect::Structure& comp );
		
		inline bool operator==( const ExampleStruct& _rhs ) const;
		inline bool operator!=( const ExampleStruct& _rhs ) const;

		ExampleStruct();

		float m_Value1;
		Helium::Vector3 m_Value2;
	};

	struct EMPTY_GAME_API ExampleAsset : public Helium::Asset
	{
	public:
		HELIUM_DECLARE_ASSET(EmptyGame::ExampleAsset, Helium::Asset);
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		ExampleAsset();

		Helium::DynamicArray<ExampleStruct> m_StructValues;
		float m_Float;
		bool m_Bool;
	};
	typedef Helium::StrongPtr<ExampleAsset> ExampleAssetPtr;

}