#include "ExamplePluginPch.h"

#include "ExamplePlugin/ExamplePluginAsset.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace ExamplePlugin;

HELIUM_DEFINE_BASE_STRUCT( ExamplePlugin::ExamplePluginStruct );

ExamplePluginStruct::ExamplePluginStruct()
{

}

void ExamplePluginStruct::PopulateMetaType( Helium::Reflect::MetaStruct& comp )
{
	comp.AddField( &ExamplePluginStruct::m_Value1, "m_Value1" );
	comp.AddField( &ExamplePluginStruct::m_Value2, "m_Value2" );
}

bool ExamplePluginStruct::operator==( const ExamplePluginStruct& _rhs ) const
{
	return (
		_rhs.m_Value1 == m_Value1 &&
		_rhs.m_Value2 == m_Value2
		);
}

bool ExamplePluginStruct::operator!=( const ExamplePluginStruct& _rhs ) const
{
	return !( *this == _rhs );
}

HELIUM_IMPLEMENT_ASSET(ExamplePlugin::ExamplePluginAsset, ExampleGame, 0);

void ExamplePluginAsset::PopulateMetaType( Reflect::MetaStruct& comp )
{
	//TODO: m_Shapes won't properly register because BulletShape isn't recognized
	comp.AddField(&ExamplePluginAsset::m_StructValues, TXT( "m_StructValues" ));
	comp.AddField(&ExamplePluginAsset::m_Float, TXT( "m_Float" ));
	comp.AddField(&ExamplePluginAsset::m_Bool, "m_Bool");
}

ExamplePluginAsset::ExamplePluginAsset()
	: m_Bool(false),
	  m_Float(0.0f)
{

}
