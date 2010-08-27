#include "ReflectArrayInterpreter.h"
#include "InspectReflectInit.h"

#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectValue.h"
#include "Application/Inspect/Controls/InspectChoice.h"
#include "Application/Inspect/Controls/InspectList.h"
#include "Application/Inspect/Controls/InspectButton.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/InspectInit.h"

#include "Foundation/String/Tokenize.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectArrayInterpreter::ReflectArrayInterpreter (Container* labelContainer)
: ReflectFieldInterpreter (labelContainer)
{

}

void ReflectArrayInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
    if (field->m_Flags & FieldFlags::Hide)
    {
        return;
    }

    // create the label
    ContainerPtr labelContainer = new Container ();
    parent->AddChild( labelContainer );
    LabelPtr label = CreateControl< Label >();
    labelContainer->AddChild( label );
    tstring temp;
    bool converted = Helium::ConvertString( field->m_UIName, temp );
    HELIUM_ASSERT( converted );
    label->BindText( temp );

    // create the list view
    ContainerPtr listContainer = new Container ();
    parent->AddChild( listContainer );
    ListPtr list = CreateControl<List>();
    listContainer->AddChild( list );

    // create the buttons
    ButtonPtr addButton;
    ButtonPtr removeButton;
    ButtonPtr upButton;
    ButtonPtr downButton;
    if ( !(field->m_Flags & FieldFlags::ReadOnly) )
    {
        addButton = AddAddButton( list );
        removeButton = AddRemoveButton( list );
        upButton = AddMoveUpButton( list );
        downButton = AddMoveDownButton( list );
    }

    // add the buttons to the container
    ContainerPtr buttonContainer = new Container ();
    parent->AddChild( buttonContainer );
    if ( addButton )
    {
        buttonContainer->AddChild( addButton );
    }
    if ( removeButton )
    {
        buttonContainer->AddChild( removeButton );
    }
    if ( upButton )
    {
        buttonContainer->AddChild( upButton );
    }
    if ( downButton )
    {
        buttonContainer->AddChild( downButton );
    }

    // create the serializers
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        SerializerPtr s = field->CreateSerializer();

        OnCreateFieldSerializer( s );

        s->ConnectField(*itr, field);

        m_Serializers.push_back(s);
    }

    // bind the ui to the serializers
    Helium::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Reflect::Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers );
    list->Bind( data );

    // setup the default value
    if (field->m_Default != NULL)
    {
        tstringstream outStream;
        *field->m_Default >> outStream;
        list->a_Default.Set( outStream.str() );
    }
}

ButtonPtr ReflectArrayInterpreter::AddAddButton( List* list )
{
    ButtonPtr addButton =CreateControl< Button >();
    addButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectArrayInterpreter::OnAdd ) );
    addButton->SetClientData( new ClientData( list ) );
    addButton->a_Label.Set( TXT( "Add" ) );

    return addButton;
}

ButtonPtr ReflectArrayInterpreter::AddRemoveButton( List* list )
{
    ButtonPtr removeButton = CreateControl< Button >();
    removeButton->a_Label.Set( TXT( "Remove" ) );
    removeButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectArrayInterpreter::OnRemove ) );
    removeButton->SetClientData( new ClientData( list ) );

    return removeButton;
}

ButtonPtr ReflectArrayInterpreter::AddMoveUpButton( List* list )
{
    ButtonPtr upButton = CreateControl< Button >();
    upButton->a_Icon.Set( TXT( "actions/go-up" ) );
    upButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectArrayInterpreter::OnMoveUp ) );
    upButton->SetClientData( new ClientData( list ) );

    return upButton;
}

ButtonPtr ReflectArrayInterpreter::AddMoveDownButton( List* list )
{
    ButtonPtr downButton = CreateControl< Button >();
    downButton->a_Icon.Set( TXT( "actions/go-down" ) );
    downButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectArrayInterpreter::OnMoveDown ) );
    downButton->SetClientData( new ClientData( list ) );

    return downButton;
}

void ReflectArrayInterpreter::OnAdd( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        list->e_AddItem.Raise( AddItemArgs() );
        args.m_Control->GetCanvas()->Read();
    }
}

void ReflectArrayInterpreter::OnRemove( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        const std::set< size_t >& selectedItemIndices = list->a_SelectedItemIndices.Get();
        if ( !selectedItemIndices.empty() )
        {
            // for each item in the array to remove (by index)
            std::set< size_t >::const_reverse_iterator itr = selectedItemIndices.rbegin();
            std::set< size_t >::const_reverse_iterator end = selectedItemIndices.rend();
            for ( ; itr != end; ++itr )
            {
                // for each array in the selection set (the objects the array serializer is connected to)
                std::vector< SerializerPtr >::const_iterator serItr = m_Serializers.begin();
                std::vector< SerializerPtr >::const_iterator serEnd = m_Serializers.end();
                for ( ; serItr != serEnd; ++serItr )
                {
                    Reflect::ArraySerializer* arraySerializer = Reflect::AssertCast<Reflect::ArraySerializer>(*serItr);

                    arraySerializer->Remove( *itr );
                }
            }

            list->a_SelectedItemIndices.Set( std::set< size_t > () );

            args.m_Control->GetCanvas()->Read();
        }
    }
}

void ReflectArrayInterpreter::OnMoveUp( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        std::set< size_t > selectedItemIndices = list->a_SelectedItemIndices.Get();
        if ( !selectedItemIndices.empty() )
        {
            // for each array in the selection set (the objects the array serializer is connected to)
            std::vector< SerializerPtr >::const_iterator serItr = m_Serializers.begin();
            std::vector< SerializerPtr >::const_iterator serEnd = m_Serializers.end();
            for ( ; serItr != serEnd; ++serItr )
            {
                Reflect::ArraySerializer* arraySerializer = Reflect::AssertCast<Reflect::ArraySerializer>(*serItr);

                arraySerializer->MoveUp( selectedItemIndices );
            }

            list->a_SelectedItemIndices.Set( selectedItemIndices );

            args.m_Control->GetCanvas()->Read();
        }
    }
}

void ReflectArrayInterpreter::OnMoveDown( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        std::set< size_t > selectedItemIndices = list->a_SelectedItemIndices.Get();
        if ( !selectedItemIndices.empty() )
        {
            // for each array in the selection set (the objects the array serializer is connected to)
            std::vector< SerializerPtr >::const_iterator serItr = m_Serializers.begin();
            std::vector< SerializerPtr >::const_iterator serEnd = m_Serializers.end();
            for ( ; serItr != serEnd; ++serItr )
            {
                Reflect::ArraySerializer* arraySerializer = Reflect::AssertCast<Reflect::ArraySerializer>(*serItr);

                arraySerializer->MoveDown( selectedItemIndices );
            }

            list->a_SelectedItemIndices.Set( selectedItemIndices );

            args.m_Control->GetCanvas()->Read();
        }
    }
}