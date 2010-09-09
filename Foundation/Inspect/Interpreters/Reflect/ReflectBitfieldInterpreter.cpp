#include "ReflectBitfieldInterpreter.h"

#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/Data.h"

#include "Foundation/Reflect/Field.h"
#include "Foundation/Reflect/Enumeration.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

class MultiBitfieldStringFormatter : public MultiStringFormatter<Serializer>
{
public:
    MultiBitfieldStringFormatter( Reflect::EnumerationElement* element, const std::vector<Serializer*>& data )
        : MultiStringFormatter<Serializer>( data, false )
        , m_EnumerationElement( element )
    {

    }

    virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL)
    {
        // get the full string set
        tstring bitSet;
        MultiStringFormatter<Serializer>::Get( bitSet );

        if ( s == TXT("1") )
        {
            if ( !bitSet.find_first_of( m_EnumerationElement->m_Name ) )
            {
                if ( bitSet.empty() )
                {
                    bitSet = m_EnumerationElement->m_Name;
                }
                else
                {
                    bitSet += TXT("|") + m_EnumerationElement->m_Name;
                }
            }
        }
        else if ( s == TXT("0") )
        {
            if ( bitSet == m_EnumerationElement->m_Name )
            {
                bitSet.clear();
            }
            else
            {
                size_t pos = bitSet.find_first_of( m_EnumerationElement->m_Name );
                if ( pos != std::string::npos )
                {
                    // remove the bit from the bitfield value
                    bitSet.erase( pos, m_EnumerationElement->m_Name.length() );

                    // cleanup delimiter
                    bitSet.erase( pos, 1 );
                }
            }
        }
        else if ( s == MULTI_VALUE_STRING || s == UNDEF_VALUE_STRING )
        {
            bitSet = s;
        }

        return MultiStringFormatter<Serializer>::Set( bitSet, emitter );
    }

    virtual void Get(tstring& s) const HELIUM_OVERRIDE
    {
        MultiStringFormatter<Serializer>::Get( s );

        if ( s.find_first_of( m_EnumerationElement->m_Name ) != std::string::npos )
        {
            s = TXT("1");
        }
        else
        {
            s = TXT("0");
        }
    }

private:
    Reflect::EnumerationElement* m_EnumerationElement;
};

ReflectBitfieldInterpreter::ReflectBitfieldInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectBitfieldInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
    // If you hit this, you are misusing this interpreter
    HELIUM_ASSERT( field->m_SerializerID == Reflect::GetType<Reflect::BitfieldSerializer>() );

    if ( field->m_SerializerID != Reflect::GetType<Reflect::BitfieldSerializer>() )
    {
        return;
    }

    if ( field->m_Flags & FieldFlags::Hide )
    {
        return;
    }

    const EnumerationField* enumField = static_cast< const EnumerationField* >( field );

    // create the container
    ContainerPtr container = new Container ();

    tstring temp;
    bool converted = Helium::ConvertString( field->m_UIName, temp );
    container->a_Name.Set( temp );
    parent->AddChild(container);

    // create the serializers
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        SerializerPtr s = field->CreateSerializer();
        s->ConnectField(*itr, field);
        m_Serializers.push_back(s);
    }

    tstringstream outStream;
    *field->m_Default >> outStream;

    // build the child gui elements
    bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;
    M_StrEnumerationElement::const_iterator enumItr = enumField->m_Enumeration->m_ElementsByLabel.begin();
    M_StrEnumerationElement::const_iterator enumEnd = enumField->m_Enumeration->m_ElementsByLabel.end();
    for ( ; enumItr != enumEnd; ++enumItr )
    {
        ContainerPtr row = CreateControl< Container >();
        container->AddChild( row );

        LabelPtr label = CreateControl< Label >();
        label->a_HelpText.Set( enumItr->second->m_HelpText );
        label->BindText( enumItr->first );
        row->AddChild( label );

        CheckBoxPtr checkbox = CreateControl< CheckBox >();
        checkbox->a_IsReadOnly.Set( readOnly );
        checkbox->a_HelpText.Set( enumItr->second->m_HelpText );
#pragma TODO("Compute correct default value")
        checkbox->a_Default.Set( outStream.str() );
        checkbox->Bind( new MultiBitfieldStringFormatter ( enumItr->second, (std::vector<Reflect::Serializer*>&)m_Serializers ) );
        row->AddChild( checkbox );
    }
}
