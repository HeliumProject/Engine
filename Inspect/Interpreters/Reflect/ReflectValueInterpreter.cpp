#include "InspectPch.h"
#include "ReflectValueInterpreter.h"

#include "Reflect/Enumeration.h"

#include "Inspect/Inspect.h"
#include "Inspect/DataBinding.h"
#include "Inspect/Script.h"
#include "Inspect/Controls/CheckBoxControl.h"
#include "Inspect/Controls/ChoiceControl.h"
#include "Inspect/Controls/ValueControl.h"
#include "Inspect/Controls/ListControl.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectValueInterpreter::ReflectValueInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectValueInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Object*>& instances, Container* parent)
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
        if ( field->m_DataClass == Reflect::GetClass<EnumerationData>() )
        {
            ChoicePtr choice = CreateControl<Choice>();

            const Reflect::Enumeration* enumeration = Reflect::ReflectionCast< Enumeration >( field->m_Type );

            std::vector< ChoiceItem > items;
            items.resize( enumeration->m_Elements.GetSize() );

            DynamicArray< EnumerationElement >::ConstIterator itr = enumeration->m_Elements.Begin();
            DynamicArray< EnumerationElement >::ConstIterator end = enumeration->m_Elements.End();
            for ( size_t index=0; itr != end; ++itr, ++index )
            {
                ChoiceItem& item = items[index];

                item.m_Key = itr->m_Name;
                item.m_Data = itr->m_Name;
            }

            choice->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
            choice->a_Items.Set( items );
            choice->a_IsDropDown.Set( true );
            choice->a_IsReadOnly.Set( readOnly );

            container->AddChild(choice);
        }
        else
        {
            if ( field->m_DataClass == Reflect::GetClass<BoolData>() )
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
            Label* label = Reflect::SafeCast<Label>( *itr );
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
        std::vector<Reflect::Object*>::const_iterator itr = instances.begin();
        std::vector<Reflect::Object*>::const_iterator end = instances.end();
        for ( ; itr != end; ++itr )
        {
            DataPtr s = field->CreateData();

            if (!s->IsClass(Reflect::GetClass<ContainerData>()))
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

    DataPtr defaultData = field->CreateDefaultData();
    if (defaultData.ReferencesObject())
    {
        tstringstream defaultStream;
        *defaultData >> defaultStream;
        container->a_Default.Set( defaultStream.str() );
    }

    //
    // Close
    //

    parent->AddChild(container);
}