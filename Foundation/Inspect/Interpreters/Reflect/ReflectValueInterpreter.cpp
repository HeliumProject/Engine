#include "ReflectValueInterpreter.h"

#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/Data.h"
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

    ContainerPtr container = new Container ();

    bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;

    //
    // Parse
    //

    tstring fieldUI;
    field->GetProperty( TXT( "UIScript" ), fieldUI );
    bool result = Script::Parse(fieldUI, this, parent->GetCanvas(), container, field->m_Flags);

    if (!result)
    {
        if ( field->m_SerializerID == Reflect::GetType<EnumerationSerializer>() )
        {
            ChoicePtr choice = CreateControl<Choice>();

            const EnumerationField* enumInfo = static_cast<const EnumerationField*>(field);

            std::vector< ChoiceItem > items;
            items.resize( enumInfo->m_Enumeration->m_Elements.size() );

            V_EnumerationElement::const_iterator itr = enumInfo->m_Enumeration->m_Elements.begin();
            V_EnumerationElement::const_iterator end = enumInfo->m_Enumeration->m_Elements.end();
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
            if ( field->m_SerializerID == Reflect::GetType<BoolSerializer>() )
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

    if (label == NULL)
    {
        label = CreateControl<Label>();

        tstring temp;
        bool converted = Helium::ConvertString( field->m_UIName, temp );
        HELIUM_ASSERT( converted );

        label->BindText( temp );
        label->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );

        container->InsertChild(0, label);
    }

    //
    // Bind data
    //

    std::vector<Serializer*> ser;

    {
        std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
        std::vector<Reflect::Element*>::const_iterator end = instances.end();
        for ( ; itr != end; ++itr )
        {
            SerializerPtr s = field->CreateSerializer();

            if (!s->HasType(Reflect::GetType<ContainerSerializer>()))
            {
                s->ConnectField(*itr, field);

                ser.push_back(s);

                m_Serializers.push_back(s);
            }
        }
    }

    Helium::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Serializer>( ser );

    container->Bind( data );

    //
    // Set default
    //

    if (field->m_Default != NULL)
    {
        tstringstream outStream;

        *field->m_Default >> outStream;

        container->a_Default.Set( outStream.str() );
    }

    //
    // Close
    //

    parent->AddChild(container);
}