#include "ReflectColorInterpreter.h"

#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Controls/SliderControl.h"
#include "Foundation/Inspect/Controls/ValueControl.h"
#include "Foundation/Inspect/Controls/ColorPickerControl.h"
#include "Foundation/Inspect/Data.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectColorInterpreter::ReflectColorInterpreter( Container* container )
: ReflectFieldInterpreter( container )
{

}

void ReflectColorInterpreter::InterpretField( const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent )
{
    ContainerPtr container = CreateControl< Container >();
    parent->AddChild( container );

    LabelPtr label = CreateControl< Label >();

    tstring temp;
    bool converted = Helium::ConvertString( field->m_UIName, temp );
    HELIUM_ASSERT( converted );

    label->BindText( temp );

    container->AddChild( label );

    bool color3 = field->m_SerializerID == Reflect::GetType<Color3Serializer>() || field->m_SerializerID == Reflect::GetType<HDRColor3Serializer>();
    bool color4 = field->m_SerializerID == Reflect::GetType<Color4Serializer>() || field->m_SerializerID == Reflect::GetType<HDRColor4Serializer>();
    HELIUM_ASSERT( !(color3 && color4) ); // we shouldn't ever have both!

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
            ColorPickerPtr colorPicker = CreateControl<ColorPicker>();
            container->AddChild( colorPicker );

            bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;
            colorPicker->a_IsReadOnly.Set( readOnly );

            DataPtr data = new MultiStringFormatter<Serializer>( ser );
            colorPicker->Bind( data );

            if ( color3 )
            {
                colorPicker->a_Alpha.Set( false );
            }

            if ( color4 )
            {
                colorPicker->a_Alpha.Set( true );

                SliderPtr slider = CreateControl<Slider>();
                container->AddChild( slider );
                slider->a_Min.Set( 0.0 );
                slider->a_Max.Set( 255.0f );
                slider->a_IsReadOnly.Set( readOnly );

                ValuePtr value = CreateControl<Value>();
                container->AddChild( value );
                value->a_IsReadOnly.Set( readOnly );

                std::vector<Serializer*> alphaSer;
                std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
                std::vector<Reflect::Element*>::const_iterator end = instances.end();
                for ( ; itr != end; ++itr )
                {
                    SerializerPtr s = new U8Serializer ();

                    uintptr fieldAddress = (uintptr)(*itr) + field->m_Offset;

                    Math::Color4* col = (Math::Color4*)fieldAddress;
                    intptr offsetInField = (intptr)( &col->a ) - fieldAddress;
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
                SliderPtr slider = CreateControl<Slider>();
                container->AddChild( slider );
                slider->a_Min.Set( 0.0 );
                slider->a_Max.Set( 8.0 );
                slider->a_IsReadOnly.Set( readOnly );

                ValuePtr value = CreateControl<Value>();
                container->AddChild( value );
                value->a_IsReadOnly.Set( readOnly );

                std::vector<Serializer*> intensitySer;
                std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
                std::vector<Reflect::Element*>::const_iterator end = instances.end();
                for ( ; itr != end; ++itr )
                {
                    SerializerPtr s = new F32Serializer();

                    uintptr fieldAddress = (uintptr)(*itr) + field->m_Offset;

                    if ( color3 )
                    {
                        Math::HDRColor3* col = (Math::HDRColor3*)fieldAddress;
                        intptr offsetInField = (intptr)( &col->s ) - fieldAddress;
                        s->ConnectField( *itr, field, offsetInField );
                    }

                    if ( color4 )
                    {
                        Math::HDRColor4* col = (Math::HDRColor4*)fieldAddress;
                        intptr offsetInField = (intptr)( &col->s ) - fieldAddress;
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
