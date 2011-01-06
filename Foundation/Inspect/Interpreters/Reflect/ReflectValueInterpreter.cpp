#include "ReflectValueInterpreter.h"

#include "Foundation/Reflect/Enumeration.h"

#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/DataBinding.h"
#include "Foundation/Inspect/Script.h"
#include "Foundation/Inspect/Controls/CheckBoxControl.h"
#include "Foundation/Inspect/Controls/ChoiceControl.h"
#include "Foundation/Inspect/Controls/ValueControl.h"
#include "Foundation/Inspect/Controls/ListControl.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectValueInterpreter::ReflectValueInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectValueInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
    if (field->m_Flags & FieldFlags::Hide)
    {
        return;
    }

    //
    // Create the ui we are generating
    //

    ContainerPtr container = CreateControl<Container>();

    bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;

    //
    // Parse
    //

    tstring fieldUI;
    field->GetProperty( TXT( "UIScript" ), fieldUI );
    bool result = Script::Parse(fieldUI, this, parent->GetCanvas(), container, field->m_Flags);

    if (!result)
    {
        if ( field->m_DataClass == Reflect::GetType<EnumerationData>() )
        {
            ChoicePtr choice = CreateControl<Choice>();

            const Reflect::Enumeration* enumeration = Reflect::ReflectionCast< Enumeration >( field->m_Type );

            std::vector< ChoiceItem > items;
            items.resize( enumeration->m_Elements.size() );

            V_EnumerationElement::const_iterator itr = enumeration->m_Elements.begin();
            V_EnumerationElement::const_iterator end = enumeration->m_Elements.end();
            for ( size_t index=0; itr != end; ++itr, ++index )
            {
                ChoiceItem& item = items[index];
                bool converted = Helium::ConvertString( (*itr)->m_Label.c_str(), item.m_Key );
                HELIUM_ASSERT( converted );

                converted = Helium::ConvertString( (*itr)->m_Label.c_str(), item.m_Data );
                HELIUM_ASSERT( converted );
            }

            choice->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
            choice->a_Items.Set( items );
            choice->a_IsDropDown.Set( true );
            choice->a_IsReadOnly.Set( readOnly );

            container->AddChild(choice);
        }
        else
        {
            if ( field->m_DataClass == Reflect::GetType<BoolData>() )
            {
                CheckBoxPtr checkBox = CreateControl<CheckBox>();
                checkBox->a_IsReadOnly.Set( readOnly );
                checkBox->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
                container->AddChild( checkBox );
            }
            else
            {
                ValuePtr value = CreateControl<Value>();
                value->a_IsReadOnly.Set( readOnly );
                value->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
                container->AddChild( value );
            }
        }
    }

    //
    // Setup label
    //

    LabelPtr label = NULL;

    {
        V_Control::const_iterator itr = container->GetChildren().begin();
        V_Control::const_iterator end = container->GetChildren().end();
        for( ; itr != end; ++itr )
        {
            Label* label = Reflect::ObjectCast<Label>( *itr );
            if (label)
            {
                break;
            }
        }
    }

    if (!label.ReferencesObject())
    {
        label = CreateControl<Label>();

        tstring temp;
        field->GetProperty( TXT( "UIName" ), temp );
        if ( temp.empty() )
        {
            bool converted = Helium::ConvertString( field->m_Name, temp );
            HELIUM_ASSERT( converted );
        }

        label->BindText( temp );
        label->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );

        container->InsertChild(0, label);
    }

    //
    // Bind data
    //

    std::vector<Data*> ser;

    {
        std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
        std::vector<Reflect::Element*>::const_iterator end = instances.end();
        for ( ; itr != end; ++itr )
        {
            DataPtr s = field->CreateData();

            if (!s->HasType(Reflect::GetType<ContainerData>()))
            {
                s->ConnectField(*itr, field);

                ser.push_back(s);

                m_Datas.push_back(s);
            }
        }
    }

    Helium::SmartPtr< MultiStringFormatter<Data> > data = new MultiStringFormatter<Data>( ser );

    container->Bind( data );

    //
    // Set default
    //

#ifdef REFLECT_REFACTOR
    if (field->m_Default.ReferencesObject())
    {
        tstringstream outStream;
        *field->m_Default >> outStream;
        container->a_Default.Set( outStream.str() );
    }
#endif

    //
    // Close
    //

    parent->AddChild(container);
}