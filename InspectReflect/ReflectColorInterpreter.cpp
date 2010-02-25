#include "stdafx.h"
#include "ReflectColorInterpreter.h"

#include "Inspect/Container.h"
#include "Inspect/Label.h"
#include "Inspect/Slider.h"
#include "Inspect/Value.h"
#include "Inspect/ColorPicker.h"
#include "Inspect/StringData.h"

using namespace Reflect;
using namespace Inspect;

ReflectColorInterpreter::ReflectColorInterpreter( Container* container )
: ReflectFieldInterpreter( container )
{

}

void ReflectColorInterpreter::InterpretField( const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent )
{
  ContainerPtr container = m_Container->GetCanvas()->Create<Container>( this );
  parent->AddControl( container );

  LabelPtr label = parent->GetCanvas()->Create<Label>( this );
  label->SetText( field->m_UIName );
  container->AddControl( label );

  bool color3 = field->m_SerializerID == Reflect::GetType<Color3Serializer>() || field->m_SerializerID == Reflect::GetType<HDRColor3Serializer>();
  bool color4 = field->m_SerializerID == Reflect::GetType<Color4Serializer>() || field->m_SerializerID == Reflect::GetType<HDRColor4Serializer>();
  NOC_ASSERT( !(color3 && color4) ); // we shouldn't ever have both!

  if ( color3 || color4 )
  {
    std::vector<Serializer*> ser;
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
      SerializerPtr s;
      
      if ( color3 )
      {
        s = new Color3Serializer();
      }

      if ( color4 )
      {
        s = new Color4Serializer();
      }

      if (s.ReferencesObject())
      {
        s->ConnectField( *itr, field );
        ser.push_back( s );
        m_Serializers.push_back( s );
      }
    }

    if ( !m_Serializers.empty() )
    {
      ColorPickerPtr colorPicker = parent->GetCanvas()->Create<ColorPicker>( this );
      container->AddControl( colorPicker );

      bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;
      colorPicker->SetReadOnly( readOnly );

      DataPtr data = new MultiStringFormatter<Serializer>( ser );
      colorPicker->Bind( data );

      if ( color3 )
      {
        colorPicker->UseAlpha( false );
      }

      if ( color4 )
      {
        colorPicker->UseAlpha( true );

        SliderPtr slider = parent->GetCanvas()->Create<Slider>( this );
        container->AddControl( slider );
        slider->SetRangeMin( 0.0 );
        slider->SetRangeMax( 255.0f );
        slider->SetReadOnly( readOnly );

        ValuePtr value = parent->GetCanvas()->Create<Value>( this );
        container->AddControl( value );
        value->SetReadOnly( readOnly );

        std::vector<Serializer*> alphaSer;
        std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
        std::vector<Reflect::Element*>::const_iterator end = instances.end();
        for ( ; itr != end; ++itr )
        {
          SerializerPtr s = new U8Serializer ();

          PointerSizedUInt fieldAddress = (PointerSizedUInt)(*itr) + field->m_Offset;

          Math::Color4* col = (Math::Color4*)fieldAddress;
          PointerSizedInt offsetInField = (PointerSizedInt)( &col->a ) - fieldAddress;
          s->ConnectField( *itr, field, offsetInField );

          alphaSer.push_back( s );

          m_Serializers.push_back( s );
        }

        data = new MultiStringFormatter<Serializer>( alphaSer );
        slider->Bind( data );
        value->Bind( data );
      }

      if ( field->m_SerializerID == Reflect::GetType<HDRColor3Serializer>() || field->m_SerializerID == Reflect::GetType<HDRColor4Serializer>() )
      {
        SliderPtr slider = parent->GetCanvas()->Create<Slider>( this );
        container->AddControl( slider );
        slider->SetRangeMin( 0.0 );
        slider->SetRangeMax( 8.0 );
        slider->SetReadOnly( readOnly );

        ValuePtr value = parent->GetCanvas()->Create<Value>( this );
        container->AddControl( value );
        value->SetReadOnly( readOnly );

        std::vector<Serializer*> intensitySer;
        std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
        std::vector<Reflect::Element*>::const_iterator end = instances.end();
        for ( ; itr != end; ++itr )
        {
          SerializerPtr s = new F32Serializer();

          PointerSizedUInt fieldAddress = (PointerSizedUInt)(*itr) + field->m_Offset;

          if ( color3 )
          {
            Math::HDRColor3* col = (Math::HDRColor3*)fieldAddress;
            PointerSizedInt offsetInField = (PointerSizedInt)( &col->s ) - fieldAddress;
            s->ConnectField( *itr, field, offsetInField );
          }

          if ( color4 )
          {
            Math::HDRColor4* col = (Math::HDRColor4*)fieldAddress;
            PointerSizedInt offsetInField = (PointerSizedInt)( &col->s ) - fieldAddress;
            s->ConnectField( *itr, field, offsetInField );
          }

          intensitySer.push_back( s );

          m_Serializers.push_back( s );
        }

        data = new MultiStringFormatter<Serializer>( intensitySer );
        slider->Bind( data );
        value->Bind( data );
      }
    }
  }
}
