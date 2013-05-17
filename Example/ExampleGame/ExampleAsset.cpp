#include "ExampleGamePch.h"

#include "ExampleGame/ExampleAsset.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace ExampleGame;

REFLECT_DEFINE_BASE_STRUCTURE( ExampleGame::ExampleStruct );

ExampleGame::ExampleStruct::ExampleStruct()
{

}

void ExampleGame::ExampleStruct::PopulateStructure( Helium::Reflect::Structure& comp )
{
	comp.AddField( &ExampleStruct::m_Value1, "m_Value1" );
	comp.AddField( &ExampleStruct::m_Value2, "m_Value2" );
}

bool ExampleGame::ExampleStruct::operator==( const ExampleStruct& _rhs ) const
{
	return (
		_rhs.m_Value1 == m_Value1 &&
		_rhs.m_Value2 == m_Value2
		);
}

bool ExampleGame::ExampleStruct::operator!=( const ExampleStruct& _rhs ) const
{
    return !( *this == _rhs );
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::ExampleAsset, ExampleGame, 0);

void ExampleAsset::PopulateStructure( Reflect::Structure& comp )
{
    //TODO: m_Shapes won't properly register because BulletShape isn't recognized
    comp.AddField(&ExampleAsset::m_StructValues, TXT( "m_StructValues" ));
    comp.AddField(&ExampleAsset::m_Float, TXT( "m_Float" ));
    comp.AddField(&ExampleAsset::m_Bool, "m_Bool");
}

ExampleAsset::ExampleAsset()
    : m_Bool(false),
	  m_Float(0.0f)
{

}
