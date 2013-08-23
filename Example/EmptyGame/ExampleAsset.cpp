#include "EmptyGamePch.h"

#include "EmptyGame/ExampleAsset.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace EmptyGame;

HELIUM_DEFINE_BASE_STRUCT( EmptyGame::ExampleStruct );

ExampleStruct::ExampleStruct()
{

}

void ExampleStruct::PopulateMetaType( Helium::Reflect::MetaStruct& comp )
{
	comp.AddField( &ExampleStruct::m_Value1, "m_Value1" );
	comp.AddField( &ExampleStruct::m_Value2, "m_Value2" );
}

bool ExampleStruct::operator==( const ExampleStruct& _rhs ) const
{
	return (
		_rhs.m_Value1 == m_Value1 &&
		_rhs.m_Value2 == m_Value2
		);
}

bool ExampleStruct::operator!=( const ExampleStruct& _rhs ) const
{
    return !( *this == _rhs );
}

HELIUM_IMPLEMENT_ASSET(EmptyGame::ExampleAsset, ExampleGame, 0);

void ExampleAsset::PopulateMetaType( Reflect::MetaStruct& comp )
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
