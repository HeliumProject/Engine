#include "ReflectBitfieldInterpreter.h"

#include "Foundation/Reflect/Enumeration.h"

#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/DataBinding.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

class MultiBitfieldStringFormatter : public MultiStringFormatter<Data>
{
public:
    MultiBitfieldStringFormatter( const Reflect::EnumerationElement* element, const std::vector<Data*>& data )
        : MultiStringFormatter<Data>( data, false )
        , m_EnumerationElement( element )
    {

    }

    virtual bool Set(const tstring& s, const DataChangedSignature::Delegate& emitter = NULL)
    {
        // get the full string set
        tstring bitSet;
        MultiStringFormatter<Data>::Get( bitSet );

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

        return MultiStringFormatter<Data>::Set( bitSet, emitter );
    }

    virtual void Get(tstring& s) const HELIUM_OVERRIDE
    {
        MultiStringFormatter<Data>::Get( s );

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
    const Reflect::EnumerationElement* m_EnumerationElement;
};

ReflectBitfieldInterpreter::ReflectBitfieldInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectBitfieldInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Object*>& instances, Container* parent)
{
    // If you hit this, you are misusing this interpreter
    HELIUM_ASSERT( field->m_DataClass == Reflect::GetClass<Reflect::BitfieldData>() );

    if ( field->m_DataClass != Reflect::GetClass<Reflect::BitfieldData>() )
    {
        return;
    }

    if ( field->m_Flags & FieldFlags::Hide )
    {
        return;
    }

    // create the container
    ContainerPtr container = CreateControl<Container>();

    tstring temp;
    field->GetProperty( TXT( "UIName" ), temp );
    if ( temp.empty() )
    {
        bool converted = Helium::ConvertString( field->m_Name, temp );
        HELIUM_ASSERT( converted );
    }

    container->a_Name.Set( temp );

    parent->AddChild(container);

    // create the serializers
    std::vector<Reflect::Object*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Object*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        DataPtr s = field->CreateData();
        s->ConnectField(*itr, field);
        m_Datas.push_back(s);
    }

    tstringstream defaultStream;
    DataPtr defaultData = field->CreateDefaultData();
    if ( defaultData )
    {
        *defaultData >> defaultStream;
    }

    const Reflect::Enumeration* enumeration = Reflect::ReflectionCast< Enumeration >( field->m_Type );

    // build the child gui elements
    bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;
    DynArray< Reflect::EnumerationElement >::ConstIterator enumItr = enumeration->m_Elements.Begin();
    DynArray< Reflect::EnumerationElement >::ConstIterator enumEnd = enumeration->m_Elements.End();
    for ( ; enumItr != enumEnd; ++enumItr )
    {
        ContainerPtr row = CreateControl< Container >();
        container->AddChild( row );

        LabelPtr label = CreateControl< Label >();
        label->a_HelpText.Set( enumItr->m_HelpText );
        label->BindText( enumItr->m_Name );
        row->AddChild( label );

        CheckBoxPtr checkbox = CreateControl< CheckBox >();
        checkbox->a_IsReadOnly.Set( readOnly );
        checkbox->a_HelpText.Set( enumItr->m_HelpText );
#pragma TODO("Compute correct default value")
        checkbox->a_Default.Set( defaultStream.str() );
        checkbox->Bind( new MultiBitfieldStringFormatter ( &*enumItr, (std::vector<Reflect::Data*>&)m_Datas ) );
        row->AddChild( checkbox );
    }
}
